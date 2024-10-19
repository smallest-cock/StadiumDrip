#include "pch.h"
#include "StadiumDrip.h"



void StadiumDrip::Event_LoadingScreenStart(std::string eventName)
{
	Teams.ogColors.clear();
	Mainmenu.aMapHasLoaded = true;
}


void StadiumDrip::Event_MenuChanged(std::string eventName)
{
	RunCommand(Cvars::changeMessageOfTheDay, 0.5f);
}


void StadiumDrip::Event_MainMenuSwitch(std::string eventName)
{
	RunCommand(Cvars::applyAdTexture, 0.5f);
}


void StadiumDrip::Event_LoadingScreenEnd(std::string eventName)
{
	DEBUGLOG("[HOOK] {}", eventName);

	RunCommandInterval(Cvars::applyAdTexture, 3, 0.3f, true);

	DELAY(2.0f,
		Replays.StoreCurrentMapName();
	);
}


void StadiumDrip::Event_EnterMainMenu(std::string eventName)
{
	DEBUGLOG("[HOOK] {}", eventName);

	RunCommand(Cvars::changeMessageOfTheDay, 0.5f);

	ApplyMainMenuCamSettings();
}


void StadiumDrip::Event_RenderColorArray(std::string eventName)
{
	if (Teams.currentlySettingColor) return;

	bool inFreeplay = gameWrapper->IsInFreeplay();

	// use RGB
	auto useRGBFreeplayColors_cvar = GetCvar(Cvars::useRGBFreeplayColors);
	if (!useRGBFreeplayColors_cvar) return;
	bool useRGBFreeplayColors = useRGBFreeplayColors_cvar.getBoolValue();
	
	if (useRGBFreeplayColors && inFreeplay) return;

	// use solid freeplay color
	auto useSingleFreeplayColor_cvar = GetCvar(Cvars::useSingleFreeplayColor);
	if (!useSingleFreeplayColor_cvar) return;
	bool useSingleFreeplayColor = useSingleFreeplayColor_cvar.getBoolValue();

	// use custom team colors
	auto useCustomTeamColors_cvar = GetCvar(Cvars::useCustomTeamColors);
	if (!useCustomTeamColors_cvar) return;
	bool setOGColors = !useCustomTeamColors_cvar.getBoolValue();

	Teams.UpdateTeamFieldColors(nullptr, setOGColors, (useSingleFreeplayColor && inFreeplay), false);
}



// ============================================ hooks with caller ============================================

void StadiumDrip::Event_HandleNameChanged(ActorWrapper caller, void* params, std::string eventName)
{
	UGFxData_TeamInfo_TA* info = reinterpret_cast<UGFxData_TeamInfo_TA*>(caller.memory_address);
	if (!info) return;

	UGFxData_TeamInfo_TA_execHandleNameChanged_Params* Params = reinterpret_cast<UGFxData_TeamInfo_TA_execHandleNameChanged_Params*>(params);
	if (!Params) return;

	Teams.ChangeNameFromTeamInfo(info, Params);
}


void StadiumDrip::Event_TeamNameComponentFuncCalled(ActorWrapper caller, void* params, std::string eventName)
{
	UTeamNameComponent_TA* tnc = reinterpret_cast<UTeamNameComponent_TA*>(caller.memory_address);
	if (!tnc) return;

	Teams.ChangeNameFromTNC(tnc, nullptr, 0.01f, 10);	// total duration is 0.1s... can be increased if necessary
}


void StadiumDrip::Event_ReceiveMessage(ActorWrapper caller, void* params, std::string eventName)
{
	auto useCustomGameMsgs_cvar = GetCvar(Cvars::useCustomGameMsgs);
	if (!useCustomGameMsgs_cvar || !useCustomGameMsgs_cvar.getBoolValue()) return;

	APlayerController_TA_execReceiveMessage_Params* Params = reinterpret_cast<APlayerController_TA_execReceiveMessage_Params*>(params);
	if (!Params) return;

	APlayerController_TA* pc = reinterpret_cast<APlayerController_TA*>(caller.memory_address);
	if (!pc) {
		LOG("APlayerController_TA* casted from caller is null!");
		return;
	}

	Messages.SetGoalScoredMessage(pc, Params->Packet);
}


void StadiumDrip::Event_CountdownBegin(ActorWrapper caller, void* params, std::string eventName)
{
	AGameEvent_TA* gameEvent = reinterpret_cast<AGameEvent_TA*>(caller.memory_address);
	if (!gameEvent) {
		LOG("AGameEvent_TA* casted from caller is null!");
		return;
	}

	auto useCustomCountdownMsgs_cvar = GetCvar(Cvars::useCustomGameMsgs);
	if (!useCustomCountdownMsgs_cvar || !useCustomCountdownMsgs_cvar.getBoolValue()) return;

	auto goMessage_cvar = GetCvar(Cvars::goMessage);
	auto countdownMsg1_cvar = GetCvar(Cvars::countdownMsg1);
	auto countdownMsg2_cvar = GetCvar(Cvars::countdownMsg2);
	auto countdownMsg3_cvar = GetCvar(Cvars::countdownMsg3);
	if (!goMessage_cvar || !countdownMsg1_cvar || !countdownMsg2_cvar || !countdownMsg3_cvar) return;

	std::vector<std::string> countdownMsgs = {
		goMessage_cvar.getStringValue(),
		countdownMsg1_cvar.getStringValue(),
		countdownMsg2_cvar.getStringValue(),
		countdownMsg3_cvar.getStringValue()
	};

	Messages.SetCountdownMessages(gameEvent, countdownMsgs);
}


void StadiumDrip::Event_HandleColorsChanged(ActorWrapper caller, void* params, std::string eventName)
{
	if (gameWrapper->IsInFreeplay()) return;

	UGFxData_TeamInfo_TA* teamInfoData = reinterpret_cast<UGFxData_TeamInfo_TA*>(caller.memory_address);
	if (!teamInfoData) {
		LOG("UGFxData_TeamInfo_TA* from caller is null!");
		return;
	}

	auto useCustomTeamColors_cvar = GetCvar(Cvars::useCustomTeamColors);
	if (!useCustomTeamColors_cvar) return;
	bool setOGColors = !useCustomTeamColors_cvar.getBoolValue();

	Teams.UpdateTeamHUDColors(teamInfoData, setOGColors);
}


void StadiumDrip::Event_EventTeamsCreated(ActorWrapper caller, void* params, std::string eventName)
{
	if (gameWrapper->IsInFreeplay() || Teams.currentlySettingColor) return;

	AGameEvent_Team_TA* gameEvent = reinterpret_cast<AGameEvent_Team_TA*>(caller.memory_address);
	if (!gameEvent) {
		LOG("AGameEvent_TA* from caller is null!");
		return;
	}

	auto useCustomTeamColors_cvar = GetCvar(Cvars::useCustomTeamColors);
	if (!useCustomTeamColors_cvar) return;
	bool setOGColors = !useCustomTeamColors_cvar.getBoolValue();

	Teams.UpdateTeamFieldColors(gameEvent, setOGColors);
	
	int numTimesToRepeat = 3;

	for (int i = 1; i <= numTimesToRepeat; i++)
	{
		gameWrapper->SetTimeout([this, gameEvent, setOGColors](GameWrapper* gw) {

			Teams.UpdateTeamFieldColors(gameEvent, setOGColors);

		}, 0.05f * i);
	}
}


void StadiumDrip::Event_HUDTick(ActorWrapper caller, void* params, std::string eventName)
{
	auto useRGBFreeplayColors_cvar = GetCvar(Cvars::useRGBFreeplayColors);
	if (!useRGBFreeplayColors_cvar) return;
	bool useRGBFreeplayColors = useRGBFreeplayColors_cvar.getBoolValue();

	if (!useRGBFreeplayColors || !gameWrapper->IsInFreeplay()) return;

	TickRGB();

	AGFxHUD_TA* hud = reinterpret_cast<AGFxHUD_TA*>(caller.memory_address);
	if (!hud) return;

	Teams.HandleHUDTick(hud);
}


// overwrite any ad texture changes
void StadiumDrip::Event_MicSetTextureParamValue(ActorWrapper caller, void* params, std::string eventName)
{
	if (Textures.currentlyApplyingTexture) return;

	auto useCustomAds_cvar = GetCvar(Cvars::useCustomAds);
	if (!useCustomAds_cvar || !useCustomAds_cvar.getBoolValue()) return;

	DEBUGLOG("[MIChook] {}", eventName);

	UMaterialInstanceConstant* mic = reinterpret_cast<UMaterialInstanceConstant*>(caller.memory_address);
	if (!mic)
	{
		LOG("UMaterialInstance* from caller is null");
		return;
	}

	Textures.SetTextureOnMIC(mic);
}

