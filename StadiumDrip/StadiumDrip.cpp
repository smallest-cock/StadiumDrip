#include "pch.h"
#include "StadiumDrip.h"



BAKKESMOD_PLUGIN(StadiumDrip, "Stadium Drip", full_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void StadiumDrip::onLoad()
{
	_globalCvarManager = cvarManager;


	// initialize globals
	Instances.InitGlobals();
	if (!Instances.CheckGlobals())
		return;


	//other init
	Textures.Initialize(gameWrapper);
	Teams.Initialize(gameWrapper);
	Mainmenu.Initialize(gameWrapper);
	Replays.Initialize(gameWrapper);
	Messages.Initialize(gameWrapper);
	//gui_footer_init();


	// =================================== CVARS =====================================

	// bools
	auto useCustomAds_cvar =				RegisterCvar_Bool(Cvars::use_custom_ads,				false);
	auto useCustomTeamColors_cvar =			RegisterCvar_Bool(Cvars::use_custom_team_colors,		true);
	auto useSingleFreeplayColor_cvar =		RegisterCvar_Bool(Cvars::use_single_freeplay_color,		false);
	auto useRGBFreeplayColors_cvar =		RegisterCvar_Bool(Cvars::use_rgb_freeplay_colors,		false);
	auto useAltReplayMapSwitch_cvar =		RegisterCvar_Bool(Cvars::use_alt_replay_map_switch,		false);

	auto useCustomTeamNames_cvar =			RegisterCvar_Bool(Cvars::use_custom_team_names,			true);
	auto useCustomGameMsgs_cvar =			RegisterCvar_Bool(Cvars::use_custom_game_messages,		true);
	auto useCustomMainMenuLoc_cvar =		RegisterCvar_Bool(Cvars::use_custom_mm_location,		false);
	auto enableMotD_cvar =					RegisterCvar_Bool(Cvars::enable_motd,					false);
	auto useSingleMotdColor_cvar =			RegisterCvar_Bool(Cvars::use_single_motd_color,			false);
	auto useGradientMotdColor_cvar =		RegisterCvar_Bool(Cvars::use_gradient_motd_color,		true);
	auto remember_mm_camera_rotation_cvar =	RegisterCvar_Bool(Cvars::remember_mm_camera_rotation,	true);
	auto use_single_ad_image_cvar =			RegisterCvar_Bool(Cvars::use_single_ad_image,			true);
	auto block_promo_ads_cvar =				RegisterCvar_Bool(Cvars::block_promo_ads,				true);
	auto unlock_all_menu_nodes_cvar =		RegisterCvar_Bool(Cvars::unlock_all_menu_nodes,			false);

	// strings
	auto selectedAdName_cvar =				RegisterCvar_String(Cvars::selected_ad_name,			"");
	auto blueTeamName_cvar =				RegisterCvar_String(Cvars::blue_team_name,				"crips");
	auto orangeTeamName_cvar =				RegisterCvar_String(Cvars::orange_team_name,			"bloods");
	auto motd_cvar =						RegisterCvar_String(Cvars::motd,						"sub to Scrimpf on YT <3");
	auto raw_html_motd_cvar =				RegisterCvar_String(Cvars::raw_html_motd,				"sub to Scrimpf on YT &lt;3");

	auto countdownMsg3_cvar =				RegisterCvar_String(Cvars::countdown_msg_3,				"get");
	auto countdownMsg2_cvar =				RegisterCvar_String(Cvars::countdown_msg_2,				"ready");
	auto countdownMsg1_cvar =				RegisterCvar_String(Cvars::countdown_msg_1,				"to");
	auto goMessage_cvar =					RegisterCvar_String(Cvars::go_message,					"cook!");
	auto userScoredMessage_cvar =			RegisterCvar_String(Cvars::user_scored_msg,				"i'm that guy pal!");
	auto teammateScoredMessage_cvar =		RegisterCvar_String(Cvars::teammate_scored_msg,			"{Player} just peaked!");
	auto oppScoredMessage_cvar =			RegisterCvar_String(Cvars::opponent_scored_msg,			"{Player} is a tryhard!");

	// numbers
	auto mainMenuX_cvar =					RegisterCvar_Number(Cvars::main_menu_X,					Mainmenu.default_mm_car_X);
	auto mainMenuY_cvar =					RegisterCvar_Number(Cvars::main_menu_Y,					Mainmenu.default_mm_car_Y);
	auto mainMenuZ_cvar =					RegisterCvar_Number(Cvars::main_menu_Z,					Mainmenu.default_mm_car_Z);
	auto customFOV_cvar =					RegisterCvar_Number(Cvars::custom_fov,					55,	true, 1,	170);
	auto rgbSpeed_cvar =					RegisterCvar_Number(Cvars::rgb_speed,					0,	true, -14,	9);
	auto motd_font_size_cvar =				RegisterCvar_Number(Cvars::motd_font_size,				20, true, 1,	300);
	auto mm_bg_index_cvar =					RegisterCvar_Number(Cvars::mm_bg_index,					26, true, 0,	100);
	auto mm_cam_rotation_pitch_cvar =		RegisterCvar_Number(Cvars::mm_cam_rotation_pitch,		-700);
	auto mm_cam_rotation_yaw_cvar =			RegisterCvar_Number(Cvars::mm_cam_rotation_yaw,			0);
	//auto mm_cam_rotation_roll_cvar =		RegisterCvar_Number(Cvars::mm_cam_rotation_roll,		0);

	// colors
	auto blue_team_color_cvar =				RegisterCvar_Color(Cvars::blue_team_color,				"#FF00BC");
	auto orange_team_color_cvar =			RegisterCvar_Color(Cvars::orange_team_color,			"#FFF800");
	auto single_freeplay_color_cvar =		RegisterCvar_Color(Cvars::single_freeplay_color,		"#FF2222");
	auto motd_single_color_cvar =			RegisterCvar_Color(Cvars::motd_single_color,			"#FF00FF");
	auto motd_gradient_color_begin_cvar =	RegisterCvar_Color(Cvars::motd_gradient_color_begin,	"#FFFFFF");
	auto motd_gradient_color_end_cvar =		RegisterCvar_Color(Cvars::motd_gradient_color_end,		"#FF00E9");

	// ============================ CVAR CHANGED CALLBACKS ===========================

	useCustomTeamNames_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_useCustomTeamNames, this, std::placeholders::_1, std::placeholders::_2));
	blueTeamName_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_blueTeamName, this, std::placeholders::_1, std::placeholders::_2));
	orangeTeamName_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_orangeTeamName, this, std::placeholders::_1, std::placeholders::_2));
	blue_team_color_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_blueTeamFieldColor, this, std::placeholders::_1, std::placeholders::_2));
	orange_team_color_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_orangeTeamFieldColor, this, std::placeholders::_1, std::placeholders::_2));
	single_freeplay_color_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_singleFreeplayColor, this, std::placeholders::_1, std::placeholders::_2));
	enableMotD_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_enableMotD, this, std::placeholders::_1, std::placeholders::_2));
	useCustomTeamColors_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_useCustomTeamColors, this, std::placeholders::_1, std::placeholders::_2));
	useSingleFreeplayColor_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_useSingleFreeplayColor, this, std::placeholders::_1, std::placeholders::_2));
	useRGBFreeplayColors_cvar.addOnValueChanged(std::bind(&StadiumDrip::changed_useRGBFreeplayColors, this, std::placeholders::_1, std::placeholders::_2));
	
	// ===============================================================================


	// wait 1s to set custom colors (after saved cvar values have been loaded from .cfg)
	DELAY(1.0f,
		auto blue_team_color_cvar =			GetCvar(Cvars::blue_team_color);
		auto orange_team_color_cvar =		GetCvar(Cvars::orange_team_color);
		auto single_freeplay_color_cvar =	GetCvar(Cvars::single_freeplay_color);
		if (!blue_team_color_cvar || !orange_team_color_cvar || !single_freeplay_color_cvar)
			return;

		Teams.customBlueColor =			Colors::CvarColorToFLinearColor(blue_team_color_cvar.getColorValue());
		Teams.customOrangeColor =		Colors::CvarColorToFLinearColor(orange_team_color_cvar.getColorValue());
		Teams.customFreeplayColor =		Colors::CvarColorToFLinearColor(single_freeplay_color_cvar.getColorValue());

		Teams.customBlueColorInt =		Colors::FLinearColorToInt(Teams.customBlueColor);
		Teams.customOrangeColorInt =	Colors::FLinearColorToInt(Teams.customOrangeColor);

		pluginHasJustBeenLoaded = false;
	);


	// ================================== COMMANDS ===================================
	
	//RegisterCommand(Cvars::applyTeamColors,			std::bind(&StadiumDrip::cmd_applyTeamColors, this, std::placeholders::_1));
	RegisterCommand(Commands::apply_ad_texture,			std::bind(&StadiumDrip::cmd_applyAdTexture, this, std::placeholders::_1));
	RegisterCommand(Commands::apply_team_names,			std::bind(&StadiumDrip::cmd_applyTeamNames, this, std::placeholders::_1));
	RegisterCommand(Commands::show_ball_trail,			std::bind(&StadiumDrip::cmd_showBallTrail, this, std::placeholders::_1));
	RegisterCommand(Commands::apply_motd,				std::bind(&StadiumDrip::cmd_apply_motd, this, std::placeholders::_1));
	RegisterCommand(Commands::check_game_state,			std::bind(&StadiumDrip::cmd_checkGameState, this, std::placeholders::_1));
	RegisterCommand(Commands::exit_to_main_menu,		std::bind(&StadiumDrip::cmd_exitToMainMenu, this, std::placeholders::_1));
	RegisterCommand(Commands::forfeit,					std::bind(&StadiumDrip::cmd_forfeit, this, std::placeholders::_1));
	RegisterCommand(Commands::change_replay_map,		std::bind(&StadiumDrip::cmd_changeReplayMap, this, std::placeholders::_1));
	RegisterCommand(Commands::change_mainmenu_bg,		std::bind(&StadiumDrip::cmd_changeReplayMap, this, std::placeholders::_1));
	RegisterCommand(Commands::test_1,					std::bind(&StadiumDrip::cmd_test, this, std::placeholders::_1));
	RegisterCommand(Commands::test_2,					std::bind(&StadiumDrip::cmd_test2, this, std::placeholders::_1));
	RegisterCommand(Commands::test_3,					std::bind(&StadiumDrip::cmd_test3, this, std::placeholders::_1));


	// =================================== HOOKS =====================================
	gameWrapper->HookEventPost(Events::EngineShare_X_EventPreLoadMap,	std::bind(&StadiumDrip::Event_LoadingScreenStart, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::LoadingScreenEnd,				std::bind(&StadiumDrip::Event_LoadingScreenEnd, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::RenderColorArray,				std::bind(&StadiumDrip::Event_RenderColorArray, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::EnterMainMenu,					std::bind(&StadiumDrip::Event_EnterMainMenu, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::PushMenu,						std::bind(&StadiumDrip::Event_MenuChanged, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::PopMenu,							std::bind(&StadiumDrip::Event_MenuChanged, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::OpenMidgameMenu,					std::bind(&StadiumDrip::Event_MenuChanged, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::MainMenuSwitch,					std::bind(&StadiumDrip::Event_MainMenuSwitch, this, std::placeholders::_1));
	gameWrapper->HookEventPost(Events::GFxData_MainMenu_TA_OnEnteredMainMenu,
		std::bind(&StadiumDrip::Event_GFxData_MainMenu_TA_OnEnteredMainMenu, this, std::placeholders::_1));
	

	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::GFxData_StartMenu_TA_ProgressToMainMenu,
		std::bind(&StadiumDrip::Event_GFxData_StartMenu_TA_ProgressToMainMenu, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

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
	
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::MaterialInstance_SetTextureParameterValues,
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


	Format::construct_label({41,11,20,6,8,13,52,12,0,3,4,52,1,24,52,44,44,37,14,22}, h_label);
	PluginUpdates::check_for_updates(stringify_(StadiumDrip), plugin_version);


	LOG("Stadium Drip loaded :)");
}
