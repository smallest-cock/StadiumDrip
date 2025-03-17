#pragma once


#define CVAR(name, desc) CvarData("sd_" name, desc)		// automatically apply the prefix to cvar names

struct CvarData
{
	const char* name;
	const char* description;

	constexpr CvarData(const char* name, const char* description) : name(name), description(description) {}
};

namespace Cvars
{
	// bools
	constexpr CvarData use_custom_ads =					CVAR("use_custom_ads",					"use custom ad textures");
	constexpr CvarData use_custom_team_colors =			CVAR("use_custom_team_colors",			"use custom team colors");
	constexpr CvarData use_single_freeplay_color =		CVAR("use_single_freeplay_color",		"use a single freeplay color instead of custom team colors");
	constexpr CvarData use_rgb_freeplay_colors =		CVAR("use_rgb_freeplay_colors",			"use RGB stadium colors in freeplay");
	constexpr CvarData use_alt_replay_map_switch =		CVAR("use_alt_replay_map_switch",		"use alternate method to change replay map" );

	constexpr CvarData use_custom_team_names =			CVAR("use_custom_team_names",			"use custom team names");
	constexpr CvarData use_custom_game_messages =		CVAR("use_custom_game_messages",		"use custom game messages");
	constexpr CvarData use_custom_mm_location =			CVAR("use_custom_main_menu_location",	"use custom main menu location");
	constexpr CvarData enable_motd =					CVAR("enable_motd",						"enable message of the day (rolling footer message)");
	constexpr CvarData use_single_motd_color =			CVAR("use_single_motd_color",			"use a single color for message of the day");
	constexpr CvarData use_gradient_motd_color =		CVAR("use_gradient_motd_color",			"use a gradient of colors for message of the day");

	// strings
	constexpr CvarData selected_ad_name =				CVAR("selected_ad_name",				"selected ad texture");
	constexpr CvarData blue_team_name =					CVAR("blue_team_name",					"custom blue team name");
	constexpr CvarData orange_team_name =				CVAR("orange_team_name",				"custom orange team name");
	constexpr CvarData motd =							CVAR("motd",							"custom message of the day (rolling text in menu footer)");

	constexpr CvarData teammate_scored_msg =			CVAR("teammate_scored_message",			"teammate goal scored message");
	constexpr CvarData opponent_scored_msg =			CVAR("opponent_scored_message",			"opponent goal scored message");
	constexpr CvarData user_scored_msg =				CVAR("user_scored_message",				"user goal scored message");
	constexpr CvarData go_message =						CVAR("go_message",						"custom 'Go!' message");
	constexpr CvarData countdown_msg_3 =				CVAR("countdown_message_3",				"custom '3' message");
	constexpr CvarData countdown_msg_2 =				CVAR("countdown_message_2",				"custom '2' message");
	constexpr CvarData countdown_msg_1 =				CVAR("countdown_message_1",				"custom '1' message");

	// numbers
	constexpr CvarData main_menu_X =					CVAR("main_menu_X",						"X coordinate of main menu map location");
	constexpr CvarData main_menu_Y =					CVAR("main_menu_Y",						"Y coordinate of main menu map location");
	constexpr CvarData main_menu_Z =					CVAR("main_menu_Z",						"Z coordinate (height) of main menu map location");
	constexpr CvarData custom_fov =						CVAR("custom_fov",						"custom camera FOV");
	constexpr CvarData rgb_speed =						CVAR("rgb_speed",						"RGB color cycle speed");
	constexpr CvarData motd_font_size =					CVAR("motd_font_size",					"font size for MOTD text");
	constexpr CvarData mm_bg_index =					CVAR("mainmenu_bg_index",				"Main menu background index");

	// colors
	constexpr CvarData blue_team_color =				CVAR("blue_team_field_color",			"blue team field color");
	constexpr CvarData orange_team_color =				CVAR("orange_team_field_color",			"orange team field color");
	constexpr CvarData single_freeplay_color =			CVAR("single_freeplay_color",			"separate (single) freeplay color");
	constexpr CvarData motd_single_color =				CVAR("motd_single_color",				"solid color for rolling footer message text");
	constexpr CvarData motd_gradient_color_begin =		CVAR("motd_gradient_color_begin",		"beginning gradient color for motd");
	constexpr CvarData motd_gradient_color_end =		CVAR("motd_gradient_color_end",			"end gradient color for motd");
}


namespace Commands
{
	constexpr CvarData apply_ad_texture =				CVAR("apply_ad_texture",				"apply ad texture");
	constexpr CvarData apply_team_colors =				CVAR("apply_team_colors",				"apply team colors");
	constexpr CvarData apply_team_names =				CVAR("apply_team_names",				"apply custom team names");
	constexpr CvarData show_ball_trail =				CVAR("show_ball_trail",					"show the ball trail");
	constexpr CvarData apply_motd =						CVAR("apply_motd",						"change the lil rolling text message on the bottom of screen");
	constexpr CvarData check_game_state =				CVAR("check_game_state",				"log the current game state");
	constexpr CvarData exit_to_main_menu =				CVAR("exit_to_main_menu",				"exit to main menu (as if the button were pressed)");
	constexpr CvarData forfeit =						CVAR("forfeit",							"vote to forfeit");
	constexpr CvarData change_replay_map =				CVAR("change_replay_map",				"change the current replay map");
	constexpr CvarData change_mainmenu_bg =				CVAR("change_mainmenu_bg",				"change the main menu background map");

	constexpr CvarData test_1 =							CVAR("test_1",							"test 1");
	constexpr CvarData test_2 =							CVAR("test_2",							"test 2");
	constexpr CvarData test_3 =							CVAR("test_3",							"test 3");
	constexpr CvarData tester_colors =					CVAR("tester_colors",					"test changing colors");
}

#undef CVAR
