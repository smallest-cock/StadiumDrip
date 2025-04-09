#pragma once


namespace Events
{
	constexpr const char* EngineShare_X_EventPreLoadMap =					"Function ProjectX.EngineShare_X.EventPreLoadMap";
	constexpr const char* LoadingScreenEnd =								"Function TAGame.LoadingScreen_TA.HandlePostLoadMap";
	constexpr const char* EnterStartState =									"Function Engine.PlayerController.EnterStartState";
	constexpr const char* RenderColorArray =								"Function TAGame.TeamColorScriptedTexture_TA.RenderColorArray";
	constexpr const char* EnterMainMenu =									"Function TAGame.GFxData_MainMenu_TA.MainMenuAdded";
	constexpr const char* PushMenu =										"Function TAGame.GFxData_MenuStack_TA.PushMenu";
	constexpr const char* PopMenu =											"Function TAGame.GFxData_MenuStack_TA.PopMenu";
	constexpr const char* OpenMidgameMenu =									"Function TAGame.GFxHUD_TA.OpenMidgameMenu";
	constexpr const char* GFxData_MainMenu_TA_OnEnteredMainMenu =			"Function TAGame.GFxData_MainMenu_TA.OnEnteredMainMenu";
	constexpr const char* MainMenuSwitch =									"Function TAGame.SeqAct_MainMenuSwitch_TA.Activated";
	constexpr const char* EventTeamsCreated =								"Function TAGame.GameEvent_Team_TA.EventTeamsCreated";
	constexpr const char* HandleColorsChanged =								"Function TAGame.GFxData_TeamInfo_TA.HandleColorsChanged";
	constexpr const char* CountdownBegin =									"Function GameEvent_TA.Countdown.BeginState";
	constexpr const char* ReceiveMessage =									"Function TAGame.PlayerController_TA.ReceiveMessage";
	constexpr const char* HUDTick =											"Function TAGame.GFxHUD_TA.Tick";
	constexpr const char* EnterPremiumGarage =								"Function TAGame.PremiumGaragePreviewSet_TA.EnterPremiumGarage";
	constexpr const char* ExitPremiumGarage =								"Function TAGame.PremiumGaragePreviewSet_TA.ExitPremiumGarage";

	constexpr const char* SetCustomTeamName =								"Function TAGame.TeamNameComponent_TA.SetCustomTeamName";
	constexpr const char* UpdateTeamName =									"Function TAGame.TeamNameComponent_TA.UpdateTeamName";
	constexpr const char* EventNameChanged =								"Function TAGame.TeamNameComponent_TA.EventNameChanged";
	constexpr const char* GetTeamName =										"Function TAGame.TeamNameComponent_TA.GetTeamName";

	constexpr const char* HandleNameChanged =								"Function TAGame.GFxData_TeamInfo_TA.HandleNameChanged";

	constexpr const char* MicSetTextureParamValue =							"Function Engine.MaterialInstanceConstant.SetTextureParameterValue";
	constexpr const char* MaterialInstance_SetTextureParameterValues =		"Function Engine.MaterialInstance.SetTextureParameterValues";

	constexpr const char* GFxData_MainMenu_TA_RotatePreviewCamera =			"Function TAGame.GFxData_MainMenu_TA.RotatePreviewCamera";
	constexpr const char* GFxData_MainMenu_TA_OnRotatePreviewFinished =		"Function TAGame.GFxData_MainMenu_TA.OnRotatePreviewFinished";


	constexpr const char* AdManager_TA_AddBillboardMeshRequest =			"Function TAGame.AdManager_TA.AddBillboardMeshRequest";
	constexpr const char* AdManager_TA_CompleteBillboardMeshRequests =		"Function TAGame.AdManager_TA.CompleteBillboardMeshRequests";	// when all the requests are done/sent


	constexpr const char* GFxData_StartMenu_TA_ProgressToMainMenu =			"Function TAGame.GFxData_StartMenu_TA.ProgressToMainMenu";

	constexpr const char* __GFxData_Community_TA__UpdateFromCache_0x1 =		"Function TAGame.GFxData_Community_TA.__GFxData_Community_TA__UpdateFromCache_0x1";
	constexpr const char* GFxData_Community_TA_SetInitialLoadCompleted =	"Function TAGame.GFxData_Community_TA.SetInitialLoadCompleted";
	constexpr const char* GFxData_Community_TA_UpdateFromCache =			"Function TAGame.GFxData_Community_TA.UpdateFromCache";
	constexpr const char* GFxData_Community_TA_HandleBlogChanged =			"Function TAGame.GFxData_Community_TA.HandleBlogChanged";
	constexpr const char* GFxData_Community_TA_OnShellSet =					"Function TAGame.GFxData_Community_TA.OnShellSet";


	constexpr const char* MenuTreeNode_TA_OnGFxNodeSet =						"Function TAGame.MenuTreeNode_TA.OnGFxNodeSet";
	constexpr const char* MenuTreeNode_TA_HandleCrumbTrailUpdated =				"Function TAGame.MenuTreeNode_TA.HandleCrumbTrailUpdated";
	constexpr const char* GFxData_MenuTreeNode_TA_CanShowEngagementEventType =	"Function TAGame.GFxData_MenuTreeNode_TA.CanShowEngagementEventType";
}