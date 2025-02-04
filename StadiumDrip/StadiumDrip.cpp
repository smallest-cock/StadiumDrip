#include "pch.h"
#include "StadiumDrip.h"



BAKKESMOD_PLUGIN(StadiumDrip, "Stadium Drip", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void StadiumDrip::onLoad()
{
	_globalCvarManager = cvarManager;


	// initialize globals
	Instances.InitGlobals();
	if (!Instances.CheckGlobals()) return;


	//other init
	Textures.Initialize(gameWrapper);
	Teams.Initialize(gameWrapper);
	Mainmenu.Initialize(gameWrapper);
	Replays.Initialize(gameWrapper);
	gui_footer_init();


	// =================================== CVARS =====================================

	// bools
	auto useCustomAds_cvar =				RegisterCvar_Bool(Cvars::useCustomAds,				false);
	auto useCustomTeamColors_cvar =			RegisterCvar_Bool(Cvars::useCustomTeamColors,		true);
	auto useSingleFreeplayColor_cvar =		RegisterCvar_Bool(Cvars::useSingleFreeplayColor,	false);
	auto useRGBFreeplayColors_cvar =		RegisterCvar_Bool(Cvars::useRGBFreeplayColors,		false);
	auto useAltReplayMapSwitch_cvar =		RegisterCvar_Bool(Cvars::useAltReplayMapSwitch,		false);

	auto useCustomTeamNames_cvar =			RegisterCvar_Bool(Cvars::useCustomTeamNames,		true);
	auto useCustomGameMsgs_cvar =			RegisterCvar_Bool(Cvars::useCustomGameMsgs,			true);
	auto useCustomMainMenuLoc_cvar =		RegisterCvar_Bool(Cvars::useCustomMainMenuLoc,		false);
	auto enableMotD_cvar =					RegisterCvar_Bool(Cvars::enableMotD,				false);
	auto useSingleMotdColor_cvar =			RegisterCvar_Bool(Cvars::useSingleMotdColor,		true);
	//auto useGradientMotdColor_cvar =		RegisterCvar_Bool(Cvars::useGradientMotdColor,		false);

	// strings
	auto selectedAdName_cvar =				RegisterCvar_String(Cvars::selectedAdName,			"");
	auto blueTeamName_cvar =				RegisterCvar_String(Cvars::blueTeamName,			"crips");
	auto orangeTeamName_cvar =				RegisterCvar_String(Cvars::orangeTeamName,			"bloods");
	auto motd_cvar =						RegisterCvar_String(Cvars::motd,					"sub to Scrimpf on YT &lt;3");

	auto countdownMsg3_cvar =				RegisterCvar_String(Cvars::countdownMsg3,			"get");
	auto countdownMsg2_cvar =				RegisterCvar_String(Cvars::countdownMsg2,			"ready");
	auto countdownMsg1_cvar =				RegisterCvar_String(Cvars::countdownMsg1,			"to");
	auto goMessage_cvar =					RegisterCvar_String(Cvars::goMessage,				"cook!");
	auto userScoredMessage_cvar =			RegisterCvar_String(Cvars::userScoredMessage,		"i'm that guy pal!");
	auto teammateScoredMessage_cvar =		RegisterCvar_String(Cvars::teammateScoredMessage,	"{Player} just peaked!");
	auto oppScoredMessage_cvar =			RegisterCvar_String(Cvars::oppScoredMessage,		"{Player} is a tryhard!");

	// numbers
	auto mainMenuX_cvar =					RegisterCvar_Number(Cvars::mainMenuX,				Mainmenu.default_mm_car_X);
	auto mainMenuY_cvar =					RegisterCvar_Number(Cvars::mainMenuY,				Mainmenu.default_mm_car_Y);
	auto mainMenuZ_cvar =					RegisterCvar_Number(Cvars::mainMenuZ,				Mainmenu.default_mm_car_Z);
	auto customFOV_cvar =					RegisterCvar_Number(Cvars::customFOV,				55, true, 1, 170);
	auto rgbSpeed_cvar =					RegisterCvar_Number(Cvars::rgbSpeed,				0, true, -14, 9);
	auto motd_font_size_cvar =				RegisterCvar_Number(Cvars::motd_font_size,			20, true, 1, 300);

	// colors
	auto blueTeamFieldColor_cvar =			RegisterCvar_Color(Cvars::blueTeamFieldColor,		"#FF00BC");
	auto orangeTeamFieldColor_cvar =		RegisterCvar_Color(Cvars::orangeTeamFieldColor,		"#FFF800");
	auto singleFreeplayColor_cvar =			RegisterCvar_Color(Cvars::singleFreeplayColor,		"#FF2222");
	auto motd_color_cvar =					RegisterCvar_Color(Cvars::motd_color,				"#FF00FF");
	//auto motdGradientColorBegin_cvar =		RegisterCvar_Color(Cvars::motdGradientColorBegin,	"#9D00FF");
	//auto motdGradientColorEnd_cvar =		RegisterCvar_Color(Cvars::motdGradientColorEnd,		"#00BB33");


	// ============================ CVAR CHANGED CALLBACKS ===========================

	useCustomTeamNames_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_useCustomTeamNames, this, std::placeholders::_1, std::placeholders::_2));
	blueTeamName_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_blueTeamName, this, std::placeholders::_1, std::placeholders::_2));
	orangeTeamName_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_orangeTeamName, this, std::placeholders::_1, std::placeholders::_2));
	blueTeamFieldColor_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_blueTeamFieldColor, this, std::placeholders::_1, std::placeholders::_2));
	orangeTeamFieldColor_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_orangeTeamFieldColor, this, std::placeholders::_1, std::placeholders::_2));
	singleFreeplayColor_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_singleFreeplayColor, this, std::placeholders::_1, std::placeholders::_2));
	enableMotD_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_enableMotD, this, std::placeholders::_1, std::placeholders::_2));
	useCustomTeamColors_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_useCustomTeamColors, this, std::placeholders::_1, std::placeholders::_2));
	useSingleFreeplayColor_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_useSingleFreeplayColor, this, std::placeholders::_1, std::placeholders::_2));
	useRGBFreeplayColors_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_useRGBFreeplayColors, this, std::placeholders::_1, std::placeholders::_2));
	
	// ===============================================================================


	// wait 1s to set custom colors (after saved cvar values have been loaded from .cfg)
	DELAY(1.0f,
		auto blueTeamFieldColor_cvar = cvarManager->getCvar(Cvars::blueTeamFieldColor.name);
		auto orangeTeamFieldColor_cvar = cvarManager->getCvar(Cvars::orangeTeamFieldColor.name);
		auto singleFreeplayColor_cvar = cvarManager->getCvar(Cvars::singleFreeplayColor.name);
		if (!blueTeamFieldColor_cvar || !orangeTeamFieldColor_cvar || !singleFreeplayColor_cvar) return;

		Teams.customBlueColor = Colors::CvarColorToFLinearColor(blueTeamFieldColor_cvar.getColorValue());
		Teams.customOrangeColor = Colors::CvarColorToFLinearColor(orangeTeamFieldColor_cvar.getColorValue());
		Teams.customFreeplayColor = Colors::CvarColorToFLinearColor(singleFreeplayColor_cvar.getColorValue());

		Teams.customBlueColorInt = Colors::FLinearColorToInt(Teams.customBlueColor);
		Teams.customOrangeColorInt = Colors::FLinearColorToInt(Teams.customOrangeColor);

		pluginHasJustBeenLoaded = false;
	);


	// ================================== COMMANDS ===================================
	
	//RegisterCommand(Cvars::applyTeamColors,			std::bind(&StadiumDrip::cmd_applyTeamColors, this, std::placeholders::_1));
	RegisterCommand(Cvars::applyAdTexture,			std::bind(&StadiumDrip::cmd_applyAdTexture, this, std::placeholders::_1));
	RegisterCommand(Cvars::applyTeamNames,			std::bind(&StadiumDrip::cmd_applyTeamNames, this, std::placeholders::_1));
	RegisterCommand(Cvars::showBallTrail,			std::bind(&StadiumDrip::cmd_showBallTrail, this, std::placeholders::_1));
	RegisterCommand(Cvars::changeMessageOfTheDay,	std::bind(&StadiumDrip::cmd_changeMessageOfTheDay, this, std::placeholders::_1));
	RegisterCommand(Cvars::checkGameState,			std::bind(&StadiumDrip::cmd_checkGameState, this, std::placeholders::_1));
	RegisterCommand(Cvars::exitToMainMenu,			std::bind(&StadiumDrip::cmd_exitToMainMenu, this, std::placeholders::_1));
	RegisterCommand(Cvars::forfeit,					std::bind(&StadiumDrip::cmd_forfeit, this, std::placeholders::_1));
	RegisterCommand(Cvars::changeReplayMap,			std::bind(&StadiumDrip::cmd_changeReplayMap, this, std::placeholders::_1));
	RegisterCommand(Cvars::test,					std::bind(&StadiumDrip::cmd_test, this, std::placeholders::_1));
	RegisterCommand(Cvars::test2,					std::bind(&StadiumDrip::cmd_test2, this, std::placeholders::_1));
	RegisterCommand(Cvars::test3,					std::bind(&StadiumDrip::cmd_test3, this, std::placeholders::_1));


	// =================================== HOOKS =====================================

	gameWrapper->HookEventPost(Events::LoadingScreenStart,		std::bind(&StadiumDrip::Event_LoadingScreenStart, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::LoadingScreenEnd,		std::bind(&StadiumDrip::Event_LoadingScreenEnd, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::RenderColorArray,		std::bind(&StadiumDrip::Event_RenderColorArray, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::EnterMainMenu,			std::bind(&StadiumDrip::Event_EnterMainMenu, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::ProgressToMainMenu,		std::bind(&StadiumDrip::Event_EnterMainMenu, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::PushMenu,				std::bind(&StadiumDrip::Event_MenuChanged, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::PopMenu,					std::bind(&StadiumDrip::Event_MenuChanged, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::OpenMidgameMenu,			std::bind(&StadiumDrip::Event_MenuChanged, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::OnEnteredMainMenu,		std::bind(&StadiumDrip::Event_MenuChanged, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::MainMenuSwitch,			std::bind(&StadiumDrip::Event_MainMenuSwitch, this, std::placeholders::_1));
	
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::EventTeamsCreated,
		std::bind(&StadiumDrip::Event_EventTeamsCreated, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::EnterPremiumGarage,
		std::bind(&StadiumDrip::Event_EnterPremiumGarage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::ExitPremiumGarage,
		std::bind(&StadiumDrip::Event_ExitPremiumGarage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	// for HUD colors
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::HandleColorsChanged,
		std::bind(&StadiumDrip::Event_HandleColorsChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	// for game messages ...
	gameWrapper->HookEventWithCaller<ActorWrapper>(Events::CountdownBegin,
		std::bind(&StadiumDrip::Event_CountdownBegin, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEventWithCaller<ActorWrapper>(Events::ReceiveMessage,
		std::bind(&StadiumDrip::Event_ReceiveMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	// for RGB
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::HUDTick,
		std::bind(&StadiumDrip::Event_HUDTick, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	// overwrite any ad texture changes
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::MicSetTextureParamValue,
		std::bind(&StadiumDrip::Event_MicSetTextureParamValue, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::HandleNameChanged,
		std::bind(&StadiumDrip::Event_HandleNameChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	// TNC functions
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::GetTeamName,
		std::bind(&StadiumDrip::Event_TeamNameComponentFuncCalled, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::EventNameChanged,
		std::bind(&StadiumDrip::Event_TeamNameComponentFuncCalled, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::SetCustomTeamName,
		std::bind(&StadiumDrip::Event_TeamNameComponentFuncCalled, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::UpdateTeamName,
		std::bind(&StadiumDrip::Event_TeamNameComponentFuncCalled, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	// ===============================================================================


	// seems to help reduce game freeze on load
	DUMMY_THREAD(
		PreventGameFreezeStuff();
	);



	LOG("Stadium Drip loaded :)");
}

