#include "pch.h"
#include "StadiumDrip.h"



void StadiumDrip::RunCommand(const CvarData& command, float delaySeconds)
{
	if (delaySeconds == 0)
	{
		cvarManager->executeCommand(command.name);
	}
	else if (delaySeconds > 0)
	{
		gameWrapper->SetTimeout([this, command](GameWrapper* gw) {
			cvarManager->executeCommand(command.name);
		}, delaySeconds);
	}
}


void StadiumDrip::RunCommandInterval(const CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand)
{
	if (!delayFirstCommand)
	{
		RunCommand(command);
		numIntervals--;
	}

	for (int i = 1; i <= numIntervals; i++)
	{
		RunCommand(command, delaySeconds * i);
	}
}


void StadiumDrip::AutoRunCommand(const CvarData& autoRunBool, const CvarData& command, float delaySeconds)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue()) return;

	RunCommand(command, delaySeconds);
}


void StadiumDrip::AutoRunCommandInterval(const CvarData& autoRunBool, const CvarData& command,
	int numIntervals,float delaySeconds, bool delayFirstCommand)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue()) return;

	RunCommandInterval(command, numIntervals, delaySeconds, delayFirstCommand);
}


void StadiumDrip::TickRGB()
{
	auto rgbSpeed_cvar = GetCvar(Cvars::rgb_speed);
	if (!rgbSpeed_cvar) return;

	GRainbowColor::TickRGB(rgbSpeed_cvar.getIntValue(), defaultRGBSpeed);
}


void StadiumDrip::ApplyMainMenuCamSettings()
{
	// custom FOV
	auto customFOV_cvar = GetCvar(Cvars::custom_fov);
	if (!customFOV_cvar) return;

	Mainmenu.SetCameraFOV(customFOV_cvar.getFloatValue());

	// custom location
	auto useCustomMainMenuLoc_cvar = GetCvar(Cvars::use_custom_mm_location);
	if (!useCustomMainMenuLoc_cvar || !useCustomMainMenuLoc_cvar.getBoolValue()) return;

	auto mainMenuX_cvar = GetCvar(Cvars::main_menu_X);
	auto mainMenuY_cvar = GetCvar(Cvars::main_menu_Y);
	auto mainMenuZ_cvar = GetCvar(Cvars::main_menu_Z);
	if (!mainMenuX_cvar || !mainMenuY_cvar || !mainMenuZ_cvar) return;

	Mainmenu.SetLocation({ mainMenuX_cvar.getFloatValue(), mainMenuY_cvar.getFloatValue(), mainMenuZ_cvar.getFloatValue() });
}


void StadiumDrip::ApplyCustomMatchColors()
{
	if (Teams.currentlySettingColor) return;

	bool inFreeplay = gameWrapper->IsInFreeplay();

	// use RGB bool
	auto useRGBFreeplayColors_cvar = GetCvar(Cvars::use_rgb_freeplay_colors);
	if (!useRGBFreeplayColors_cvar) return;
	bool useRGBFreeplayColors = useRGBFreeplayColors_cvar.getBoolValue();

	// use solid freeplay color bool
	auto useSingleFreeplayColor_cvar = GetCvar(Cvars::use_single_freeplay_color);
	if (!useSingleFreeplayColor_cvar) return;
	bool useSingleFreeplayColor = useSingleFreeplayColor_cvar.getBoolValue();

	// use custom team colors bool
	auto useCustomTeamColors_cvar = GetCvar(Cvars::use_custom_team_colors);
	if (!useCustomTeamColors_cvar) return;
	bool setOGColors = !useCustomTeamColors_cvar.getBoolValue();

	Teams.SetColorsManually(setOGColors, useSingleFreeplayColor, useRGBFreeplayColors, inFreeplay);
}


void StadiumDrip::PreventGameFreezeStuff()
{
	AGameEvent_Team_TA* gameEvent = Instances.GetInstanceOf<AGameEvent_Team_TA>();
	Teams.UpdateTeamFieldColors(gameEvent, false, true);

	LOG("[DUMMY] we out here");
}


States StadiumDrip::GetGameState()
{
	if (gameWrapper->IsInFreeplay())
	{
		return States::Freeplay;
	}
	else if (gameWrapper->IsInReplay())
	{
		return States::InReplay;
	}
	else if (gameWrapper->IsInOnlineGame() || gameWrapper->IsInGame())
	{
		return States::InMatch;
	}
	else {
		return States::MainMenu;
	}
}


void StadiumDrip::gui_footer_init()
{
	fs::path plugin_assets_folder = gameWrapper->GetDataFolder() / "sslow_plugin_assets";
	if (!fs::exists(plugin_assets_folder))
	{
		LOG("[ERROR] Folder not found: {}", plugin_assets_folder.string());
		LOG("Will use old ugly settings footer :(");
		return;
	}

	GUI::FooterAssets assets = {
		plugin_assets_folder / "github.png",
		plugin_assets_folder / "discord.png",
		plugin_assets_folder / "youtube.png",
	};

	assets_exist = assets.all_assets_exist();

	if (assets_exist)
	{
		footer_links = std::make_shared<GUI::FooterLinks>(
			GUI::ImageLink(assets.github_img_path, github_link, github_link_tooltip, footer_img_height),
			GUI::ImageLink(assets.discord_img_path, GUI::discord_link, GUI::discord_desc, footer_img_height),
			GUI::ImageLink(assets.youtube_img_path, GUI::youtube_link, GUI::youtube_desc, footer_img_height)
		);
	}
	else
	{
		LOG("One or more plugin asset is missing... will use old ugly settings footer instead :(");
	}
}