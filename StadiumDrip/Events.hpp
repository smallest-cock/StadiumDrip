#pragma once


namespace Events
{
	constexpr const char* LoadingScreenStart =				"Function ProjectX.EngineShare_X.EventPreLoadMap";
	constexpr const char* LoadingScreenEnd =				"Function TAGame.LoadingScreen_TA.HandlePostLoadMap";
	constexpr const char* EnterStartState =					"Function Engine.PlayerController.EnterStartState";
	constexpr const char* RenderColorArray =				"Function TAGame.TeamColorScriptedTexture_TA.RenderColorArray";
	constexpr const char* EnterMainMenu =					"Function TAGame.GFxData_MainMenu_TA.MainMenuAdded";
	constexpr const char* ProgressToMainMenu =				"Function TAGame.GFxData_StartMenu_TA.ProgressToMainMenu";
	constexpr const char* PushMenu =						"Function TAGame.GFxData_MenuStack_TA.PushMenu";
	constexpr const char* PopMenu =							"Function TAGame.GFxData_MenuStack_TA.PopMenu";
	constexpr const char* OpenMidgameMenu =					"Function TAGame.GFxHUD_TA.OpenMidgameMenu";
	constexpr const char* OnEnteredMainMenu =				"Function TAGame.GFxData_MainMenu_TA.OnEnteredMainMenu";
	constexpr const char* MainMenuSwitch =					"Function TAGame.SeqAct_MainMenuSwitch_TA.Activated";
	constexpr const char* EventTeamsCreated =				"Function TAGame.GameEvent_Team_TA.EventTeamsCreated";
	constexpr const char* HandleColorsChanged =				"Function TAGame.GFxData_TeamInfo_TA.HandleColorsChanged";
	constexpr const char* CountdownBegin =					"Function GameEvent_TA.Countdown.BeginState";
	constexpr const char* ReceiveMessage =					"Function TAGame.PlayerController_TA.ReceiveMessage";
	constexpr const char* HUDTick =							"Function TAGame.GFxHUD_TA.Tick";
	constexpr const char* EnterPremiumGarage =				"Function TAGame.PremiumGaragePreviewSet_TA.EnterPremiumGarage";
	constexpr const char* ExitPremiumGarage =				"Function TAGame.PremiumGaragePreviewSet_TA.ExitPremiumGarage";

	constexpr const char* SetCustomTeamName =				"Function TAGame.TeamNameComponent_TA.SetCustomTeamName";
	constexpr const char* UpdateTeamName =					"Function TAGame.TeamNameComponent_TA.UpdateTeamName";
	constexpr const char* EventNameChanged =				"Function TAGame.TeamNameComponent_TA.EventNameChanged";
	constexpr const char* GetTeamName =						"Function TAGame.TeamNameComponent_TA.GetTeamName";

	constexpr const char* HandleNameChanged =				"Function TAGame.GFxData_TeamInfo_TA.HandleNameChanged";

	constexpr const char* MicSetTextureParamValue =			"Function Engine.MaterialInstanceConstant.SetTextureParameterValue";

	// could be useful for freeing mem
	//constexpr const char* MatchDestroyed =					"Function TAGame.GameEvent_TA.Destroyed";
	//constexpr const char* GameDestroyed =					"Function TAGame.GameEvent_TA.EventDestroyed";
}