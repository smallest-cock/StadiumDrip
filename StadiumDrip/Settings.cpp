#include "pch.h"
#include "StadiumDrip.h"
#include "GuiTools.hpp"



void StadiumDrip::RenderSettings()
{
	// ---------------- calculate ImGui::BeginChild sizes ------------------

	ImVec2 availableSpace = ImGui::GetContentRegionAvail();
	availableSpace.y -= 4;		// act as if availableSpace height is 4px smaller, bc for some reason availableSpace height is cap (prevents scroll bars)
	constexpr float headerHeight = 80.0f;
	constexpr float footerHeight = 35.0f;
	const float contentHeight = availableSpace.y - footerHeight;

	ImVec2 contentSize = ImVec2(0, contentHeight);
	ImVec2 footerSize = ImVec2(0, footerHeight);
	ImVec2 headerSize = ImVec2(0, headerHeight);

	// ----------------------------------------------------------------------
	
	if (ImGui::BeginChild("Content#sd", contentSize))
	{
		GUI::SettingsHeader("Header##sd", pretty_plugin_version, headerSize, false);

		GUI::Spacing(4);

		// open bindings window button
		std::string openMenuCommand = "togglemenu " + GetMenuName();
		if (ImGui::Button("Open Menu"))
		{
			GAME_THREAD_EXECUTE_CAPTURE(
				cvarManager->executeCommand(openMenuCommand);
			, openMenuCommand);
		}

		GUI::Spacing(8);

		ImGui::Text("or bind this command:  ");
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		ImGui::InputText("", &openMenuCommand, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
	}
	ImGui::EndChild();

	GUI::SettingsFooter("Footer##sd", footerSize, availableSpace.x, false);
}


void StadiumDrip::RenderWindow()
{
	ImGui::BeginTabBar("##Tabs");

	if (ImGui::BeginTabItem("Teams"))
	{
		Teams_Tab();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Messages"))
	{
		Messages_Tab();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Main Menu"))
	{
		MainMenu_Tab();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Ads"))
	{
		Ads_Tab();
		ImGui::EndTabItem();
	}
	
	if (ImGui::BeginTabItem("Replays"))
	{
		Replays_Tab();
		ImGui::EndTabItem();
	}
}


// teams tab
void StadiumDrip::Teams_Tab()
{
	auto blueTeamFieldColor_cvar =      GetCvar(Cvars::blueTeamFieldColor);
	auto orangeTeamFieldColor_cvar =    GetCvar(Cvars::orangeTeamFieldColor);
	auto useCustomTeamNames_cvar =      GetCvar(Cvars::useCustomTeamNames);
	auto blueTeamName_cvar =            GetCvar(Cvars::blueTeamName);
	auto orangeTeamName_cvar =          GetCvar(Cvars::orangeTeamName);
	auto useCustomTeamColors_cvar =		GetCvar(Cvars::useCustomTeamColors);
	auto useSingleFreeplayColor_cvar =  GetCvar(Cvars::useSingleFreeplayColor);
	auto singleFreeplayColor_cvar =     GetCvar(Cvars::singleFreeplayColor);
	auto useRGBFreeplayColors_cvar =    GetCvar(Cvars::useRGBFreeplayColors);
	auto rgbSpeed_cvar =				GetCvar(Cvars::rgbSpeed);

	// if any is null, they prolly all are...
	if (!blueTeamFieldColor_cvar || !orangeTeamFieldColor_cvar || !useCustomTeamNames_cvar || !blueTeamName_cvar || !orangeTeamName_cvar ||
		!useCustomTeamColors_cvar || !useSingleFreeplayColor_cvar || !singleFreeplayColor_cvar || !useRGBFreeplayColors_cvar || !rgbSpeed_cvar) return;

	// --------------------- ImGui::BeginChild sizes ------------------------

	ImVec2 parentSize = ImGui::GetContentRegionAvail();

	ImVec2 teamNamesSize = ImVec2(0, parentSize.y * 0.3f - 2);
	ImVec2 teamColorsSize = ImVec2(0, parentSize.y * 0.7f - 2);

	// ----------------------------------------------------------------------

	if (ImGui::BeginChild("##teamNames", teamNamesSize, true))
	{
		GUI::Spacing(2);

		// enable custom team names checkbox
		bool useCustomTeamNames = useCustomTeamNames_cvar.getBoolValue();
		if (ImGui::Checkbox("custom team names", &useCustomTeamNames))
		{
			useCustomTeamNames_cvar.setValue(useCustomTeamNames);
		}

		if (useCustomTeamNames)
		{
			GUI::Spacing(2);

			// custom blue team name
			std::string blueTeamName = blueTeamName_cvar.getStringValue();
			ImGui::SetNextItemWidth(itemWidth);
			if (ImGui::InputText("blue", &blueTeamName))
			{
				blueTeamName_cvar.setValue(blueTeamName);
			}

			GUI::SameLineSpacing(sameLineSpacing);

			// custom orange team name
			std::string orangeTeamName = orangeTeamName_cvar.getStringValue();
			ImGui::SetNextItemWidth(itemWidth);
			if (ImGui::InputText("orange", &orangeTeamName))
			{
				orangeTeamName_cvar.setValue(orangeTeamName);
			}
		}
	}
	ImGui::EndChild();

	if (ImGui::BeginChild("##teamColors", teamColorsSize, true))
	{
		// --------------------- ImGui::BeginChild sizes ------------------------
		
		ImVec2 teamColParentSize = ImGui::GetContentRegionAvail();
		
		ImVec2 matchTeamColorsSize = ImVec2(teamColParentSize.x * 0.5f - 4, 0);
		ImVec2 freeplayColorsSize = ImVec2(teamColParentSize.x * 0.5f - 4, 0);

		// ----------------------------------------------------------------------

		if (ImGui::BeginChild("##matchTeamColors", matchTeamColorsSize, true))
		{
			ImGui::TextColored(ImVec4{ 1, 1, 0, 1 }, "Matches");

			GUI::Spacing(2);

			// enable custom team colors checkbox
			bool useCustomTeamColors = useCustomTeamColors_cvar.getBoolValue();
			if (ImGui::Checkbox("custom team colors", &useCustomTeamColors))
			{
				useCustomTeamColors_cvar.setValue(useCustomTeamColors);
			}

			if (useCustomTeamColors)
			{
				GUI::Spacing(2);

				// blue team color
				LinearColor blueTeamFieldColor = blueTeamFieldColor_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("blue##FieldColor", &blueTeamFieldColor.R, ImGuiColorEditFlags_NoInputs))
				{
					blueTeamFieldColor_cvar.setValue(blueTeamFieldColor * 255);
				}

				GUI::Spacing(2);

				// orange team color
				LinearColor orangeTeamFieldColor = orangeTeamFieldColor_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("orange##FieldColor", &orangeTeamFieldColor.R, ImGuiColorEditFlags_NoInputs))
				{
					orangeTeamFieldColor_cvar.setValue(orangeTeamFieldColor * 255);
				}

				GUI::Spacing(2);

				if (ImGui::Button("Apply##custom_team_colors"))
				{
					GAME_THREAD_EXECUTE(
						ApplyCustomMatchColors();
					);
				}
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGui::BeginChild("##freeplayTeamColors", freeplayColorsSize, true))
		{
			ImGui::TextColored(ImVec4{ 1, 1, 0, 1 }, "Freeplay");

			GUI::Spacing(2);

			// enable custom team colors checkbox
			bool useSingleFreeplayColor = useSingleFreeplayColor_cvar.getBoolValue();
			if (ImGui::Checkbox("solid color", &useSingleFreeplayColor))
			{
				useSingleFreeplayColor_cvar.setValue(useSingleFreeplayColor);
			}

			if (useSingleFreeplayColor)
			{
				GUI::SameLineSpacing(30);

				// solid freeplay color
				LinearColor singleFreeplayColor = singleFreeplayColor_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("color##FieldColor", &singleFreeplayColor.R, ImGuiColorEditFlags_NoInputs))
				{
					singleFreeplayColor_cvar.setValue(singleFreeplayColor * 255);
				}
			}

			GUI::Spacing(2);

			bool useRGBFreeplayColors = useRGBFreeplayColors_cvar.getBoolValue();
			if (ImGui::Checkbox("RGB", &useRGBFreeplayColors))
			{
				useRGBFreeplayColors_cvar.setValue(useRGBFreeplayColors);
			}

			if (useRGBFreeplayColors)
			{
				GUI::Spacing(2);

				// determine slider text
				int rgbSpeed = rgbSpeed_cvar.getIntValue();
				std::string formatStr;
				if (rgbSpeed < defaultRGBSpeed)
				{
					formatStr = std::to_string(std::abs(rgbSpeed) + 1) + "x slower";
				}
				else if (rgbSpeed == defaultRGBSpeed)
				{
					formatStr = "default";
				}
				else
				{
					formatStr = std::to_string(std::abs(rgbSpeed) + 1) + "x faster";
				}

				if (ImGui::SliderInt("RGB speed", &rgbSpeed, -14, 9, formatStr.c_str()))
				{
					rgbSpeed_cvar.setValue(rgbSpeed);
				}

				GUI::Spacing(2);

				if (ImGui::Button("reset##rgbSpeed"))
				{
					rgbSpeed_cvar.setValue(defaultRGBSpeed);
				}
			}
		}
		ImGui::EndChild();

	}
	ImGui::EndChild();
}


// ads tab
void StadiumDrip::Ads_Tab()
{
	auto useCustomAds_cvar = GetCvar(Cvars::useCustomAds);
	if (!useCustomAds_cvar) return;

	GUI::Spacing(4);

	// enable custom ads checkbox
	bool useCustomAds = useCustomAds_cvar.getBoolValue();
	if (ImGui::Checkbox("custom ads", &useCustomAds))
		useCustomAds_cvar.setValue(useCustomAds);

	if (useCustomAds)
	{
		GUI::Spacing(8);

		AdTexturesDropdown();

		ImGui::SameLine();

		// refresh/parse ad textures button
		if (ImGui::Button("Refresh"))
		{
			GAME_THREAD_EXECUTE(
				Textures.ParseAdTextures();
			);
		}

		GUI::Spacing(2);

		// open AdTextures button
		if (ImGui::Button("Open AdTextures folder"))
			Files::OpenFolder(Textures.adTexturesFolder);

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Images must be PNG format");
	}
}


// messages tab
void StadiumDrip::Messages_Tab()
{
	auto enableMotD_cvar =					GetCvar(Cvars::enableMotD);
	auto motd_cvar =						GetCvar(Cvars::motd);
	auto useSingleMotdColor_cvar =			GetCvar(Cvars::useSingleMotdColor);
	auto motdSingleColor_cvar =				GetCvar(Cvars::motdSingleColor);
	//auto useGradientMotdColor_cvar =		GetCvar(Cvars::useGradientMotdColor);
	//auto motdGradientColorBegin_cvar =		GetCvar(Cvars::motdGradientColorBegin);
	//auto motdGradientColorEnd_cvar =		GetCvar(Cvars::motdGradientColorEnd);
	if (!enableMotD_cvar || !motd_cvar || !useSingleMotdColor_cvar) return;

	auto useCustomGameMsgs_cvar =       GetCvar(Cvars::useCustomGameMsgs);
	auto countdownMsg3_cvar =           GetCvar(Cvars::countdownMsg3);
	auto countdownMsg2_cvar =           GetCvar(Cvars::countdownMsg2);
	auto countdownMsg1_cvar =           GetCvar(Cvars::countdownMsg1);
	auto goMessage_cvar =               GetCvar(Cvars::goMessage);
	auto userScoredMessage_cvar =       GetCvar(Cvars::userScoredMessage);
	auto teammateScoredMessage_cvar =   GetCvar(Cvars::teammateScoredMessage);
	auto oppScoredMessage_cvar =        GetCvar(Cvars::oppScoredMessage);
	if (!useCustomGameMsgs_cvar || !countdownMsg3_cvar || !countdownMsg2_cvar || !countdownMsg1_cvar || !goMessage_cvar
		|| !teammateScoredMessage_cvar || !oppScoredMessage_cvar || !userScoredMessage_cvar) return;

	// --------------------- ImGui::BeginChild sizes ------------------------

	ImVec2 parentSize = ImGui::GetContentRegionAvail();

	ImVec2 footerMsgSectionSize = ImVec2(0, parentSize.y * 0.5f - 2);
	ImVec2 gameMsgSectionSize = ImVec2(0, parentSize.y * 0.5f - 2);

	// ----------------------------------------------------------------------


	if (ImGui::BeginChild("footerMessage##messages", footerMsgSectionSize, true))
	{
		GUI::Spacing(2);

		// enable custom team names checkbox
		bool enableMotD = enableMotD_cvar.getBoolValue();
		if (ImGui::Checkbox("rolling footer message", &enableMotD))
			enableMotD_cvar.setValue(enableMotD);

		if (enableMotD)
		{
			GUI::Spacing(2);

			bool useSingleMotdColor = useSingleMotdColor_cvar.getBoolValue();
			//bool useGradientMotdColor = useGradientMotdColor_cvar.getBoolValue();
			
			int radioState = 0;

			if (useSingleMotdColor)
			{
				radioState = 1;
			}
			//else if (useGradientMotdColor)
			//{
			//	radioState = 2;
			//}

			if (ImGui::RadioButton("custom text/html", &radioState, 0))
			{
				useSingleMotdColor_cvar.setValue(false);
				//useGradientMotdColor_cvar.setValue(false);
			}
			if (ImGui::RadioButton("colored text", &radioState, 1))
			{
				useSingleMotdColor_cvar.setValue(true);
				//useGradientMotdColor_cvar.setValue(false);
			}
			//if (ImGui::RadioButton("gradient color text", &radioState, 2))
			//{
			//	useSingleMotdColor_cvar.setValue(false);
			//	useGradientMotdColor_cvar.setValue(true);
			//}

			GUI::Spacing(2);

			// custom blue team name
			std::string motd = Format::UnescapeQuotesHTML(motd_cvar.getStringValue());
			if (ImGui::InputText("message", &motd))
				motd_cvar.setValue(Format::EscapeQuotesHTML(motd));

			GUI::SameLineSpacing(10.0f);

			if (ImGui::Button("Apply"))
			{
				GAME_THREAD_EXECUTE(
					RunCommand(Cvars::changeMessageOfTheDay);
				);
			}

			if (useSingleMotdColor)
			{
				// color picker
				LinearColor motdSingleColor = motdSingleColor_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("color##motdSingleColor", &motdSingleColor.R, ImGuiColorEditFlags_NoInputs))
				{
					motdSingleColor_cvar.setValue(motdSingleColor * 255);
				}
			}
			//else if (useGradientMotdColor)
			//{
			//	// 2 color pickers
			//	LinearColor motdGradientColorBegin = motdGradientColorBegin_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
			//	if (ImGui::ColorEdit3("begin##motdGradientColor", &motdGradientColorBegin.R, ImGuiColorEditFlags_NoInputs))
			//	{
			//		motdGradientColorBegin_cvar.setValue(motdGradientColorBegin * 255);
			//	}
			//	LinearColor motdGradientColorEnd = motdGradientColorEnd_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
			//	if (ImGui::ColorEdit3("end##motdGradientColor", &motdGradientColorEnd.R, ImGuiColorEditFlags_NoInputs))
			//	{
			//		motdGradientColorEnd_cvar.setValue(motdGradientColorEnd * 255);
			//	}
			//}
		}
	}
	ImGui::EndChild();

	if (ImGui::BeginChild("gameMessages##messages", gameMsgSectionSize, true))
	{
		// enable custom team names checkbox
		bool useCustomGameMsgs = useCustomGameMsgs_cvar.getBoolValue();
		if (ImGui::Checkbox("custom game messages", &useCustomGameMsgs))
			useCustomGameMsgs_cvar.setValue(useCustomGameMsgs);

		if (useCustomGameMsgs)
		{
			GUI::Spacing(2);

			if (ImGui::CollapsingHeader("Countdown messages##collapsing"))
			{
				GUI::Spacing(2);

				std::string countdownMsg3 = countdownMsg3_cvar.getStringValue();
				if (ImGui::InputText("3", &countdownMsg3))
					countdownMsg3_cvar.setValue(countdownMsg3);

				GUI::Spacing();

				std::string countdownMsg2 = countdownMsg2_cvar.getStringValue();
				if (ImGui::InputText("2", &countdownMsg2))
					countdownMsg2_cvar.setValue(countdownMsg2);

				GUI::Spacing();

				std::string countdownMsg1 = countdownMsg1_cvar.getStringValue();
				if (ImGui::InputText("1", &countdownMsg1))
					countdownMsg1_cvar.setValue(countdownMsg1);

				GUI::Spacing();

				std::string goMessage = goMessage_cvar.getStringValue();
				if (ImGui::InputText("Go!", &goMessage))
					goMessage_cvar.setValue(goMessage);
			}

			GUI::Spacing(2);

			if (ImGui::CollapsingHeader("Goal scored messages##collapsing"))
			{
				GUI::Spacing(2);

				std::string userScoredMessage = userScoredMessage_cvar.getStringValue();
				if (ImGui::InputTextWithHint("Me", "i'm a god", &userScoredMessage))
					userScoredMessage_cvar.setValue(userScoredMessage);

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("TIP: Use {Player} in your message to include the name of the player who scored");

				GUI::Spacing();

				std::string teammateScoredMessage = teammateScoredMessage_cvar.getStringValue();
				if (ImGui::InputTextWithHint("Teammate", "{Player} just peaked!", &teammateScoredMessage))
					teammateScoredMessage_cvar.setValue(teammateScoredMessage);

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("TIP: Use {Player} in your message to include the name of the player who scored");

				GUI::Spacing();

				std::string oppScoredMessage = oppScoredMessage_cvar.getStringValue();
				if (ImGui::InputTextWithHint("Opponent", "{Player} is a tryhard!", &oppScoredMessage))
					oppScoredMessage_cvar.setValue(oppScoredMessage);

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("TIP: Use {Player} in your message to include the name of the player who scored");
			}
		}

	}
	ImGui::EndChild();
	

	//std::string numInvincibleMsgs = "invincibleMessages size: " + std::to_string(Messages.invincibleMessages.size());
	//ImGui::Text(numInvincibleMsgs.c_str());
}


// main menu tab
void StadiumDrip::MainMenu_Tab()
{
	auto useCustomMainMenuLoc_cvar =	GetCvar(Cvars::useCustomMainMenuLoc);
	auto mainMenuX_cvar =				GetCvar(Cvars::mainMenuX);
	auto mainMenuY_cvar =				GetCvar(Cvars::mainMenuY);
	auto mainMenuZ_cvar =				GetCvar(Cvars::mainMenuZ);
	auto customFOV_cvar =				GetCvar(Cvars::customFOV);
	if (!mainMenuX_cvar || !mainMenuY_cvar || !mainMenuZ_cvar || !customFOV_cvar || !useCustomMainMenuLoc_cvar) return;


	// --------------------- ImGui::BeginChild sizes ------------------------

	ImVec2 parentSize = ImGui::GetContentRegionAvail();

	ImVec2 mainMenuLocationSectionSize =	ImVec2(0, parentSize.y * 0.6f - 2);
	ImVec2 customFOVSectionSize =			ImVec2(0, parentSize.y * 0.4f - 2);

	// ----------------------------------------------------------------------

	if (ImGui::BeginChild("##mainMenuLocation", mainMenuLocationSectionSize, true))
	{
		GUI::Spacing(2);

		// enable custom team names checkbox
		bool useCustomMainMenuLoc = useCustomMainMenuLoc_cvar.getBoolValue();
		if (ImGui::Checkbox("custom location", &useCustomMainMenuLoc))
			useCustomMainMenuLoc_cvar.setValue(useCustomMainMenuLoc);

		if (useCustomMainMenuLoc)
		{
			GUI::Spacing(2);
			
			// X
			float mainMenuX = mainMenuX_cvar.getIntValue();
			if (ImGui::SliderFloat("X##sliderX", &mainMenuX, -50000, 50000, "%.0f"))
			{
				mainMenuX_cvar.setValue(mainMenuX);
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			if (ImGui::InputFloat("##inputX", &mainMenuX, 10.0f, 100.0f, "%.0f"))
			{
				mainMenuX_cvar.setValue(mainMenuX);
			}
			ImGui::PopItemWidth();

			// Y
			float mainMenuY = mainMenuY_cvar.getIntValue();
			if (ImGui::SliderFloat("Y##sliderY", &mainMenuY, -50000, 50000, "%.0f"))
			{
				mainMenuY_cvar.setValue(mainMenuY);
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			if (ImGui::InputFloat("##inputY", &mainMenuY, 10.0f, 100.0f, "%.0f"))
			{
				mainMenuY_cvar.setValue(mainMenuY);
			}
			ImGui::PopItemWidth();

			// Z
			float mainMenuZ = mainMenuZ_cvar.getFloatValue();
			if (ImGui::SliderFloat("Z##sliderZ", &mainMenuZ, -50000, 50000, "%.0f"))
			{
				mainMenuZ_cvar.setValue(mainMenuZ);
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			if (ImGui::InputFloat("##inputZ", &mainMenuZ, 10.0f, 100.0f, "%.0f"))
			{
				mainMenuZ_cvar.setValue(mainMenuZ);
			}
			ImGui::PopItemWidth();

			GUI::Spacing(2);

			if (ImGui::Button("Apply##mainMenuLocation"))
			{
				FVector newLocation = { mainMenuX, mainMenuY, mainMenuZ };

				GAME_THREAD_EXECUTE_CAPTURE(
					Mainmenu.SetLocation(newLocation);
				, newLocation);
			}

			GUI::SameLineSpacing(25);

			if (ImGui::Button("Reset##mainMenuLocation"))
			{
				// reset cvar values
				mainMenuX_cvar.setValue(Mainmenu.default_mm_car_X);
				mainMenuY_cvar.setValue(Mainmenu.default_mm_car_Y);
				mainMenuZ_cvar.setValue(Mainmenu.default_mm_car_Z);

				GAME_THREAD_EXECUTE(
					Mainmenu.ResetLocation(true);
				);
			}
		}
	}
	ImGui::EndChild();


	if (ImGui::BeginChild("##customFOV", customFOVSectionSize, true))
	{
		GUI::Spacing(2);

		int customFOV = customFOV_cvar.getIntValue();
		if (ImGui::SliderInt("FOV", &customFOV, 1, 170))
		{
			customFOV_cvar.setValue(customFOV);

			// TODO: move to a cvar callback function
			GAME_THREAD_EXECUTE_CAPTURE(
				Mainmenu.SetCameraFOV(customFOV);
			, customFOV);
		}

		GUI::SameLineSpacing(25);

		if (ImGui::Button("Reset##mainMenuFOV"))
		{
			// reset cvar values
			customFOV_cvar.setValue(Mainmenu.default_mm_FOV);

			GAME_THREAD_EXECUTE(
				Mainmenu.ResetCameraFOV(true);
			);
		}

		GUI::Spacing(2);

		MainMenuBackgroundsDropdown();
	}
	ImGui::EndChild();
}


// replays tab
void StadiumDrip::Replays_Tab()
{
	auto useAltReplayMapSwitch_cvar = GetCvar(Cvars::useAltReplayMapSwitch);
	if (!useAltReplayMapSwitch_cvar) return;

	GUI::Spacing(2);

	bool useAltReplayMapSwitch = useAltReplayMapSwitch_cvar.getBoolValue();
	if (ImGui::Checkbox("use alternative method to change replay map", &useAltReplayMapSwitch))
	{
		useAltReplayMapSwitch_cvar.setValue(useAltReplayMapSwitch);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Might help if changing replay map isn't working");
	}

	GUI::Spacing(8);

	ReplayMapsDropdown();

	GUI::Spacing(4);

	if (ImGui::Button("Update map names"))
	{
		GAME_THREAD_EXECUTE(
			Replays.FindMapNames();
		);
	}
}


void StadiumDrip::AdTexturesDropdown()
{
	// cvars
	auto selectedAdNameCvar = GetCvar(Cvars::selectedAdName);
	if (!selectedAdNameCvar) return;


	if (Textures.adImgNames.empty())
	{
		ImGui::Text("No PNG images found in AdTextures folder ....");
		return;
	}

	char searchBuffer[128] = "";  // Buffer for the search input
	const char* previewValue = Textures.adImgNames[Textures.selectedAdTextureIndex].c_str();

	if (ImGui::BeginSearchableCombo("ad image##adTextureDropdown", previewValue, searchBuffer, sizeof(searchBuffer), "search..."))
	{
		// convert search text to lower
		std::string searchQuery = Format::ToLower(searchBuffer);

		for (int i = 0; i < Textures.adImgNames.size(); i++)
		{
			ImGui::PushID(i);

			std::string adTexName = Textures.adImgNames[i];

			// convert title text to lower
			std::string adTexNameLower = Format::ToLower(adTexName);

			// filter results if necessary
			if (searchBuffer != "")
			{
				if (adTexNameLower.find(searchQuery) != std::string::npos)
				{
					if (ImGui::Selectable(adTexName.c_str(), Textures.selectedAdTextureIndex == i))
					{
						Textures.selectedAdTextureIndex = i;

						// update cvar string value
						selectedAdNameCvar.setValue(adTexName);

						GAME_THREAD_EXECUTE(
							RunCommand(Cvars::applyAdTexture);
						);
					}
				}
			}
			else
			{
				if (ImGui::Selectable(adTexName.c_str(), Textures.selectedAdTextureIndex == i))
				{
					Textures.selectedAdTextureIndex = i;

					// update cvar string value
					selectedAdNameCvar.setValue(adTexName);

					GAME_THREAD_EXECUTE(
						RunCommand(Cvars::applyAdTexture);
					);
				}
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}
}


void StadiumDrip::MainMenuBackgroundsDropdown()
{
	if (Mainmenu.backgroundNames.empty())
	{
		ImGui::Text("No main menu backgrounds found....");
		return;
	}

	char searchBuffer[128] = "";  // Buffer for the search input
	const char* previewValue = Mainmenu.backgroundNames[Mainmenu.selectedBackgroundIndex].c_str();

	if (ImGui::BeginSearchableCombo("background##backgroundsDropdown", previewValue, searchBuffer, sizeof(searchBuffer), "search..."))
	{
		// convert search text to lower
		std::string searchQuery = Format::ToLower(searchBuffer);

		for (int i = 0; i < Mainmenu.backgroundNames.size(); i++)
		{
			ImGui::PushID(i);

			std::string bgName = Mainmenu.backgroundNames[i];

			// convert title text to lower
			std::string bgNameLower = Format::ToLower(bgName);

			// filter results if necessary
			if (searchBuffer != "")
			{
				if (bgNameLower.find(searchQuery) != std::string::npos)
				{
					if (ImGui::Selectable(bgName.c_str(), Mainmenu.selectedBackgroundIndex == i))
					{
						Mainmenu.selectedBackgroundIndex = i;

						// do something when a bg is selected ... 
						if (GetGameState() != States::MainMenu) continue;

						GAME_THREAD_EXECUTE(
							Mainmenu.SetBackground(Mainmenu.working_mmbg_ids[Mainmenu.backgroundNames[Mainmenu.selectedBackgroundIndex]], true);

							DELAY(1.0f,
								RunCommandInterval(Cvars::applyAdTexture, 3, 1.0f, true);
							);
						);
					}
				}
			}
			else
			{
				if (ImGui::Selectable(bgName.c_str(), Mainmenu.selectedBackgroundIndex == i))
				{
					Mainmenu.selectedBackgroundIndex = i;

					// do something when a bg is selected ...
					if (GetGameState() != States::MainMenu) continue;

					GAME_THREAD_EXECUTE(
						Mainmenu.SetBackground(Mainmenu.working_mmbg_ids[Mainmenu.backgroundNames[Mainmenu.selectedBackgroundIndex]], true);
						
						DELAY(1.0f,
							RunCommandInterval(Cvars::applyAdTexture, 3, 1.0f, true);
						);
					);
				}
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}
}


void StadiumDrip::ReplayMapsDropdown()
{
	if (Replays.mapNames.empty())
	{
		ImGui::Text("No map names found :(");
		return;
	}

	char searchBuffer[128] = "";	// Buffer for the search input
	const char* previewValue = Replays.mapNames[Replays.dropdownPreviewIndex].prettyName.c_str();		// just show first map name in vector as default preview value

	if (ImGui::BeginSearchableCombo("change map##replayMapsDropdown", previewValue, searchBuffer, sizeof(searchBuffer), "search..."))
	{
		// convert search text to lower
		const std::string searchQuery = Format::ToLower(searchBuffer);

		for (int i = 0; i < Replays.mapNames.size(); i++)
		{
			const std::string& internalMapNameStr = Replays.mapNames[i].internalName;
			const std::string& mapNameStr = Replays.mapNames[i].prettyName;
			const std::string mapNameStrLower = Format::ToLower(mapNameStr);

			ImGui::PushID(i);

			// only render option if there's text in search box & it matches the key name
			if (searchBuffer != "")
			{
				if (mapNameStrLower.find(searchQuery) != std::string::npos)
				{
					if (ImGui::Selectable(mapNameStr.c_str(), Replays.dropdownPreviewIndex == i))
					{
						GAME_THREAD_EXECUTE_CAPTURE(
							Replays.ChangeMap(internalMapNameStr);
						, internalMapNameStr);
					}
				}
			}
			// if there's no text in search box, render all possible key options
			else
			{
				if (ImGui::Selectable(mapNameStr.c_str(), Replays.dropdownPreviewIndex == i))
				{
					GAME_THREAD_EXECUTE_CAPTURE(
						Replays.ChangeMap(internalMapNameStr);
					, internalMapNameStr);
				}
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}
}