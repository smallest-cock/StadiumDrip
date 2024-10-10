#pragma once
#include <string>


namespace Cvars
{
	const std::string prefix = "sd_";

	struct CvarData
	{
		const std::string name;
		const std::string description;

		// constructor to automatically add prefix to name
		CvarData(const std::string& name, const std::string& description)
			: name(prefix + name), description(description) {}
	};


	// ============================== cvars =============================

	// bools
	const CvarData useCustomAds =				{ "use_custom_ads",					"use custom ad textures" };
	const CvarData useCustomTeamColors =		{ "use_custom_team_colors",			"use custom team colors" };
	const CvarData useSingleFreeplayColor =		{ "use_single_freeplay_color",		"use a single freeplay color instead of custom team colors" };
	const CvarData useRGBFreeplayColors =		{ "use_rgb_freeplay_colors",		"use RGB stadium colors in freeplay" };

	const CvarData useCustomTeamNames =			{ "use_custom_team_names",			"use custom team names" };
	const CvarData useCustomGameMsgs =			{ "use_custom_game_messages",		"use custom game messages" };
	const CvarData useCustomMainMenuLoc =		{ "use_custom_main_menu_location",	"use custom main menu location" };
	const CvarData enableMotD =					{ "enable_message_of_the_day",		"enable message of the day (rolling footer message)" };
	const CvarData useSingleMotdColor =			{ "use_single_motd_color",			"use a single color for message of the day" };
	const CvarData useGradientMotdColor =		{ "use_gradient_motd_color",		"use a gradient of colors for message of the day" };

	// strings
	const CvarData selectedAdName =				{ "selected_ad_name",				"selected ad texture" };
	const CvarData blueTeamName =				{ "blue_team_name",					"custom blue team name" };
	const CvarData orangeTeamName =				{ "orange_team_name",				"custom orange team name" };
	const CvarData motd =						{ "message_of_the_day",				"custom message of the day (rolling text in menu footer)" };

	const CvarData teammateScoredMessage =		{ "teammate_scored_message",		"teammate goal scored message" };
	const CvarData oppScoredMessage =			{ "opponent_scored_message",		"opponent goal scored message" };
	const CvarData userScoredMessage =			{ "user_scored_message",			"user goal scored message" };
	const CvarData goMessage =					{ "go_message",						"custom 'Go!' message" };
	const CvarData countdownMsg3 =				{ "countdown_message_3",			"custom '3' message" };
	const CvarData countdownMsg2 =				{ "countdown_message_2",			"custom '2' message" };
	const CvarData countdownMsg1 =				{ "countdown_message_1",			"custom '1' message" };

	// numbers
	const CvarData mainMenuX =					{ "main_menu_X",					"X coordinate of main menu map location" };
	const CvarData mainMenuY =					{ "main_menu_Y",					"Y coordinate of main menu map location" };
	const CvarData mainMenuZ =					{ "main_menu_Z",					"Z coordinate (height) of main menu map location" };
	const CvarData customFOV =					{ "custom_fov",						"custom camera FOV" };
	const CvarData rgbSpeed =					{ "rgb_speed",						"RGB color cycle speed" };

	// colors
	const CvarData blueTeamFieldColor =			{ "blue_team_field_color",			"blue team field color" };
	const CvarData orangeTeamFieldColor =		{ "orange_team_field_color",		"orange team field color" };
	const CvarData singleFreeplayColor =		{ "single_freeplay_color",			"separate (single) freeplay color" };
	const CvarData motdSingleColor =			{ "motd_single_color",				"solid color for rolling footer message text" };
	const CvarData motdGradientColorBegin =		{ "motd_gradient_color_begin",		"beginning gradient color for motd" };
	const CvarData motdGradientColorEnd =		{ "motd_gradient_color_end",		"end gradient color for motd" };


	// ============================ commands ============================

	const CvarData applyAdTexture =				{ "apply_ad_texture",				"apply ad texture" };
	const CvarData applyTeamColors =			{ "apply_team_colors",				"apply team colors" };
	const CvarData applyTeamNames =				{ "apply_team_names",				"apply custom team names" };
	const CvarData showBallTrail =				{ "show_ball_trail",				"show the ball trail" };
	const CvarData changeMessageOfTheDay =		{ "apply_motd",						"change the lil rolling text message on the bottom of screen" };
	const CvarData checkGameState =				{ "check_game_state",				"log the current game state" };
	const CvarData exitToMainMenu =				{ "exit_to_main_menu",				"exit to main menu (as if the button were pressed)" };
	const CvarData forfeit =					{ "forfeit",						"vote to forfeit" };
	const CvarData changeReplayMap =			{ "change_replay_map",				"change the current replay map" };

	const CvarData test =						{ "test",							"test" };
	const CvarData test2 =						{ "test2",							"test 2" };
	const CvarData test3 =						{ "test3",							"test 3" };
	const CvarData testerColors =				{ "tester_colors",					"test changing colors" };
}


