#pragma once
#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include "Macros.hpp"
#include "Events.hpp"
#include "Cvars.hpp"
#include "Components/Includes.hpp"


constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
constexpr auto pretty_plugin_version = "v" stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);


enum States : uint8_t
{
	MainMenu =	0,
	Freeplay =	1,
	InReplay =	2,
	InMatch =	3
};


class StadiumDrip: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	,public PluginWindowBase // Uncomment if you want to render your own plugin window
{
	//Boilerplate
	void onLoad() override;
	//void onUnload() override;


	// cvar helper stuff
	CVarWrapper RegisterCvar_Bool(const Cvars::CvarData& cvar, bool startingValue);
	CVarWrapper RegisterCvar_String(const Cvars::CvarData& cvar, const std::string& startingValue);
	CVarWrapper RegisterCvar_Number(const Cvars::CvarData& cvar, float startingValue, bool hasMinMax = false, float min = 0, float max = 0);
	CVarWrapper RegisterCvar_Color(const Cvars::CvarData& cvar, const std::string& startingValue);
	void RegisterCommand(const Cvars::CvarData& cvar, std::function<void(std::vector<std::string>)> callback);
	CVarWrapper GetCvar(const Cvars::CvarData& cvar);

	void RunCommand(const Cvars::CvarData& command, float delaySeconds = 0);
	void AutoRunCommand(const Cvars::CvarData& autoRunBool, const Cvars::CvarData& command, float delaySeconds = 0);
	void RunCommandInterval(const Cvars::CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand = false);
	void AutoRunCommandInterval(
		const Cvars::CvarData& autoRunBool,
		const Cvars::CvarData& command,
		int numIntervals,
		float delaySeconds,
		bool delayFirstCommand = false);


	// default values
	static constexpr const int defaultRGBSpeed = 0;


	// misc
	void TickRGB();
	bool pluginHasJustBeenLoaded = true;
	void PreventGameFreezeStuff();
	States GetGameState();
	void ApplyMainMenuCamSettings();
	void ApplyCustomMatchColors();


	// commands
	void cmd_applyAdTexture(std::vector<std::string> args);
	void cmd_applyTeamColors(std::vector<std::string> args);
	void cmd_applyTeamNames(std::vector<std::string> args);
	void cmd_showBallTrail(std::vector<std::string> args);
	void cmd_changeMessageOfTheDay(std::vector<std::string> args);
	void cmd_checkGameState(std::vector<std::string> args);
	void cmd_exitToMainMenu(std::vector<std::string> args);
	void cmd_forfeit(std::vector<std::string> args);
	void cmd_changeReplayMap(std::vector<std::string> args);
	void cmd_test(std::vector<std::string> args);
	void cmd_test2(std::vector<std::string> args);
	void cmd_test3(std::vector<std::string> args);


	// cvar changed callbacks
	void changed_useCustomTeamNames(std::string cvarName, CVarWrapper updatedCvar);
	void changed_useSingleFreeplayColor(std::string cvarName, CVarWrapper updatedCvar);
	void changed_useCustomTeamColors(std::string cvarName, CVarWrapper updatedCvar);
	void changed_blueTeamName(std::string cvarName, CVarWrapper updatedCvar);
	void changed_orangeTeamName(std::string cvarName, CVarWrapper updatedCvar);
	void changed_blueTeamFieldColor(std::string cvarName, CVarWrapper updatedCvar);
	void changed_orangeTeamFieldColor(std::string cvarName, CVarWrapper updatedCvar);
	void changed_singleFreeplayColor(std::string cvarName, CVarWrapper updatedCvar);
	void changed_enableMotD(std::string cvarName, CVarWrapper updatedCvar);
	void changed_useRGBFreeplayColors(std::string cvarName, CVarWrapper updatedCvar);


	// hook callbacks
	void Event_LoadingScreenStart(std::string eventName);
	void Event_LoadingScreenEnd(std::string eventName);
	void Event_EnterMainMenu(std::string eventName);
	void Event_MenuChanged(std::string eventName);
	void Event_RenderColorArray(std::string eventName);
	void Event_MainMenuSwitch(std::string eventName);
	void Event_GameDestroyed(ActorWrapper caller, void* params, std::string eventName);
	void Event_CountdownBegin(ActorWrapper caller, void* params, std::string eventName);
	void Event_ReceiveMessage(ActorWrapper caller, void* params, std::string eventName);
	void Event_EventTeamsCreated(ActorWrapper caller, void* params, std::string eventName);
	void Event_HandleColorsChanged(ActorWrapper caller, void* params, std::string eventName);
	void Event_HUDTick(ActorWrapper caller, void* params, std::string eventName);
	
	void Event_HandleNameChanged(ActorWrapper caller, void* params, std::string eventName);
	void Event_TeamNameComponentFuncCalled(ActorWrapper caller, void* params, std::string eventName);

	// hijack ad texture changes
	void Event_MicSetTextureParamValue(ActorWrapper caller, void* params, std::string eventName);	// this is the one
	//void Event_SetTextureParamValue(ActorWrapper caller, void* params, std::string eventName);
	//void Event_SetTextureParamValues(ActorWrapper caller, void* params, std::string eventName);


public:
	// GUI
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	void RenderWindow() override; // Uncomment if you want to render your own plugin window
	void AdTexturesDropdown();
	void MainMenuBackgroundsDropdown();
	void ReplayMapsDropdown();
	void Teams_Tab();
	void Ads_Tab();
	void Messages_Tab();
	void MainMenu_Tab();
	void Replays_Tab();

	// general width & spacing
	float itemWidth = 200;
	float sameLineSpacing = itemWidth / 8;    // ~25 px
};
