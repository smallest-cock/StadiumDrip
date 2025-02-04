#pragma once


#define CVAR(name, desc) CvarData("sd_" name, desc)		// automatically apply the prefix to cvar names

namespace Cvars
{
	struct CvarData
	{
		const char* name;
		const char* description;

		constexpr CvarData(const char* name, const char* description)
			: name(name), description(description) {}
	};

	// ============================== cvars =============================

	// bools
	constexpr CvarData useCustomAds =				CVAR("use_custom_ads",					"use custom ad textures");
	constexpr CvarData useCustomTeamColors =		CVAR("use_custom_team_colors",			"use custom team colors");
	constexpr CvarData useSingleFreeplayColor =		CVAR("use_single_freeplay_color",		"use a single freeplay color instead of custom team colors");
	constexpr CvarData useRGBFreeplayColors =		CVAR("use_rgb_freeplay_colors",			"use RGB stadium colors in freeplay");
	constexpr CvarData useAltReplayMapSwitch =		CVAR("use_alt_replay_map_switch",		"use alternate method to change replay map" );

	constexpr CvarData useCustomTeamNames =			CVAR("use_custom_team_names",			"use custom team names");
	constexpr CvarData useCustomGameMsgs =			CVAR("use_custom_game_messages",		"use custom game messages");
	constexpr CvarData useCustomMainMenuLoc =		CVAR("use_custom_main_menu_location",	"use custom main menu location");
	constexpr CvarData enableMotD =					CVAR("enable_message_of_the_day",		"enable message of the day (rolling footer message)");
	constexpr CvarData useSingleMotdColor =			CVAR("use_single_motd_color",			"use a single color for message of the day");
	constexpr CvarData useGradientMotdColor =		CVAR("use_gradient_motd_color",			"use a gradient of colors for message of the day");

	// strings
	constexpr CvarData selectedAdName =				CVAR("selected_ad_name",				"selected ad texture");
	constexpr CvarData blueTeamName =				CVAR("blue_team_name",					"custom blue team name");
	constexpr CvarData orangeTeamName =				CVAR("orange_team_name",				"custom orange team name");
	constexpr CvarData motd =						CVAR("message_of_the_day",				"custom message of the day (rolling text in menu footer)");

	constexpr CvarData teammateScoredMessage =		CVAR("teammate_scored_message",			"teammate goal scored message");
	constexpr CvarData oppScoredMessage =			CVAR("opponent_scored_message",			"opponent goal scored message");
	constexpr CvarData userScoredMessage =			CVAR("user_scored_message",				"user goal scored message");
	constexpr CvarData goMessage =					CVAR("go_message",						"custom 'Go!' message");
	constexpr CvarData countdownMsg3 =				CVAR("countdown_message_3",				"custom '3' message");
	constexpr CvarData countdownMsg2 =				CVAR("countdown_message_2",				"custom '2' message");
	constexpr CvarData countdownMsg1 =				CVAR("countdown_message_1",				"custom '1' message");

	// numbers
	constexpr CvarData mainMenuX =					CVAR("main_menu_X",						"X coordinate of main menu map location");
	constexpr CvarData mainMenuY =					CVAR("main_menu_Y",						"Y coordinate of main menu map location");
	constexpr CvarData mainMenuZ =					CVAR("main_menu_Z",						"Z coordinate (height) of main menu map location");
	constexpr CvarData customFOV =					CVAR("custom_fov",						"custom camera FOV");
	constexpr CvarData rgbSpeed =					CVAR("rgb_speed",						"RGB color cycle speed");
	constexpr CvarData motd_font_size =				CVAR("motd_font_size",					"font size for MOTD text");

	// colors
	constexpr CvarData blueTeamFieldColor =			CVAR("blue_team_field_color",			"blue team field color");
	constexpr CvarData orangeTeamFieldColor =		CVAR("orange_team_field_color",			"orange team field color");
	constexpr CvarData singleFreeplayColor =		CVAR("single_freeplay_color",			"separate (single) freeplay color");
	constexpr CvarData motdSingleColor =			CVAR("motd_single_color",				"solid color for rolling footer message text");
	constexpr CvarData motdGradientColorBegin =		CVAR("motd_gradient_color_begin",		"beginning gradient color for motd");
	constexpr CvarData motdGradientColorEnd =		CVAR("motd_gradient_color_end",			"end gradient color for motd");


	// ============================ commands ============================

	constexpr CvarData applyAdTexture =				CVAR("apply_ad_texture",				"apply ad texture");
	constexpr CvarData applyTeamColors =			CVAR("apply_team_colors",				"apply team colors");
	constexpr CvarData applyTeamNames =				CVAR("apply_team_names",				"apply custom team names");
	constexpr CvarData showBallTrail =				CVAR("show_ball_trail",					"show the ball trail");
	constexpr CvarData changeMessageOfTheDay =		CVAR("apply_motd",						"change the lil rolling text message on the bottom of screen");
	constexpr CvarData checkGameState =				CVAR("check_game_state",				"log the current game state");
	constexpr CvarData exitToMainMenu =				CVAR("exit_to_main_menu",				"exit to main menu (as if the button were pressed)");
	constexpr CvarData forfeit =					CVAR("forfeit",							"vote to forfeit");
	constexpr CvarData changeReplayMap =			CVAR("change_replay_map",				"change the current replay map");

	constexpr CvarData test =						CVAR("test",							"test");
	constexpr CvarData test2 =						CVAR("test2",							"test 2");
	constexpr CvarData test3 =						CVAR("test3",							"test 3");
	constexpr CvarData testerColors =				CVAR("tester_colors",					"test changing colors");
}

#undef CVAR
