#include "pch.h"
#include "StadiumDrip.h"



void StadiumDrip::changed_useCustomTeamNames(std::string cvarName, CVarWrapper updatedCvar)
{
	GAME_THREAD_EXECUTE(
		Teams.ChangeNamesFromGameEvent();
	);
}


void StadiumDrip::changed_blueTeamFieldColor(std::string cvarName, CVarWrapper updatedCvar)
{
	Teams.customBlueColor = Colors::CvarColorToFLinearColor(updatedCvar.getColorValue());
	Teams.customBlueColorInt = Colors::FLinearColorToInt(Teams.customBlueColor);
}


void StadiumDrip::changed_orangeTeamFieldColor(std::string cvarName, CVarWrapper updatedCvar)
{
	Teams.customOrangeColor = Colors::CvarColorToFLinearColor(updatedCvar.getColorValue());
	Teams.customOrangeColorInt = Colors::FLinearColorToInt(Teams.customOrangeColor);
}


void StadiumDrip::changed_singleFreeplayColor(std::string cvarName, CVarWrapper updatedCvar)
{
	Teams.customFreeplayColor = Colors::CvarColorToFLinearColor(updatedCvar.getColorValue());

	if (Teams.currentlySettingColor || !gameWrapper->IsInFreeplay()) return;

	auto useSingleFreeplayColor_cvar = GetCvar(Cvars::useSingleFreeplayColor);
	if (!useSingleFreeplayColor_cvar || !useSingleFreeplayColor_cvar.getBoolValue()) return;

	GAME_THREAD_EXECUTE(
		Teams.UpdateTeamFieldColors(nullptr, false, true);
	);
}


// these dont need to check if useCustomTeamNames is enabled, bc they are only visible/editable when it's enabled (except in the console)
void StadiumDrip::changed_blueTeamName(std::string cvarName, CVarWrapper updatedCvar)
{
	std::string updatedVal = updatedCvar.getStringValue();

	auto orangeTeamName_cvar = GetCvar(Cvars::orangeTeamName);
	std::string orangeTeamName = orangeTeamName_cvar.getStringValue();

	GAME_THREAD_EXECUTE_CAPTURE(
		Teams.ChangeTeamNames(updatedVal, orangeTeamName);
	, updatedVal, orangeTeamName);

	DEBUGLOG("{} changed...", cvarName);
}


void StadiumDrip::changed_orangeTeamName(std::string cvarName, CVarWrapper updatedCvar)
{
	std::string updatedVal = updatedCvar.getStringValue();

	auto blueTeamName_cvar = GetCvar(Cvars::blueTeamName);
	std::string blueTeamName = blueTeamName_cvar.getStringValue();

	GAME_THREAD_EXECUTE_CAPTURE(
		Teams.ChangeTeamNames(blueTeamName, updatedVal);
	, updatedVal, blueTeamName);

	DEBUGLOG("{} changed...", cvarName);
}


void StadiumDrip::changed_enableMotD(std::string cvarName, CVarWrapper updatedCvar)
{
	//if (pluginHasJustBeenLoaded) return;

	bool updatedVal = updatedCvar.getBoolValue();

	if (updatedVal)
	{
		GAME_THREAD_EXECUTE(
			RunCommand(Cvars::changeMessageOfTheDay);
		);
	} 
	else
	{
		GAME_THREAD_EXECUTE(
			Messages.ClearMOTD();
		);
	}

	DEBUGLOG("{} changed...", cvarName);
}


// ================================ enabling/disabling map colors ================================

void StadiumDrip::changed_useCustomTeamColors(std::string cvarName, CVarWrapper updatedCvar)
{
	if (Teams.currentlySettingColor) return;

	bool updatedVal = updatedCvar.getBoolValue();

	GAME_THREAD_EXECUTE_CAPTURE(
		if (updatedVal)
		{
			// update field colors
			auto useSingleFreeplayColor_cvar = GetCvar(Cvars::useSingleFreeplayColor);
			auto useRGBFreeplayColors_cvar = GetCvar(Cvars::useRGBFreeplayColors);
			if (!useSingleFreeplayColor_cvar || !useRGBFreeplayColors_cvar) return;

			bool inFreeplay = gameWrapper->IsInFreeplay();

			if (inFreeplay)
			{
				Teams.UpdateTeamFieldColors(nullptr, !updatedVal, useSingleFreeplayColor_cvar.getBoolValue(), useRGBFreeplayColors_cvar.getBoolValue());
			}
			else
			{
				AGameEvent_Team_TA* gameEvent = Instances.GetInstanceOf<AGameEvent_Team_TA>();
				Teams.UpdateTeamFieldColors(gameEvent, !updatedVal);
			}

			// update HUD colors
			if (inFreeplay) return;

			auto teamInfos = Instances.GetAllInstancesOf<UGFxData_TeamInfo_TA>();
			for (UGFxData_TeamInfo_TA* teamInfo : teamInfos)
			{
				if (!teamInfo) continue;

				Teams.UpdateTeamHUDColors(teamInfo, !updatedVal);
			}
		}
		else
		{
			Teams.ApplyOgColors();
		}
	, updatedVal);
}


void StadiumDrip::changed_useSingleFreeplayColor(std::string cvarName, CVarWrapper updatedCvar)
{
	if (Teams.currentlySettingColor) return;

	bool updatedVal = updatedCvar.getBoolValue();

	GAME_THREAD_EXECUTE_CAPTURE(

		if (!gameWrapper->IsInFreeplay()) return;

		auto useCustomTeamColors_cvar =	GetCvar(Cvars::useCustomTeamColors);
		auto useRGBFreeplayColors_cvar = GetCvar(Cvars::useRGBFreeplayColors);
		if (!useCustomTeamColors_cvar || !useRGBFreeplayColors_cvar) return;

		Teams.UpdateTeamFieldColors(nullptr, !useCustomTeamColors_cvar.getBoolValue(), updatedVal, useRGBFreeplayColors_cvar.getBoolValue());
	, updatedVal);
}


void StadiumDrip::changed_useRGBFreeplayColors(std::string cvarName, CVarWrapper updatedCvar)
{
	if (Teams.currentlySettingColor) return;

	bool updatedVal = updatedCvar.getBoolValue();

	GAME_THREAD_EXECUTE_CAPTURE(

		if (!gameWrapper->IsInFreeplay()) return;

		auto useCustomTeamColors_cvar = GetCvar(Cvars::useCustomTeamColors);
		auto useSingleFreeplayColor_cvar = GetCvar(Cvars::useSingleFreeplayColor);
		if (!useCustomTeamColors_cvar || !useSingleFreeplayColor_cvar) return;

		Teams.UpdateTeamFieldColors(nullptr, !useCustomTeamColors_cvar.getBoolValue(), useSingleFreeplayColor_cvar.getBoolValue(), updatedVal);
	, updatedVal);
}