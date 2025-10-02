#include "pch.h"
#include "Replays.hpp"
#include "Cvars.hpp"
#include "Macros.hpp"

// ##############################################################################################################
// ###############################################    INIT    ###################################################
// ##############################################################################################################

void ReplaysComponent::init(const std::shared_ptr<GameWrapper>& gw)
{
	gameWrapper = gw;

	initCvars();
	initCommands();
	findMapNames();
}

void ReplaysComponent::initCvars() { registerCvar_bool(Cvars::useAltReplayMapSwitch, false).bindTo(m_useAltMapSwitchMethod); }

void ReplaysComponent::initCommands()
{
	registerCommand(Commands::changeReplayMap,
	    [this](std::vector<std::string> args)
	    {
		    if (args.size() < 2)
			    return;

		    changeMap(args[1]);
	    });
}

// ##############################################################################################################
// ###############################################    FUNCTIONS    ##############################################
// ##############################################################################################################

void ReplaysComponent::changeMap(const std::string& mapName)
{
	if (!gameWrapper->IsInReplay())
		return;

	UReplayManager_TA* man = UReplayManager_TA::GetInstance();
	if (!validUObject(man))
	{
		LOGERROR("UReplayManager_TA* from UReplayManager_TA::GetInstance() is invalid");
		return;
	}

	if (*m_useAltMapSwitchMethod)
	{
		// RLSDK method
		auto viewer = Instances.GetInstanceOf<UGFxData_ReplayViewer_TA>();
		if (!validUObject(viewer))
			return;

		man->PlayReplay(viewer->Replay, FString::create(mapName), L"");
		LOG("Changed replay map to: {}", mapName);
	}
	else
	{
		// BMSDK method
		ReplayServerWrapper serverReplay = gameWrapper->GetGameEventAsReplay();
		if (!serverReplay || serverReplay.IsNull())
			return;

		ReplayWrapper replay = serverReplay.GetReplay();
		if (!replay || replay.IsNull())
			return;

		UnrealStringWrapper replayFilename = replay.GetId();
		if (!replayFilename || replayFilename.IsNull())
			return;
		LOG("Replay ID: {}", replayFilename.ToString());

		man->PlayReplayID(replayFilename.ToWideString().c_str(), FString::create(mapName), L"");
		LOG("Changed replay map to: {}", mapName);
	}
}

void ReplaysComponent::findMapNames()
{
	m_mapNames.clear();

	auto* list = Instances.GetInstanceOf<UMapList_TA>();
	if (!validUObject(list))
	{
		LOGERROR("UMapList_TA* is null");
		return;
	}

	auto& maps = list->SortedMaps;
	LOG("Num maps found: {}", maps.size());

	for (UMapData_TA* map : maps)
	{
		if (!validUObject(map))
			continue;

		std::string rawMapName = map->Name.ToString();
		std::string prettyName = map->eventGetLocalizedName().ToString();

		m_mapNames.emplace_back(rawMapName, prettyName);
	}

	LOG("Stored {} map names", m_mapNames.size());

	storeCurrentMapName(); // update current map name (for dropdown default selected/preview name)
}

void ReplaysComponent::storeCurrentMapName()
{
	AWorldInfo* worldInfo = Instances.IAWorldInfo();
	if (!validUObject(worldInfo))
		return;

	m_currentMapName = worldInfo->GetMapName(false).ToString();
	LOG("Updated map name: {}", m_currentMapName);

	for (int i = 0; i < m_mapNames.size(); ++i)
	{
		const MapName& map = m_mapNames[i];

		if (map.internalName == m_currentMapName)
		{
			m_dropdownPreviewIndex = i;
			return;
		}
	}

	m_dropdownPreviewIndex = 0;
}

// ##############################################################################################################
// ###########################################    DISPLAY FUNCTIONS    ##########################################
// ##############################################################################################################

void ReplaysComponent::display()
{
	auto useAltReplayMapSwitch_cvar = getCvar(Cvars::useAltReplayMapSwitch);
	if (!useAltReplayMapSwitch_cvar)
		return;

	GUI::Spacing(2);

	bool useAltReplayMapSwitch = useAltReplayMapSwitch_cvar.getBoolValue();
	if (ImGui::Checkbox("Use alternative method to change replay map", &useAltReplayMapSwitch))
		useAltReplayMapSwitch_cvar.setValue(useAltReplayMapSwitch);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Might help if changing replay map isn't working");

	GUI::Spacing(8);

	display_mapsDropdown();

	GUI::Spacing(4);

	if (ImGui::Button("Update map names"))
	{
		GAME_THREAD_EXECUTE({ findMapNames(); });
	}
}

void ReplaysComponent::display_mapsDropdown()
{
	if (m_mapNames.empty())
	{
		ImGui::TextUnformatted("No map names found :(");
		return;
	}
	if (m_dropdownPreviewIndex >= m_mapNames.size())
	{
		ImGui::TextUnformatted("Invalid dropdown idex :(");
		return;
	}

	char               searchBuffer[128] = "";                                            // Buffer for the search input
	const std::string& previewValue      = m_mapNames[m_dropdownPreviewIndex].prettyName; // show first map name as default preview

	if (ImGui::BeginSearchableCombo("Change map", previewValue.c_str(), searchBuffer, sizeof(searchBuffer), "search..."))
	{
		const std::string searchQueryLower = Format::ToLower(searchBuffer);

		const std::string_view searchView{searchBuffer};
		const bool             hasSearch = !searchView.empty();

		for (int i = 0; i < m_mapNames.size(); ++i)
		{
			GUI::ScopedID id{i};

			const auto& internalMapNameStr = m_mapNames[i].internalName;
			const auto& mapNameStr         = m_mapNames[i].prettyName;
			const auto  mapNameStrLower    = Format::ToLower(mapNameStr);

			if (hasSearch && mapNameStrLower.find(searchQueryLower) == std::string::npos)
				continue;

			if (ImGui::Selectable(mapNameStr.c_str(), m_dropdownPreviewIndex == i))
			{
				GAME_THREAD_EXECUTE({ changeMap(internalMapNameStr); }, internalMapNameStr);
			}
		}

		ImGui::EndCombo();
	}
}

class ReplaysComponent Replays{};