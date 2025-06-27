#include "pch.h"
#include "Teams.hpp"


// ##############################################################################################################
// ###############################################    INIT    ###################################################
// ##############################################################################################################

void TeamsComponent::Initialize(const std::shared_ptr<GameWrapper>& gw)
{
	gameWrapper = gw;

	initCvars();
	initHooks();
	initCommands();
}

// clang-format off
void TeamsComponent::initCvars()
{
	auto updateColors = [this](std::string name, CVarWrapper updatedCvar)
	{
		GAME_THREAD_EXECUTE(
			applyColors();
		);
	};

	// bools
	auto useCustomTeamColors_cvar = registerCvar_bool(Cvars::use_custom_team_colors, true);
	useCustomTeamColors_cvar.bindTo(m_useCustomTeamColors);
	useCustomTeamColors_cvar.addOnValueChanged(updateColors);

	auto useSingleFreeplayColor_cvar = registerCvar_bool(Cvars::use_single_freeplay_color, false);
	useSingleFreeplayColor_cvar.bindTo(m_useSingleFreeplayColor);
	useSingleFreeplayColor_cvar.addOnValueChanged(updateColors);

	auto useRGBFreeplayColors_cvar = registerCvar_bool(Cvars::use_rgb_freeplay_colors, false);
	useRGBFreeplayColors_cvar.bindTo(m_useRGBFreeplayColors);
	useRGBFreeplayColors_cvar.addOnValueChanged(updateColors);

	auto useCustomTeamNames_cvar = registerCvar_bool(Cvars::use_custom_team_names, true);
	useCustomTeamNames_cvar.bindTo(m_useCustomTeamNames);
	useCustomTeamNames_cvar.addOnValueChanged([this](...)
		{
			GAME_THREAD_EXECUTE(
				changeNamesFromGameEvent();
			);
		});

	// strings
	auto blueName_cvar = registerCvar_string(Cvars::blue_team_name, "crips");
	blueName_cvar.bindTo(m_blueName);
	blueName_cvar.addOnValueChanged([this](std::string name, CVarWrapper updatedCvar)
	{
		std::string updatedVal = updatedCvar.getStringValue();

		GAME_THREAD_EXECUTE_CAPTURE(
			changeTeamNames(updatedVal, *m_orangeName);
		, updatedVal);

		DEBUGLOG("{} changed...", name);
	});

	auto orangeName_cvar = registerCvar_string(Cvars::orange_team_name, "bloods");
	orangeName_cvar.bindTo(m_orangeName);
	orangeName_cvar.addOnValueChanged([this](std::string name, CVarWrapper updatedCvar)
	{
		std::string updatedVal = updatedCvar.getStringValue();

		GAME_THREAD_EXECUTE_CAPTURE(
			changeTeamNames(*m_blueName, updatedVal);
		, updatedVal);

		DEBUGLOG("{} changed...", name);
	});

	// numbers
	registerCvar_number(Cvars::rgb_speed, 0, true, -14, 9).bindTo(m_rgbSpeed);

	// colors
	auto blueColor_cvar = registerCvar_color(Cvars::blue_team_color, "#FF00BC");
	blueColor_cvar.bindTo(m_blueColor);

	auto orangeColor_cvar = registerCvar_color(Cvars::orange_team_color, "#FFF800");
	orangeColor_cvar.bindTo(m_orangeColor);

	auto solidFreeplayColor_cvar = registerCvar_color(Cvars::single_freeplay_color, "#FF2222");
	solidFreeplayColor_cvar.bindTo(m_solidFreeplayColor);
	solidFreeplayColor_cvar.addOnValueChanged([this](std::string name, CVarWrapper updatedCvar)
	{
		if (!gameWrapper->IsInFreeplay())
			return;

		GAME_THREAD_EXECUTE(
			applySolidFreeplayColor();
		);
	});
}

void TeamsComponent::initHooks()
{
	// for freeplay colors
	hookWithCaller(Events::Team_TA_SetColorList, [this](ActorWrapper Caller, void* Params, ...)
	{
		// freeplay seems to be the only place we need to override SetColorList params (editing archetypes doesnt work for whatever reason)
		if (!gameWrapper->IsInFreeplay())
			return;

		auto caller = reinterpret_cast<ATeam_TA*>(Caller.memory_address);
		if (!validUObject(caller))
			return;

		auto params = reinterpret_cast<ATeam_TA_execSetColorList_Params*>(Params);
		if (!params)
			return;

		// determine color
		FLinearColor newColor{};
		if (*m_useRGBFreeplayColors)
			return;
		else if (*m_useSingleFreeplayColor)
			newColor = Colors::CvarColorToFLinearColor(*m_solidFreeplayColor);
		else if (*m_useCustomTeamColors)
			newColor = Colors::CvarColorToFLinearColor(caller->TeamIndex == 0 ? *m_blueColor : *m_orangeColor);
		else
		{
			if (!m_backedUpOgBlueColor || !m_backedUpOgOrangeColor)
				backupOgColors();
			return;
		}

		// edit param colors
		for (auto& col : params->ColorList)
			col = newColor;

		LOG("Changed SetColorList() params for team {}", Format::ToHexString(caller));
	});

	// for freeplay RGB
	hookWithCallerPost(Events::GFxHUD_TA_Tick, [this](ActorWrapper Caller, ...)
	{
		if (!*m_useRGBFreeplayColors || !gameWrapper->IsInFreeplay())
			return;

		tickRGB();

		auto hud = reinterpret_cast<AGFxHUD_TA*>(Caller.memory_address);
		if (!validUObject(hud) || !validUObject(hud->TeamGameEvent))
			return;

		applyRgbColor(hud->TeamGameEvent);
	});

	// team names
	hookWithCaller(Events::TeamNameComponent_TA_GetTeamName, [this](ActorWrapper Caller, void* Params, ...)
	{
		if (!*m_useCustomTeamNames)
			return;

		EGameStates state = getGameState();
		if (state == EGameStates::Menu || state == EGameStates::Freeplay)
			return;

		auto* caller = reinterpret_cast<UTeamNameComponent_TA*>(Caller.memory_address);
		if (!validUObject(caller))
			return;
	
		auto* params = reinterpret_cast<UTeamNameComponent_TA_execGetTeamName_Params*>(Params);
		if (!params)
			return;
	
		const auto&  teamNum      = caller->TeamIndex;
		std::string  existingName = caller->SanitizedTeamName.ToString();
		const auto&  customName   = teamNum == 0 ? *m_blueName : *m_orangeName;
		std::string& ogNameCache  = teamNum == 0 ? m_ogBlueName : m_ogOrangeName;

		// save OG name if necessary
		/*
			NOTE:
			We keep trying to update ogNameCache on every GetTeamName call, even if SanitizedTeamName is empty or censored at first,
			bc SanitizedTeamName wont contain the actual club name (if it exists) on the first calls, but might on later calls
		*/
		if (!existingName.empty() && existingName != customName)
		{
			// if OG name is empty, or (likely) censored: update cached OG name
			if (ogNameCache.empty() || ogNameCache.find("*") == 0)
				ogNameCache = existingName;
		}
			
		// o p t i m i z a t i o n  (comes after the code above for updating ogNameCache on purpose)
		if (existingName == customName)
			return;
			
		caller->SanitizedTeamName = FString::create(customName);
		LOG("Modified SanitizedTeamName from TeamNameComponent_TA::GetTeamName pre hook");
	});
}

void TeamsComponent::initCommands()
{
	registerCommand(Commands::apply_team_colors, [this](std::vector<std::string>)
	{
		applyColors();
	});

	registerCommand(Commands::apply_team_names, [this](std::vector<std::string>) { changeNamesFromGameEvent(); });
}
// clang-format on



// ##############################################################################################################
// ###############################################    FUNCTIONS    ##############################################
// ##############################################################################################################

// top level function for manually applying colors
void TeamsComponent::applyColors()
{
	EGameStates state = getGameState();

	if (state == EGameStates::Freeplay)
	{
		if (*m_useRGBFreeplayColors)	// just return, bc GFxHUD_TA::Tick() hook already handles applying RGB colors
			return;

		if (*m_useSingleFreeplayColor)
			applySolidFreeplayColor();	// apply solid freeplay color
		else if (*m_useCustomTeamColors)
			applyCustomColors(state);	// apply custom team colors
		else
			applyOgColors(state);		// apply OG colors
	}
	else
	{
		if (*m_useCustomTeamColors)
			applyCustomColors(state);	// apply custom team colors
		else
			applyOgColors(state);		// apply OG colors
	}
}

void TeamsComponent::applyRgbColor(AGameEvent_Team_TA* event)
{
	if (!validUObject(event))
		return;

	auto act = Instances.getSetStadiumTeamColorsSeqAct();
	if (!act)
	{
		LOG("ERROR: Unable to get valid USeqAct_SetStadiumTeamColors_TA instance");
		return;
	}

	FLinearColor customCol = GRainbowColor::GetFLinear();
	for (ATeam_TA* team : event->Teams)
		changeColorLiveOutsideMatch(team, customCol, act);
}

void TeamsComponent::applySolidFreeplayColor()
{
	AGameEvent_Team_TA* event = Instances.getTeamGameEvent();
	if (!event)
	{
		LOG("ERROR: Unable to get current AGameEvent_Team_TA instance");
		return;
	}

	auto act = Instances.getSetStadiumTeamColorsSeqAct();
	if (!act)
	{
		LOG("ERROR: Unable to get valid USeqAct_SetStadiumTeamColors_TA instance");
		return;
	}

	FLinearColor customCol = Colors::CvarColorToFLinearColor(*m_solidFreeplayColor);

	for (ATeam_TA* team : event->Teams)
		changeColorLiveOutsideMatch(team, customCol, act);
}

void TeamsComponent::applyCustomColors(EGameStates state)
{
	applyColorsToArchetypes(); // also saves OG colors if necessary

	auto allTeams = Instances.GetAllInstancesOf<ATeam_TA>();
	LOG("Number of regular ATeam_TA instances found: {}", allTeams.size());

	for (ATeam_TA* team : allTeams)
	{
		FLinearColor newColor = Colors::CvarColorToFLinearColor(team->TeamIndex == 0 ? *m_blueColor : *m_orangeColor);

		if (state == EGameStates::Freeplay || state == EGameStates::Menu)
			changeColorLiveOutsideMatch(team, newColor);
		else
			changeColorLiveInMatch(team, newColor);
	}
}

void TeamsComponent::applyOgColors(EGameStates state)
{
	if (!m_backedUpOgBlueColor || !m_backedUpOgOrangeColor)
		backupOgColors();

	applyColorsToArchetypes();

	auto allTeams = Instances.GetAllInstancesOf<ATeam_TA>();
	LOG("Number of regular ATeam_TA instances found: {}", allTeams.size());

	for (ATeam_TA* team : allTeams)
	{
		OgTeamColorBackup& ogColors = team->TeamIndex == 0 ? m_ogBlueColor : m_ogOrangeColor;

		if (state == EGameStates::Freeplay || state == EGameStates::Menu)
		{
			changeColorLiveOutsideMatch(team, ogColors.defaultColors);
			LOG("Applied OG colors live outside match");
		}
		else
		{
			changeColorLiveInMatch(team, ogColors.defaultColors);
			LOG("Applied OG colors live in match");
		}
	}
}


void TeamsComponent::applyColorsToArchetypes(AGameEvent_Team_TA* event)
{
	if (!m_backedUpOgBlueColor && !m_backedUpOgOrangeColor)
		backupOgColors();

	if (event)
	{
		for (ATeam_TA* teamArch : event->TeamArchetypes)
			applyColorsToArchetype(teamArch);
	}
	else
	{
		LOG("Unable to get a valid AGameEvent_Team_TA instance. Manually searching for ATeam_TA archetypes...");

		auto teamArchs = Instances.GetAllArchetypeInstancesOf<ATeam_TA>();
		LOG("{} ATeam_TA archetype instances found", teamArchs.size());

		for (ATeam_TA* teamArch : teamArchs)
			applyColorsToArchetype(teamArch);
	}
}

void TeamsComponent::applyColorsToArchetype(ATeam_TA* team)
{
	if (!validUObject(team))
	{
		LOG("ERROR: ATeam_TA* is invalid");
		return;
	}

	const auto& teamNum = team->TeamIndex;
	if (*m_useCustomTeamColors && !m_inUnload)
	{
		// apply custom color
		FLinearColor customCol = getCustomColor(teamNum);

		for (auto& col : team->DefaultColorList)
			col = customCol;

		for (auto& col : team->CurrentColorList)
			col = customCol;

		team->FontColor = customCol;

		LOG("Applied custom color to {} team archetype {}", teamNum == 0 ? "Blue" : "Orange", Format::ToHexString(team));
	}
	else
	{
		if (!m_backedUpOgBlueColor || !m_backedUpOgOrangeColor)
		{
			LOG("ERROR: Unable to apply OG colors, they haven't been saved");
			return;
		}

		// apply OG colors
		OgTeamColorBackup& ogColors = teamNum == 0 ? m_ogBlueColor : m_ogOrangeColor;

		for (int i = 0; i < team->DefaultColorList.size(); ++i)
			team->DefaultColorList[i] = ogColors.defaultColors[i];

		team->FontColor = ogColors.fontColor;

		// skip freeplay team archetypes here, bc they have an empty CurrentColorList
		if (!team->IsA<ATeam_Freeplay_TA>())
		{
			if (team->CurrentColorList.size() != ogColors.currentColors.size())
			{
				LOG("ERROR: Team CurrentColorList size ({}) doesn't match saved OG currentColors size ({})",
				    team->CurrentColorList.size(),
				    ogColors.currentColors.size());
			}

			for (int i = 0; i < team->CurrentColorList.size(); ++i)
				team->CurrentColorList[i] = ogColors.currentColors[i];
		}

		LOG("Applied OG colors to {} team archetype {}", teamNum == 0 ? "Blue" : "Orange", Format::ToHexString(team));
	}
}

void TeamsComponent::backupOgColors()
{
	auto archetypes = Instances.GetAllArchetypeInstancesOf<ATeam_TA>();

	LOG("{} ATeam_TA archetype instances found", archetypes.size());
	for (ATeam_TA* teamArch : archetypes)
	{
		if (teamArch->IsA<ATeam_Freeplay_TA>()) // skip freeplay team archtypes which dont have the blue/orange colors
			continue;

		backupOgColorsForTeam(teamArch);
	}
}

void TeamsComponent::backupOgColorsForTeam(const ATeam_TA* team)
{
	if (!validUObject(team))
	{
		LOG("ERROR: ATeam_TA* is invalid");
		return;
	}

	const auto&           teamNum = team->TeamIndex;
	OgTeamColorBackup& backup  = teamNum == 0 ? m_ogBlueColor : m_ogOrangeColor;

	if (team->DefaultColorList.size() != COLOR_ARRAY_SIZE)
	{
		LOG("ERROR: team->DefaultColorList size ({}) isn't {}", team->DefaultColorList.size(), COLOR_ARRAY_SIZE);
		return;
	}

	// save default colors
	for (int i = 0; i < team->DefaultColorList.size(); ++i)
		backup.defaultColors[i] = team->DefaultColorList[i];

	// save current colors
	for (int i = 0; i < team->CurrentColorList.size(); ++i)
		backup.currentColors[i] = team->CurrentColorList[i];

	// save font color (HUD color)
	backup.fontColor = team->FontColor;

	LOG("Saved OG colors for {} team", teamNum == 0 ? "Blue" : "Orange");
	if (teamNum == 0)
		m_backedUpOgBlueColor = true;
	else if (teamNum == 1)
		m_backedUpOgOrangeColor = true;
}

void TeamsComponent::restoreOgColorsInUnload()
{
	m_inUnload = true;
	EGameStates state = getGameState();
	applyOgColors(state);
}


FLinearColor TeamsComponent::getCustomColor(int teamNum)
{
	LinearColor& customCol = teamNum == 0 ? *m_blueColor : *m_orangeColor;
	return Colors::CvarColorToFLinearColor(customCol);
}

void TeamsComponent::tickRGB()
{
	GRainbowColor::TickRGB(*m_rgbSpeed, DEFAULT_RGB_SPEED);
}


// ################################### live color changing ####################################

void TeamsComponent::changeColorLiveInMatch(ATeam_TA* team, const FLinearColor& color, bool log)
{
	if (!validUObject(team))
		return;

	if (log)
	{
		LOG("Finna apply this color:");
		LOG(Colors::logColor(color));
	}

	// change current colors
	for (auto& col : team->CurrentColorList)
		col = color;

	team->UpdateColors(); // updates HUD colors live
	

	// change default colors
	for (auto& col : team->DefaultColorList)
		col = color;

	 team->SetDefaultColors(); // updates field colors live

	if (log)
		LOG("Called UpdateColors() for team {}", Format::ToHexString(team));
}

void TeamsComponent::changeColorLiveInMatch(ATeam_TA* team, const std::array<FLinearColor, 3>& colors, bool log)
{
	if (!validUObject(team))
		return;

	// change current colors
	if (team->CurrentColorList.size() != COLOR_ARRAY_SIZE)
	{
		LOG("ERROR: Team's CurrentColorList size ({}) isn't {}", team->CurrentColorList.size(), COLOR_ARRAY_SIZE);
		return;
	}
	
	if (log)
	{
		LOG("Finna apply these colors:");
		for (int i = 0; i < COLOR_ARRAY_SIZE; ++i)
			LOG("[{}] {}", i, Colors::logColor(colors[i]));
	}

	for (int i = 0; i < team->CurrentColorList.size(); ++i)
		team->CurrentColorList[i] = colors[i];

	team->UpdateColors(); // updates HUD colors live


	// change default colors
	if (team->DefaultColorList.size() != COLOR_ARRAY_SIZE)
	{
		LOG("ERROR: Team's DefaultColorList size ({}) isn't {}", team->CurrentColorList.size(), COLOR_ARRAY_SIZE);
		return;
	}

	for (int i = 0; i < team->DefaultColorList.size(); ++i)
		team->DefaultColorList[i] = colors[i];

	 team->SetDefaultColors(); // updates field colors live

	if (log)
		LOG("Called UpdateColors() for team {}", Format::ToHexString(team));
}

void TeamsComponent::changeColorLiveOutsideMatch(
    ATeam_TA* team, const FLinearColor& color, USeqAct_SetStadiumTeamColors_TA* act, bool log)
{
	if (!validUObject(team))
		return;

	if (!validUObject(act))
	{
		act = Instances.getSetStadiumTeamColorsSeqAct();
		if (!act)
		{
			LOG("ERROR: Unable to get valid USeqAct_SetStadiumTeamColors_TA instance");
			return;
		}
	}

	for (auto& col : team->CurrentColorList)
		col = color;

	act->SwapTeamArchetypeColors(team, team->CurrentColorList); // changes field colors in freeplay & some matches (depending on the map)

	if (log)
		LOG("Applied single color to {} team [{}]", team->TeamIndex == 0 ? "Blue" : "Orange", Format::ToHexString(team));
}

void TeamsComponent::changeColorLiveOutsideMatch(
    ATeam_TA* team, const std::array<FLinearColor, 3>& colors, USeqAct_SetStadiumTeamColors_TA* act, bool log)
{
	if (!validUObject(team))
		return;

	if (!validUObject(act))
	{
		act = Instances.getSetStadiumTeamColorsSeqAct();
		if (!act)
		{
			LOG("ERROR: Unable to get valid USeqAct_SetStadiumTeamColors_TA instance");
			return;
		}
	}

	if (team->CurrentColorList.size() != COLOR_ARRAY_SIZE)
	{
		LOG("ERROR: Team's CurrentColorList size ({}) isn't {}", team->CurrentColorList.size(), COLOR_ARRAY_SIZE);
		return;
	}

	for (int i = 0; i < team->CurrentColorList.size(); ++i)
		team->CurrentColorList[i] = colors[i];

	act->SwapTeamArchetypeColors(team, team->CurrentColorList); // changes field colors in freeplay & some matches (depending on the map)

	if (log)
		LOG("Applied colors to {} team [{}]", team->TeamIndex == 0 ? "Blue" : "Orange", Format::ToHexString(team));
}


// ######################################## team names ########################################

void TeamsComponent::clearOgNames()
{
	m_ogBlueName.clear();
	m_ogOrangeName.clear();
	LOG("Cleared OG team names...");
}

void TeamsComponent::changeTeamNames(const std::string& blueTeamName, const std::string& orangeTeamName)
{
	auto* ds = getDatastore();
	if (!ds)
		return;

	for (int teamNum = 0; teamNum < 2; ++teamNum)
	{
		const std::string& newName = teamNum == 0 ? blueTeamName : orangeTeamName;

		// works for team names displayed on HUD, but doesn't affect "X team scored" goal scored messages
		ds->SetStringValue(L"TeamInfo", teamNum, L"TeamName", FString::create(newName));
	}
}

void TeamsComponent::changeNamesFromGameEvent(AGameEvent_Team_TA* gameEvent)
{
	if (!validUObject(gameEvent))
	{
		gameEvent = Instances.getTeamGameEvent();
		if (!validUObject(gameEvent))
			return;
	}

	UGFxDataStore_X* ds = getDatastore(gameEvent);
	if (!ds)
	{
		LOG("ERROR: Unable to get UGFxDataStore_X instance from game event");
		return;
	}

	for (ATeam_TA* team : gameEvent->Teams)
	{
		if (!validUObject(team))
			continue;

		const auto& teamIndex = team->TeamIndex;
		FString     newName;

		// determine name to apply
		if (*m_useCustomTeamNames)
			newName = FString::create(teamIndex == 0 ? *m_blueName : *m_orangeName);
		else
		{
			const std::string& cachedOgName = teamIndex == 0 ? m_ogBlueName : m_ogOrangeName;
			if (cachedOgName.empty())
			{
				auto* matchType = Instances.getMatchType();
				if (!matchType)
					return; // return bc there's no point in trying to apply team names without being in a match (idk how we'd get here tho)

				EMatchType type = getMatchType(matchType);
				switch (type)
				{
				case EMatchType::Lan:
				case EMatchType::Private:
					newName = FString::create(teamIndex == 0 ? "HOME" : "AWAY");
					break;
				default:
					newName = FString::create(teamIndex == 0 ? "BLUE" : "ORANGE");
					break;
				}
			}
			else
				newName = FString::create(cachedOgName);
		}

		// apply new name
		ds->SetStringValue(L"TeamInfo", teamIndex, L"TeamName", newName);

		LOG("Set team name: {}", newName.ToString());
	}
}

UGFxDataStore_X* TeamsComponent::getDatastore(AGameEvent_TA* event)
{
	if (!validUObject(event))
		return nullptr;

	if (event->LocalPlayers.empty() || !validUObject(event->LocalPlayers[0]))
		return nullptr;

	APlayerController_TA* pc = event->LocalPlayers[0];
	if (!validUObject(pc->myHUD) || !pc->myHUD->IsA<AHUDBase_TA>())
		return nullptr;

	auto* hud = static_cast<AHUDBase_TA*>(pc->myHUD);
	if (!validUObject(hud->Shell) || !validUObject(hud->Shell->DataStore))
		return nullptr;

	return hud->Shell->DataStore;
}

UGFxDataStore_X* TeamsComponent::getDatastore()
{
	APlayerController* pc = Instances.getPlayerController();
	if (!pc)
		return nullptr;

	if (!validUObject(pc->myHUD) || !pc->myHUD->IsA<AHUDBase_TA>())
		return nullptr;

	auto* hud = static_cast<AHUDBase_TA*>(pc->myHUD);
	if (!validUObject(hud->Shell) || !validUObject(hud->Shell->DataStore))
		return nullptr;

	return hud->Shell->DataStore;
}



// ##############################################################################################################
// ##########################################    DISPLAY FUNCTIONS    ###########################################
// ##############################################################################################################

void TeamsComponent::display()
{
	auto blueTeamColor_cvar          = getCvar(Cvars::blue_team_color);
	auto orangeTeamColor_cvar        = getCvar(Cvars::orange_team_color);
	auto useCustomTeamNames_cvar     = getCvar(Cvars::use_custom_team_names);
	auto blueTeamName_cvar           = getCvar(Cvars::blue_team_name);
	auto orangeTeamName_cvar         = getCvar(Cvars::orange_team_name);
	auto useCustomTeamColors_cvar    = getCvar(Cvars::use_custom_team_colors);
	auto useSingleFreeplayColor_cvar = getCvar(Cvars::use_single_freeplay_color);
	auto singleFreeplayColor_cvar    = getCvar(Cvars::single_freeplay_color);
	auto useRGBFreeplayColors_cvar   = getCvar(Cvars::use_rgb_freeplay_colors);
	auto rgbSpeed_cvar               = getCvar(Cvars::rgb_speed);

	if (!blueTeamColor_cvar) // if any is null, they prolly all are...
		return;

	const float team_names_height = ImGui::GetContentRegionAvail().y * 0.3f; // 30% available height

	{
		GUI::ScopedChild c{"teamNames", ImVec2(0, team_names_height), true};

		GUI::Spacing(2);

		// enable custom team names checkbox
		bool useCustomTeamNames = useCustomTeamNames_cvar.getBoolValue();
		if (ImGui::Checkbox("Custom team names", &useCustomTeamNames))
		{
			useCustomTeamNames_cvar.setValue(useCustomTeamNames);
		}

		if (useCustomTeamNames)
		{
			GUI::Spacing(2);

			// custom blue team name
			std::string blueTeamName = blueTeamName_cvar.getStringValue();
			ImGui::SetNextItemWidth(ITEM_WIDTH);
			if (ImGui::InputText("Blue", &blueTeamName))
			{
				blueTeamName_cvar.setValue(blueTeamName);
			}

			GUI::SameLineSpacing_relative(SAMELINE_SPACING);

			// custom orange team name
			std::string orangeTeamName = orangeTeamName_cvar.getStringValue();
			ImGui::SetNextItemWidth(ITEM_WIDTH);
			if (ImGui::InputText("Orange", &orangeTeamName))
			{
				orangeTeamName_cvar.setValue(orangeTeamName);
			}

			GUI::Spacing(4);

			if (!m_ogBlueName.empty() || !m_ogOrangeName.empty())
			{
				if (ImGui::CollapsingHeader("Original team names"))
				{
					constexpr float NAME_SPACING = 200.0f;

					ImGui::TextColored(GUI::Colors::Yellow, "Blue:");

					GUI::SameLineSpacing_absolute(NAME_SPACING);

					ImGui::TextColored(GUI::Colors::Yellow, "Orange:");

					ImGui::TextUnformatted(m_ogBlueName.c_str());

					GUI::SameLineSpacing_absolute(NAME_SPACING);

					ImGui::TextUnformatted(m_ogOrangeName.c_str());

					GUI::Spacing(2);
				}
			}
		}
	}

	{
		GUI::ScopedChild c{"teamColors", ImGui::GetContentRegionAvail(), true};

		const float match_colors_width = ImGui::GetContentRegionAvail().x * 0.5f;

		{
			GUI::ScopedChild d{"matchTeamColors", ImVec2(match_colors_width, 0), true};

			ImGui::TextColored(GUI::Colors::Yellow, "Matches");

			GUI::Spacing(2);

			// enable custom team colors checkbox
			bool useCustomTeamColors = useCustomTeamColors_cvar.getBoolValue();
			if (ImGui::Checkbox("Custom team colors", &useCustomTeamColors))
			{
				useCustomTeamColors_cvar.setValue(useCustomTeamColors);
			}

			if (useCustomTeamColors)
			{
				GUI::Spacing(2);

				// blue team color
				LinearColor blueTeamFieldColor = blueTeamColor_cvar.getColorValue() / 255; // converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("Blue##FieldColor", &blueTeamFieldColor.R, ImGuiColorEditFlags_NoInputs))
				{
					blueTeamColor_cvar.setValue(blueTeamFieldColor * 255);
				}

				GUI::Spacing(2);

				// orange team color
				LinearColor orangeTeamFieldColor = orangeTeamColor_cvar.getColorValue() / 255; // converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("Orange##FieldColor", &orangeTeamFieldColor.R, ImGuiColorEditFlags_NoInputs))
				{
					orangeTeamColor_cvar.setValue(orangeTeamFieldColor * 255);
				}

				GUI::Spacing(2);

				if (ImGui::Button("Apply##custom_team_colors"))
				{
					GAME_THREAD_EXECUTE(
					    applyColors();
					);
				}
			}

			GUI::Spacing(4);

			if (m_backedUpOgBlueColor || m_backedUpOgOrangeColor)
			{
				if (ImGui::CollapsingHeader("Cached original colors"))
				{
					static auto displayOgColors = [this](OgTeamColorBackup& col)
					{
						ImGui::TextUnformatted("FontColor:");
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("HUD color");

						ImGui::ColorEdit4("", &col.fontColor.R, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs);

						ImGui::Spacing();

						ImGui::TextUnformatted("DefaultColorList:");
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Stadium & egg colors");

						for (int i = 0; i < col.defaultColors.size(); ++i)
						{
							GUI::ScopedID id{i};

							std::string label = std::to_string(i);
							ImGui::ColorEdit4(label.c_str(), &col.defaultColors[i].R,
							    ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs);

							if (i < col.defaultColors.size() - 1)
								ImGui::SameLine();
						}

						ImGui::Spacing();

						ImGui::TextUnformatted("CurrentColorList:");
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Stadium & egg colors");

						for (int i = 0; i < col.currentColors.size(); ++i)
						{
							GUI::ScopedID id{i};

							std::string label = std::to_string(i);
							ImGui::ColorEdit4(label.c_str(), &col.currentColors[i].R,
							    ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs);

							if (i < col.currentColors.size() - 1)
								ImGui::SameLine();
						}
					};

					if (m_backedUpOgBlueColor)
					{
						if (ImGui::CollapsingHeader("Blue"))
						{
							displayOgColors(m_ogBlueColor);

							GUI::Spacing(2);
						}
					}

					if (m_backedUpOgOrangeColor)
					{
						if (ImGui::CollapsingHeader("Orange"))
						{
							displayOgColors(m_ogOrangeColor);
						}
					}
				}
			}
		}

		ImGui::SameLine();

		{
			GUI::ScopedChild e{"freeplayTeamColors", ImGui::GetContentRegionAvail(), true};

			ImGui::TextColored(GUI::Colors::Yellow, "Freeplay");

			GUI::Spacing(2);

			// enable custom team colors checkbox
			bool useSingleFreeplayColor = useSingleFreeplayColor_cvar.getBoolValue();
			if (ImGui::Checkbox("Solid color", &useSingleFreeplayColor))
				useSingleFreeplayColor_cvar.setValue(useSingleFreeplayColor);

			if (useSingleFreeplayColor)
			{
				GUI::SameLineSpacing_relative(30);

				// solid freeplay color
				LinearColor singleFreeplayColor = singleFreeplayColor_cvar.getColorValue() / 255;
				if (ImGui::ColorEdit3("Color##FieldColor", &singleFreeplayColor.R, ImGuiColorEditFlags_NoInputs))
					singleFreeplayColor_cvar.setValue(singleFreeplayColor * 255);
			}

			GUI::Spacing(2);

			bool useRGBFreeplayColors = useRGBFreeplayColors_cvar.getBoolValue();
			if (ImGui::Checkbox("RGB", &useRGBFreeplayColors))
				useRGBFreeplayColors_cvar.setValue(useRGBFreeplayColors);

			if (useRGBFreeplayColors)
			{
				GUI::Spacing(2);

				// determine slider text
				int         rgbSpeed = rgbSpeed_cvar.getIntValue();
				std::string formatStr;

				if (rgbSpeed == DEFAULT_RGB_SPEED)
					formatStr = "default";
				else if (rgbSpeed < DEFAULT_RGB_SPEED)
					formatStr = std::format("{}x slower", std::abs(rgbSpeed) + 1);
				else
					formatStr = std::format("{}x faster", std::abs(rgbSpeed) + 1);

				if (ImGui::SliderInt("RGB speed", &rgbSpeed, -14, 9, formatStr.c_str()))
					rgbSpeed_cvar.setValue(rgbSpeed);

				GUI::Spacing(2);

				if (ImGui::Button("Reset##rgbSpeed"))
					rgbSpeed_cvar.setValue(DEFAULT_RGB_SPEED);
			}
		}
	}
}


class TeamsComponent Teams{};