#pragma once

namespace Events
{
constexpr auto EngineShare_X_EventPreLoadMap                 = "Function ProjectX.EngineShare_X.EventPreLoadMap"; // loading screen start
constexpr auto LoadingScreen_TA_HandlePostLoadMap            = "Function TAGame.LoadingScreen_TA.HandlePostLoadMap"; // loading screen end

constexpr auto Team_TA_SetCustomTeamName                     = "Function TAGame.Team_TA.SetCustomTeamName";
constexpr auto Team_TA_SetColorList                          = "Function TAGame.Team_TA.SetColorList";
constexpr auto EnterStartState                               = "Function Engine.PlayerController.EnterStartState";
constexpr auto TeamColorScriptedTexture_TA_RenderColorArray  = "Function TAGame.TeamColorScriptedTexture_TA.RenderColorArray";
constexpr auto GFxData_MainMenu_TA_MainMenuAdded             = "Function TAGame.GFxData_MainMenu_TA.MainMenuAdded";
constexpr auto GFxData_MainMenu_TA_OnEnteredMainMenu         = "Function TAGame.GFxData_MainMenu_TA.OnEnteredMainMenu";
constexpr auto GFxData_MainMenu_TA_RotatePreviewCamera       = "Function TAGame.GFxData_MainMenu_TA.RotatePreviewCamera";
constexpr auto GFxData_MainMenu_TA_OnRotatePreviewFinished   = "Function TAGame.GFxData_MainMenu_TA.OnRotatePreviewFinished";
constexpr auto PushMenu                                      = "Function TAGame.GFxData_MenuStack_TA.PushMenu";
constexpr auto PopMenu                                       = "Function TAGame.GFxData_MenuStack_TA.PopMenu";
constexpr auto OpenMidgameMenu                               = "Function TAGame.GFxHUD_TA.OpenMidgameMenu";
constexpr auto GFxHUD_TA_Tick                                = "Function TAGame.GFxHUD_TA.Tick";
constexpr auto SeqAct_MainMenuSwitch_TA_Activated            = "Function TAGame.SeqAct_MainMenuSwitch_TA.Activated";
constexpr auto GameEvent_Team_TA_EventTeamsCreated           = "Function TAGame.GameEvent_Team_TA.EventTeamsCreated";
constexpr auto GFxData_TeamInfo_TA_HandleColorsChanged       = "Function TAGame.GFxData_TeamInfo_TA.HandleColorsChanged";
constexpr auto GFxData_TeamInfo_TA_HandleNameChanged         = "Function TAGame.GFxData_TeamInfo_TA.HandleNameChanged";
constexpr auto GameEvent_TA_Countdown_BeginState             = "Function GameEvent_TA.Countdown.BeginState";
constexpr auto PlayerController_TA_ReceiveMessage            = "Function TAGame.PlayerController_TA.ReceiveMessage";
constexpr auto PremiumGaragePreviewSet_TA_EnterPremiumGarage = "Function TAGame.PremiumGaragePreviewSet_TA.EnterPremiumGarage";
constexpr auto PremiumGaragePreviewSet_TA_ExitPremiumGarage  = "Function TAGame.PremiumGaragePreviewSet_TA.ExitPremiumGarage";
constexpr auto TeamNameComponent_TA_SetCustomTeamName        = "Function TAGame.TeamNameComponent_TA.SetCustomTeamName";
constexpr auto TeamNameComponent_TA_UpdateTeamName           = "Function TAGame.TeamNameComponent_TA.UpdateTeamName";
constexpr auto TeamNameComponent_TA_EventNameChanged         = "Function TAGame.TeamNameComponent_TA.EventNameChanged";
constexpr auto TeamNameComponent_TA_GetTeamName              = "Function TAGame.TeamNameComponent_TA.GetTeamName";
constexpr auto MicSetTextureParamValue                       = "Function Engine.MaterialInstanceConstant.SetTextureParameterValue";
constexpr auto MaterialInstance_SetTextureParameterValues    = "Function Engine.MaterialInstance.SetTextureParameterValues";
constexpr auto AdManager_TA_AddBillboardMeshRequest          = "Function TAGame.AdManager_TA.AddBillboardMeshRequest";
constexpr auto AdManager_TA_CompleteBillboardMeshRequests    = "Function TAGame.AdManager_TA.CompleteBillboardMeshRequests";
constexpr auto GFxData_StartMenu_TA_ProgressToMainMenu       = "Function TAGame.GFxData_StartMenu_TA.ProgressToMainMenu";
constexpr auto __GFxData_Community_TA__UpdateFromCache_0x1 =
    "Function TAGame.GFxData_Community_TA.__GFxData_Community_TA__UpdateFromCache_0x1";
constexpr auto GFxData_Community_TA_SetInitialLoadCompleted       = "Function TAGame.GFxData_Community_TA.SetInitialLoadCompleted";
constexpr auto GFxData_Community_TA_UpdateFromCache               = "Function TAGame.GFxData_Community_TA.UpdateFromCache";
constexpr auto GFxData_Community_TA_HandleBlogChanged             = "Function TAGame.GFxData_Community_TA.HandleBlogChanged";
constexpr auto GFxData_Community_TA_OnShellSet                    = "Function TAGame.GFxData_Community_TA.OnShellSet";
constexpr auto MenuTreeNode_TA_OnGFxNodeSet                       = "Function TAGame.MenuTreeNode_TA.OnGFxNodeSet";
constexpr auto MenuTreeNode_TA_HandleCrumbTrailUpdated            = "Function TAGame.MenuTreeNode_TA.HandleCrumbTrailUpdated";
constexpr auto GFxData_MenuTreeNode_TA_CanShowEngagementEventType = "Function TAGame.GFxData_MenuTreeNode_TA.CanShowEngagementEventType";
}