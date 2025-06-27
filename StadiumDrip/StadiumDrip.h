#pragma once
#include "GuiBase.h"
#include <bakkesmod/plugin/bakkesmodplugin.h>
#include <bakkesmod/plugin/pluginwindow.h>
#include <bakkesmod/plugin/PluginSettingsWindow.h>

#include "version.h"

#include "Macros.hpp"
#include "Events.hpp"
#include "Cvars.hpp"
#include <ModUtils/includes.hpp>
#include "Components/Includes.hpp"


constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
constexpr auto short_plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);
constexpr auto pretty_plugin_version = "v" stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);


class StadiumDrip:
	public BakkesMod::Plugin::BakkesModPlugin,
	public SettingsWindowBase,
	public PluginWindowBase
{
private:
	void onLoad() override;
	void onUnload() override;

	// init
	void pluginInit();
	void initCvars();
	void initHooks();
	void initCommands();

private:
	// plugin boilerplate helper stuff
	CVarWrapper RegisterCvar_Bool(const CvarData& cvar, bool startingValue);
	CVarWrapper RegisterCvar_String(const CvarData& cvar, const std::string& startingValue);
	CVarWrapper RegisterCvar_Number(const CvarData& cvar, float startingValue, bool hasMinMax = false, float min = 0, float max = 0);
	CVarWrapper RegisterCvar_Color(const CvarData& cvar, const std::string& startingValue);
	void RegisterCommand(const CvarData& cvar, std::function<void(std::vector<std::string>)> callback);
	CVarWrapper GetCvar(const CvarData& cvar);

	void HookEvent(const char* funcName, std::function<void(std::string)> callback);
	void HookEventPost(const char* funcName, std::function<void(std::string)> callback);
	void HookWithCaller(const char* funcName, std::function<void(ActorWrapper, void*, std::string)> callback);
	void HookWithCallerPost(const char* funcName, std::function<void(ActorWrapper, void*, std::string)> callback);

	void RunCommand(const CvarData& command, float delaySeconds = 0);
	void AutoRunCommand(const CvarData& autoRunBool, const CvarData& command, float delaySeconds = 0);
	void RunCommandInterval(const CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand = false);
	void AutoRunCommandInterval(
		const CvarData& autoRunBool,
		const CvarData& command,
		int numIntervals,
		float delaySeconds,
		bool delayFirstCommand = false);

private:
	// flags
	bool pluginHasJustBeenLoaded = true;

	// values
	std::string h_label;

public:
	// gui
	void RenderSettings() override;
	void RenderWindow() override;
	void Misc_Tab();

	// header/footer stuff
	static constexpr float header_height =					80.0f;
	static constexpr float footer_height =					40.0f;
};
