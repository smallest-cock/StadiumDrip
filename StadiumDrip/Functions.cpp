#include "pch.h"
#include "StadiumDrip.h"



void StadiumDrip::RunCommand(const Cvars::CvarData& command, float delaySeconds)
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


void StadiumDrip::RunCommandInterval(const Cvars::CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand)
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


void StadiumDrip::AutoRunCommand(const Cvars::CvarData& autoRunBool, const Cvars::CvarData& command, float delaySeconds)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue()) return;

	RunCommand(command, delaySeconds);
}


void StadiumDrip::AutoRunCommandInterval(const Cvars::CvarData& autoRunBool, const Cvars::CvarData& command,
	int numIntervals,float delaySeconds, bool delayFirstCommand)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue()) return;

	RunCommandInterval(command, numIntervals, delaySeconds, delayFirstCommand);
}


void StadiumDrip::TickRGB()
{
	auto rgbSpeed_cvar = GetCvar(Cvars::rgbSpeed);
	if (!rgbSpeed_cvar) return;

	GRainbowColor::TickRGB(rgbSpeed_cvar.getIntValue(), defaultRGBSpeed);
}


void StadiumDrip::ApplyMainMenuCamSettings()
{
	// custom FOV
	auto customFOV_cvar = GetCvar(Cvars::customFOV);
	if (!customFOV_cvar) return;

	Mainmenu.SetCameraFOV(customFOV_cvar.getFloatValue());

	// custom location
	auto useCustomMainMenuLoc_cvar = GetCvar(Cvars::useCustomMainMenuLoc);
	if (!useCustomMainMenuLoc_cvar || !useCustomMainMenuLoc_cvar.getBoolValue()) return;

	auto mainMenuX_cvar = GetCvar(Cvars::mainMenuX);
	auto mainMenuY_cvar = GetCvar(Cvars::mainMenuY);
	auto mainMenuZ_cvar = GetCvar(Cvars::mainMenuZ);
	if (!mainMenuX_cvar || !mainMenuY_cvar || !mainMenuZ_cvar) return;

	Mainmenu.SetLocation({ mainMenuX_cvar.getFloatValue(), mainMenuY_cvar.getFloatValue(), mainMenuZ_cvar.getFloatValue() });
}


void StadiumDrip::ApplyCustomMatchColors()
{
	if (Teams.currentlySettingColor) return;

	bool inFreeplay = gameWrapper->IsInFreeplay();

	// use RGB bool
	auto useRGBFreeplayColors_cvar = GetCvar(Cvars::useRGBFreeplayColors);
	if (!useRGBFreeplayColors_cvar) return;
	bool useRGBFreeplayColors = useRGBFreeplayColors_cvar.getBoolValue();

	// use solid freeplay color bool
	auto useSingleFreeplayColor_cvar = GetCvar(Cvars::useSingleFreeplayColor);
	if (!useSingleFreeplayColor_cvar) return;
	bool useSingleFreeplayColor = useSingleFreeplayColor_cvar.getBoolValue();

	// use custom team colors bool
	auto useCustomTeamColors_cvar = GetCvar(Cvars::useCustomTeamColors);
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