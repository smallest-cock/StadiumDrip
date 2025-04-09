#include "pch.h"
#include "StadiumDrip.h"



void StadiumDrip::RenderSettings()
{
	const float content_height = ImGui::GetContentRegionAvail().y - footer_height;	// available height after accounting for footer

	if (ImGui::BeginChild("PluginSettingsSection", ImVec2(0, content_height)))
	{
		GUI::alt_settings_header(h_label.c_str(), pretty_plugin_version);

		GUI::Spacing(4);

		// open bindings window button
		std::string openMenuCommand = "togglemenu " + GetMenuName();
		if (ImGui::Button("Open Menu"))
		{
			GAME_THREAD_EXECUTE_CAPTURE(
				cvarManager->executeCommand(openMenuCommand);
			, openMenuCommand);
		}

		GUI::Spacing(8);

		ImGui::Text("or bind this command:  ");
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		ImGui::InputText("", &openMenuCommand, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
	}
	ImGui::EndChild();

	GUI::alt_settings_footer("Need help? Join the Discord", "https://discord.gg/d5ahhQmJbJ");
}


void StadiumDrip::RenderWindow()
{
	ImGui::BeginTabBar("##Tabs");

	if (ImGui::BeginTabItem("Teams"))
	{
		Teams_Tab();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Messages"))
	{
		Messages_Tab();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Main Menu"))
	{
		MainMenu_Tab();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Ads"))
	{
		Ads_Tab();
		ImGui::EndTabItem();
	}
	
	if (ImGui::BeginTabItem("Replays"))
	{
		Replays_Tab();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Misc"))
	{
		Misc_Tab();
		ImGui::EndTabItem();
	}
}


void StadiumDrip::Misc_Tab()
{
	auto unlock_all_menu_nodes_cvar = GetCvar(Cvars::unlock_all_menu_nodes);
	if (!unlock_all_menu_nodes_cvar)
		return;

	GUI::Spacing(4);

	bool unlock_all_menu_nodes = unlock_all_menu_nodes_cvar.getBoolValue();
	if (ImGui::Checkbox("Unlock all menu nodes", &unlock_all_menu_nodes))
	{
		unlock_all_menu_nodes_cvar.setValue(unlock_all_menu_nodes);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Enables all menu options in freeplay. Like creating a private match, joining tournaments, etc.");
	}
}


// teams tab
void StadiumDrip::Teams_Tab()
{
	auto blueTeamFieldColor_cvar =      GetCvar(Cvars::blue_team_color);
	auto orangeTeamFieldColor_cvar =    GetCvar(Cvars::orange_team_color);
	auto useCustomTeamNames_cvar =      GetCvar(Cvars::use_custom_team_names);
	auto blueTeamName_cvar =            GetCvar(Cvars::blue_team_name);
	auto orangeTeamName_cvar =          GetCvar(Cvars::orange_team_name);
	auto useCustomTeamColors_cvar =		GetCvar(Cvars::use_custom_team_colors);
	auto useSingleFreeplayColor_cvar =  GetCvar(Cvars::use_single_freeplay_color);
	auto singleFreeplayColor_cvar =     GetCvar(Cvars::single_freeplay_color);
	auto useRGBFreeplayColors_cvar =    GetCvar(Cvars::use_rgb_freeplay_colors);
	auto rgbSpeed_cvar =				GetCvar(Cvars::rgb_speed);

	// if any is null, they prolly all are...
	if (!blueTeamFieldColor_cvar || !orangeTeamFieldColor_cvar || !useCustomTeamNames_cvar || !blueTeamName_cvar || !orangeTeamName_cvar ||
		!useCustomTeamColors_cvar || !useSingleFreeplayColor_cvar || !singleFreeplayColor_cvar || !useRGBFreeplayColors_cvar || !rgbSpeed_cvar) return;


	const float team_names_height = ImGui::GetContentRegionAvail().y * 0.3f;	// 50% available height

	if (ImGui::BeginChild("teamNames", ImVec2(0, team_names_height), true))
	{
		GUI::Spacing(2);

		// enable custom team names checkbox
		bool useCustomTeamNames = useCustomTeamNames_cvar.getBoolValue();
		if (ImGui::Checkbox("custom team names", &useCustomTeamNames))
		{
			useCustomTeamNames_cvar.setValue(useCustomTeamNames);
		}

		if (useCustomTeamNames)
		{
			GUI::Spacing(2);

			// custom blue team name
			std::string blueTeamName = blueTeamName_cvar.getStringValue();
			ImGui::SetNextItemWidth(itemWidth);
			if (ImGui::InputText("blue", &blueTeamName))
			{
				blueTeamName_cvar.setValue(blueTeamName);
			}

			GUI::SameLineSpacing_relative(sameLineSpacing);

			// custom orange team name
			std::string orangeTeamName = orangeTeamName_cvar.getStringValue();
			ImGui::SetNextItemWidth(itemWidth);
			if (ImGui::InputText("orange", &orangeTeamName))
			{
				orangeTeamName_cvar.setValue(orangeTeamName);
			}
		}
	}
	ImGui::EndChild();

	if (ImGui::BeginChild("teamColors", ImGui::GetContentRegionAvail(), true))
	{
		const float match_colors_width = ImGui::GetContentRegionAvail().x * 0.5f;

		if (ImGui::BeginChild("matchTeamColors", ImVec2(match_colors_width, 0), true))
		{
			ImGui::TextColored(GUI::Colors::Yellow, "Matches");

			GUI::Spacing(2);

			// enable custom team colors checkbox
			bool useCustomTeamColors = useCustomTeamColors_cvar.getBoolValue();
			if (ImGui::Checkbox("custom team colors", &useCustomTeamColors))
			{
				useCustomTeamColors_cvar.setValue(useCustomTeamColors);
			}

			if (useCustomTeamColors)
			{
				GUI::Spacing(2);

				// blue team color
				LinearColor blueTeamFieldColor = blueTeamFieldColor_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("blue##FieldColor", &blueTeamFieldColor.R, ImGuiColorEditFlags_NoInputs))
				{
					blueTeamFieldColor_cvar.setValue(blueTeamFieldColor * 255);
				}

				GUI::Spacing(2);

				// orange team color
				LinearColor orangeTeamFieldColor = orangeTeamFieldColor_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("orange##FieldColor", &orangeTeamFieldColor.R, ImGuiColorEditFlags_NoInputs))
				{
					orangeTeamFieldColor_cvar.setValue(orangeTeamFieldColor * 255);
				}

				GUI::Spacing(2);

				if (ImGui::Button("Apply##custom_team_colors"))
				{
					GAME_THREAD_EXECUTE(
						ApplyCustomMatchColors();
					);
				}
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGui::BeginChild("freeplayTeamColors", ImGui::GetContentRegionAvail(), true))
		{
			ImGui::TextColored(GUI::Colors::Yellow, "Freeplay");

			GUI::Spacing(2);

			// enable custom team colors checkbox
			bool useSingleFreeplayColor = useSingleFreeplayColor_cvar.getBoolValue();
			if (ImGui::Checkbox("solid color", &useSingleFreeplayColor))
			{
				useSingleFreeplayColor_cvar.setValue(useSingleFreeplayColor);
			}

			if (useSingleFreeplayColor)
			{
				GUI::SameLineSpacing_relative(30);

				// solid freeplay color
				LinearColor singleFreeplayColor = singleFreeplayColor_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("color##FieldColor", &singleFreeplayColor.R, ImGuiColorEditFlags_NoInputs))
				{
					singleFreeplayColor_cvar.setValue(singleFreeplayColor * 255);
				}
			}

			GUI::Spacing(2);

			bool useRGBFreeplayColors = useRGBFreeplayColors_cvar.getBoolValue();
			if (ImGui::Checkbox("RGB", &useRGBFreeplayColors))
			{
				useRGBFreeplayColors_cvar.setValue(useRGBFreeplayColors);
			}

			if (useRGBFreeplayColors)
			{
				GUI::Spacing(2);

				// determine slider text
				int rgbSpeed = rgbSpeed_cvar.getIntValue();
				std::string formatStr;
				if (rgbSpeed < defaultRGBSpeed)
				{
					formatStr = std::to_string(std::abs(rgbSpeed) + 1) + "x slower";
				}
				else if (rgbSpeed == defaultRGBSpeed)
				{
					formatStr = "default";
				}
				else
				{
					formatStr = std::to_string(std::abs(rgbSpeed) + 1) + "x faster";
				}

				if (ImGui::SliderInt("RGB speed", &rgbSpeed, -14, 9, formatStr.c_str()))
				{
					rgbSpeed_cvar.setValue(rgbSpeed);
				}

				GUI::Spacing(2);

				if (ImGui::Button("reset##rgbSpeed"))
				{
					rgbSpeed_cvar.setValue(defaultRGBSpeed);
				}
			}
		}
		ImGui::EndChild();

	}
	ImGui::EndChild();
}


// ads tab
void StadiumDrip::Ads_Tab()
{
	auto use_custom_ads_cvar =		GetCvar(Cvars::use_custom_ads);
	auto use_single_ad_image_cvar = GetCvar(Cvars::use_single_ad_image);
	auto block_promo_ads_cvar =		GetCvar(Cvars::block_promo_ads);
	if (!use_custom_ads_cvar || !use_single_ad_image_cvar)
		return;

	GUI::Spacing(4);

	bool block_promo_ads = block_promo_ads_cvar.getBoolValue();
	if (ImGui::Checkbox("Block promotional ads", &block_promo_ads))
		block_promo_ads_cvar.setValue(block_promo_ads);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("If unchecked, RL promo ads will interfere with custom ads in certain locations. Idk why that would be desirable, but the option exists...");

	bool use_custom_ads = use_custom_ads_cvar.getBoolValue();
	if (ImGui::Checkbox("Use custom ads", &use_custom_ads))
		use_custom_ads_cvar.setValue(use_custom_ads);

	if (use_custom_ads)
	{
		GUI::SameLineSpacing_relative(100);

		// open AdTextures button
		if (ImGui::Button("Open AdTextures folder"))
			Files::OpenFolder(Textures.ad_textures_folder);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Images must be in 32-bit PNG format");
	}
	else
		return;

	GUI::Spacing(2);

	bool use_single_ad_image = use_single_ad_image_cvar.getBoolValue();
	if (ImGui::Checkbox("Use single image for all ad locations", &use_single_ad_image))
		use_single_ad_image_cvar.setValue(use_single_ad_image);

	GUI::Spacing(4);

	if (ImGui::Button("Apply"))
	{
		GAME_THREAD_EXECUTE(
			RunCommand(Commands::apply_ad_texture);
		);
	}

	GUI::Spacing(4);

	if (use_single_ad_image)
	{
		auto selected_ad_name_cvar = GetCvar(Cvars::selected_ad_name);
		AdTexturesDropdown(selected_ad_name_cvar);

		ImGui::SameLine();

		// refresh/parse ad textures button
		if (ImGui::Button("Refresh"))
		{
			GAME_THREAD_EXECUTE(
				Textures.find_available_ad_imgs();
			);
		}
	}
	else
	{
		auto it = Textures.map_ad_mic_data.find(Textures.current_map_name);
		if (it != Textures.map_ad_mic_data.end())
		{
			auto& mic_map = it->second;

			for (auto& [mic_name, mic_data] : mic_map)
			{
				ImGui::PushID(&mic_data);

				mic_data.display(mic_name, gameWrapper);

				ImGui::PopID();
			}
		}
		
		GUI::Spacing(4);
		
		if (ImGui::CollapsingHeader("debug info"))
		{
			// testing
			ImGui::Text("onload_json_data size: %i", Textures.onload_json_data.size());
			ImGui::Text("map_ad_mic_data size: %i", Textures.map_ad_mic_data.size());
			ImGui::Text("ad_img_options_map size: %i", Textures.ad_img_options_map.size());
		}
	}
}


// messages tab
void StadiumDrip::Messages_Tab()
{
	auto enable_motd_cvar =					GetCvar(Cvars::enable_motd);
	auto motd_cvar =						GetCvar(Cvars::motd);
	auto raw_html_motd_cvar =				GetCvar(Cvars::raw_html_motd);
	auto use_single_motd_color_cvar =		GetCvar(Cvars::use_single_motd_color);
	auto motd_single_color_cvar =			GetCvar(Cvars::motd_single_color);
	auto motd_font_size_cvar =				GetCvar(Cvars::motd_font_size);
	auto use_gradient_motd_color_cvar =		GetCvar(Cvars::use_gradient_motd_color);
	auto motd_gradient_color_begin_cvar =	GetCvar(Cvars::motd_gradient_color_begin);
	auto motd_gradient_color_end_cvar =		GetCvar(Cvars::motd_gradient_color_end);
	if (!enable_motd_cvar || !motd_cvar || !use_single_motd_color_cvar)
		return;

	auto use_custom_game_messages_cvar =	GetCvar(Cvars::use_custom_game_messages);
	auto countdown_msg_3_cvar =				GetCvar(Cvars::countdown_msg_3);
	auto countdown_msg_2_cvar =				GetCvar(Cvars::countdown_msg_2);
	auto countdown_msg_1_cvar =				GetCvar(Cvars::countdown_msg_1);
	auto go_message_cvar =					GetCvar(Cvars::go_message);
	auto user_scored_msg_cvar =				GetCvar(Cvars::user_scored_msg);
	auto teammate_scored_msg_cvar =			GetCvar(Cvars::teammate_scored_msg);
	auto opponent_scored_msg_cvar =			GetCvar(Cvars::opponent_scored_msg);
	if (!use_custom_game_messages_cvar || !countdown_msg_3_cvar || !countdown_msg_2_cvar)
		return;


	const float motd_height = ImGui::GetContentRegionAvail().y * 0.5f;

	if (ImGui::BeginChild("customMOTD", ImVec2(0, motd_height), true))
	{
		GUI::Spacing(2);

		// enable custom team names checkbox
		bool enableMotD = enable_motd_cvar.getBoolValue();
		if (ImGui::Checkbox("Custom message of the day (MOTD)", &enableMotD))
			enable_motd_cvar.setValue(enableMotD);

		if (enableMotD)
		{
			GUI::Spacing(2);

			bool useSingleMotdColor = use_single_motd_color_cvar.getBoolValue();
			bool use_gradient_motd_color = use_gradient_motd_color_cvar.getBoolValue();
			
			int radioState = 0;

			if (useSingleMotdColor)
			{
				radioState = 1;
			}
			else if (use_gradient_motd_color)
			{
				radioState = 2;
			}

			if (ImGui::RadioButton("custom HTML", &radioState, 0))
			{
				use_single_motd_color_cvar.setValue(false);
				use_gradient_motd_color_cvar.setValue(false);
			}
			if (ImGui::IsItemHovered())
			{
				constexpr const char* tooltip =	"Put regular text or use HTML tags to customize the appearance:\n\n"
												"\t\t<font size=\"25\" color=\"#FF0000\">Big red text</font>"
												"\n\nYou can even wrap individual characters in a <font> tag to make colorful designs";

				ImGui::SetTooltip(tooltip);
			}

			if (ImGui::RadioButton("colored text", &radioState, 1))
			{
				use_single_motd_color_cvar.setValue(true);
				use_gradient_motd_color_cvar.setValue(false);
			}
			if (ImGui::RadioButton("gradient color text", &radioState, 2))
			{
				use_single_motd_color_cvar.setValue(false);
				use_gradient_motd_color_cvar.setValue(true);
			}

			GUI::Spacing(2);

			std::string motd = Format::UnescapeQuotesHTML(radioState == 0 ? raw_html_motd_cvar.getStringValue() : motd_cvar.getStringValue());
			if (ImGui::InputText("message", &motd))
			{
				switch (radioState)
				{
				case 0:
					raw_html_motd_cvar.setValue(Format::EscapeQuotesHTML(motd));
					break;
				default:
					motd_cvar.setValue(Format::EscapeQuotesHTML(motd));
					break;
				}
			}

			GUI::SameLineSpacing_relative(10.0f);

			if (ImGui::Button("Apply"))
			{
				GAME_THREAD_EXECUTE(
					Messages.calculate_motd_text();
					Messages.apply_custom_motd();
				);
			}

			if (useSingleMotdColor)
			{
				// color picker
				LinearColor motdSingleColor = motd_single_color_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("color##motdSingleColor", &motdSingleColor.R, ImGuiColorEditFlags_NoInputs))
				{
					motd_single_color_cvar.setValue(motdSingleColor * 255);
				}

				GUI::SameLineSpacing_absolute(150);
				ImGui::SetNextItemWidth(100);

				int motd_font_size = motd_font_size_cvar.getIntValue();
				if (ImGui::InputInt("font size", &motd_font_size))
				{
					motd_font_size_cvar.setValue(motd_font_size);
				}
			}
			else if (use_gradient_motd_color)
			{
				// 2 color pickers
				LinearColor motd_gradient_color_begin = motd_gradient_color_begin_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("start##motdGradientColor", &motd_gradient_color_begin.R, ImGuiColorEditFlags_NoInputs))
				{
					motd_gradient_color_begin_cvar.setValue(motd_gradient_color_begin * 255);
				}

				GUI::SameLineSpacing_absolute(150);
				ImGui::SetNextItemWidth(100);

				int motd_font_size = motd_font_size_cvar.getIntValue();
				if (ImGui::InputInt("font size", &motd_font_size))
				{
					motd_font_size_cvar.setValue(motd_font_size);
				}

				LinearColor motd_gradient_color_end = motd_gradient_color_end_cvar.getColorValue() / 255;	// converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("end##motdGradientColor", &motd_gradient_color_end.R, ImGuiColorEditFlags_NoInputs))
				{
					motd_gradient_color_end_cvar.setValue(motd_gradient_color_end * 255);
				}
			}
		}
	}
	ImGui::EndChild();

	if (ImGui::BeginChild("gameMessages", ImGui::GetContentRegionAvail(), true))
	{
		// enable custom team names checkbox
		bool useCustomGameMsgs = use_custom_game_messages_cvar.getBoolValue();
		if (ImGui::Checkbox("Custom game messages", &useCustomGameMsgs))
			use_custom_game_messages_cvar.setValue(useCustomGameMsgs);

		if (useCustomGameMsgs)
		{
			GUI::Spacing(2);

			if (ImGui::CollapsingHeader("Countdown messages##collapsing"))
			{
				GUI::Spacing(2);

				std::string countdownMsg3 = countdown_msg_3_cvar.getStringValue();
				if (ImGui::InputText("3", &countdownMsg3))
					countdown_msg_3_cvar.setValue(countdownMsg3);

				GUI::Spacing();

				std::string countdownMsg2 = countdown_msg_2_cvar.getStringValue();
				if (ImGui::InputText("2", &countdownMsg2))
					countdown_msg_2_cvar.setValue(countdownMsg2);

				GUI::Spacing();

				std::string countdownMsg1 = countdown_msg_1_cvar.getStringValue();
				if (ImGui::InputText("1", &countdownMsg1))
					countdown_msg_1_cvar.setValue(countdownMsg1);

				GUI::Spacing();

				std::string goMessage = go_message_cvar.getStringValue();
				if (ImGui::InputText("Go!", &goMessage))
					go_message_cvar.setValue(goMessage);
			}

			GUI::Spacing(2);

			if (ImGui::CollapsingHeader("Goal scored messages##collapsing"))
			{
				GUI::Spacing(2);

				std::string userScoredMessage = user_scored_msg_cvar.getStringValue();
				if (ImGui::InputTextWithHint("Me", "i'm a god", &userScoredMessage))
					user_scored_msg_cvar.setValue(userScoredMessage);

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("TIP: Use {Player} in your message to include the name of the player who scored");

				GUI::Spacing();

				std::string teammateScoredMessage = teammate_scored_msg_cvar.getStringValue();
				if (ImGui::InputTextWithHint("Teammate", "{Player} just peaked!", &teammateScoredMessage))
					teammate_scored_msg_cvar.setValue(teammateScoredMessage);

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("TIP: Use {Player} in your message to include the name of the player who scored");

				GUI::Spacing();

				std::string oppScoredMessage = opponent_scored_msg_cvar.getStringValue();
				if (ImGui::InputTextWithHint("Opponent", "{Player} is a tryhard!", &oppScoredMessage))
					opponent_scored_msg_cvar.setValue(oppScoredMessage);

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("TIP: Use {Player} in your message to include the name of the player who scored");
			}
		}

	}
	ImGui::EndChild();
}


// main menu tab
void StadiumDrip::MainMenu_Tab()
{
	auto useCustomMainMenuLoc_cvar =			GetCvar(Cvars::use_custom_mm_location);
	auto mainMenuX_cvar =						GetCvar(Cvars::main_menu_X);
	auto mainMenuY_cvar =						GetCvar(Cvars::main_menu_Y);
	auto mainMenuZ_cvar =						GetCvar(Cvars::main_menu_Z);
	auto customFOV_cvar =						GetCvar(Cvars::custom_fov);
	auto remember_mm_camera_rotation_cvar =		GetCvar(Cvars::remember_mm_camera_rotation);
	if (!mainMenuX_cvar || !mainMenuY_cvar || !mainMenuZ_cvar || !customFOV_cvar || !useCustomMainMenuLoc_cvar)
		return;

	const float mm_location_height = ImGui::GetContentRegionAvail().y * 0.6f;

	if (ImGui::BeginChild("mainMenuLocation", ImVec2(0, mm_location_height), true))
	{
		GUI::Spacing(2);

		// enable custom team names checkbox
		bool useCustomMainMenuLoc = useCustomMainMenuLoc_cvar.getBoolValue();
		if (ImGui::Checkbox("custom location", &useCustomMainMenuLoc))
			useCustomMainMenuLoc_cvar.setValue(useCustomMainMenuLoc);

		if (useCustomMainMenuLoc)
		{
			GUI::Spacing(2);
			
			// X
			float mainMenuX = mainMenuX_cvar.getIntValue();
			if (ImGui::SliderFloat("X##sliderX", &mainMenuX, -50000, 50000, "%.0f"))
			{
				mainMenuX_cvar.setValue(mainMenuX);
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			if (ImGui::InputFloat("##inputX", &mainMenuX, 10.0f, 100.0f, "%.0f"))
			{
				mainMenuX_cvar.setValue(mainMenuX);
			}
			ImGui::PopItemWidth();

			// Y
			float mainMenuY = mainMenuY_cvar.getIntValue();
			if (ImGui::SliderFloat("Y##sliderY", &mainMenuY, -50000, 50000, "%.0f"))
			{
				mainMenuY_cvar.setValue(mainMenuY);
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			if (ImGui::InputFloat("##inputY", &mainMenuY, 10.0f, 100.0f, "%.0f"))
			{
				mainMenuY_cvar.setValue(mainMenuY);
			}
			ImGui::PopItemWidth();

			// Z
			float mainMenuZ = mainMenuZ_cvar.getFloatValue();
			if (ImGui::SliderFloat("Z##sliderZ", &mainMenuZ, -50000, 50000, "%.0f"))
			{
				mainMenuZ_cvar.setValue(mainMenuZ);
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			if (ImGui::InputFloat("##inputZ", &mainMenuZ, 10.0f, 100.0f, "%.0f"))
			{
				mainMenuZ_cvar.setValue(mainMenuZ);
			}
			ImGui::PopItemWidth();

			GUI::Spacing(2);

			if (ImGui::Button("Apply##mainMenuLocation"))
			{
				FVector newLocation = { mainMenuX, mainMenuY, mainMenuZ };

				GAME_THREAD_EXECUTE_CAPTURE(
					Mainmenu.SetLocation(newLocation);
				, newLocation);
			}

			GUI::SameLineSpacing_relative(25);

			if (ImGui::Button("Reset##mainMenuLocation"))
			{
				// reset cvar values
				mainMenuX_cvar.setValue(Mainmenu.default_mm_car_X);
				mainMenuY_cvar.setValue(Mainmenu.default_mm_car_Y);
				mainMenuZ_cvar.setValue(Mainmenu.default_mm_car_Z);

				GAME_THREAD_EXECUTE(
					Mainmenu.ResetLocation(true);
				);
			}
		}
	}
	ImGui::EndChild();

	if (ImGui::BeginChild("customFOV", ImGui::GetContentRegionAvail(), true))
	{
		bool remember_mm_camera_rotation = remember_mm_camera_rotation_cvar.getBoolValue();
		if (ImGui::Checkbox("Remember camera rotation", &remember_mm_camera_rotation))
		{
			remember_mm_camera_rotation_cvar.setValue(remember_mm_camera_rotation);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Rotation will only be saved if you use the mouse to rotate... not a controller stick\n\nbecause Psyonix");
		}

		GUI::Spacing(2);

		int customFOV = customFOV_cvar.getIntValue();
		if (ImGui::SliderInt("FOV", &customFOV, 1, 170))
		{
			customFOV_cvar.setValue(customFOV);

			// TODO: move to a cvar callback function
			GAME_THREAD_EXECUTE_CAPTURE(
				Mainmenu.SetCameraFOV(customFOV);
			, customFOV);
		}

		GUI::SameLineSpacing_relative(25);

		if (ImGui::Button("Reset##mainMenuFOV"))
		{
			// reset cvar values
			customFOV_cvar.setValue(Mainmenu.default_mm_FOV);

			GAME_THREAD_EXECUTE(
				Mainmenu.ResetCameraFOV(true);
			);
		}

		GUI::Spacing(2);

		MainMenuBackgroundsDropdown();
	}
	ImGui::EndChild();
}


// replays tab
void StadiumDrip::Replays_Tab()
{
	auto useAltReplayMapSwitch_cvar = GetCvar(Cvars::use_alt_replay_map_switch);
	if (!useAltReplayMapSwitch_cvar) return;

	GUI::Spacing(2);

	bool useAltReplayMapSwitch = useAltReplayMapSwitch_cvar.getBoolValue();
	if (ImGui::Checkbox("use alternative method to change replay map", &useAltReplayMapSwitch))
	{
		useAltReplayMapSwitch_cvar.setValue(useAltReplayMapSwitch);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Might help if changing replay map isn't working");
	}

	GUI::Spacing(8);

	ReplayMapsDropdown();

	GUI::Spacing(4);

	if (ImGui::Button("Update map names"))
	{
		GAME_THREAD_EXECUTE(
			Replays.FindMapNames();
		);
	}
}


void StadiumDrip::AdTexturesDropdown(CVarWrapper& cvar_to_update)
{
	if (cvar_to_update.IsNull())
	{
		ImGui::TextColored(GUI::Colors::LightRed, "ERROR: Cvar is null");
		return;
	}


	if (Textures.ad_img_options_map.empty())
	{
		ImGui::Text("No PNG images found in AdTextures folder ....");
		return;
	}

	char search_buffer[128] = "";  // Buffer for the search input
	std::string cvar_val = cvar_to_update.getStringValue();
	const char* previewValue = cvar_val.c_str();

	if (ImGui::BeginSearchableCombo("ad image##adTextureDropdown", previewValue, search_buffer, sizeof(search_buffer), "search..."))
	{
		std::string search_str_lower = Format::ToLower(search_buffer);	// convert search text to lower

		for (const auto& [filename, img_info] : Textures.ad_img_options_map)
		{
			ImGui::PushID(&img_info);

			// filter results if necessary
			if (!search_str_lower.empty())
			{
				std::string ad_name_lower = Format::ToLower(img_info.name);	// convert ad name text to lower

				if (ad_name_lower.find(search_str_lower) == std::string::npos)
				{
					ImGui::PopID();
					continue;
				}

				if (ImGui::Selectable(img_info.name.c_str(), img_info.name == cvar_val))
				{
					cvar_to_update.setValue(img_info.name);	// update cvar string value

					GAME_THREAD_EXECUTE(
						RunCommand(Commands::apply_ad_texture);
					);
				}
			}
			else
			{
				if (ImGui::Selectable(img_info.name.c_str(), img_info.name == cvar_val))
				{
					cvar_to_update.setValue(img_info.name);	// update cvar string value

					GAME_THREAD_EXECUTE(
						RunCommand(Commands::apply_ad_texture);
					);
				}
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}
}


void StadiumDrip::MainMenuBackgroundsDropdown()
{
	if (Mainmenu.bg_dropdown_names.empty())
	{
		ImGui::Text("No main menu backgrounds found....");
		return;
	}

	char searchBuffer[128] = "";  // Buffer for the search input
	const char* previewValue = Mainmenu.bg_dropdown_names[Mainmenu.selected_bg_dropdown_index].c_str();

	if (ImGui::BeginSearchableCombo("background##backgroundsDropdown", previewValue, searchBuffer, sizeof(searchBuffer), "search..."))
	{
		// convert search text to lower
		std::string searchQuery = Format::ToLower(searchBuffer);

		for (int i = 0; i < Mainmenu.bg_dropdown_names.size(); i++)
		{
			ImGui::PushID(i);

			std::string bgName = Mainmenu.bg_dropdown_names[i];

			// convert title text to lower
			std::string bgNameLower = Format::ToLower(bgName);

			// filter results if necessary
			if (searchBuffer != "")
			{
				if (bgNameLower.find(searchQuery) != std::string::npos)
				{
					if (ImGui::Selectable(bgName.c_str(), Mainmenu.selected_bg_dropdown_index == i))
					{
						Mainmenu.selected_bg_dropdown_index = i;

						// do something when a bg is selected ... 
						if (GetGameState() != States::MainMenu) continue;

						GAME_THREAD_EXECUTE(
							Mainmenu.SetBackground(Mainmenu.working_mmbg_ids[Mainmenu.bg_dropdown_names[Mainmenu.selected_bg_dropdown_index]], true);

							DELAY(1.0f,
								RunCommandInterval(Commands::apply_ad_texture, 3, 1.0f, true);
							);
						);
					}
				}
			}
			else
			{
				if (ImGui::Selectable(bgName.c_str(), Mainmenu.selected_bg_dropdown_index == i))
				{
					Mainmenu.selected_bg_dropdown_index = i;

					// do something when a bg is selected ...
					if (GetGameState() != States::MainMenu) continue;

					GAME_THREAD_EXECUTE(
						Mainmenu.SetBackground(Mainmenu.working_mmbg_ids[Mainmenu.bg_dropdown_names[Mainmenu.selected_bg_dropdown_index]], true);
						
						DELAY(1.0f,
							RunCommandInterval(Commands::apply_ad_texture, 3, 1.0f, true);
						);
					);
				}
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}
}


void StadiumDrip::ReplayMapsDropdown()
{
	if (Replays.mapNames.empty())
	{
		ImGui::Text("No map names found :(");
		return;
	}

	char searchBuffer[128] = "";	// Buffer for the search input
	const char* previewValue = Replays.mapNames[Replays.dropdownPreviewIndex].prettyName.c_str();		// just show first map name in vector as default preview value

	if (ImGui::BeginSearchableCombo("change map##replayMapsDropdown", previewValue, searchBuffer, sizeof(searchBuffer), "search..."))
	{
		// convert search text to lower
		const std::string searchQuery = Format::ToLower(searchBuffer);

		for (int i = 0; i < Replays.mapNames.size(); i++)
		{
			const std::string& internalMapNameStr = Replays.mapNames[i].internalName;
			const std::string& mapNameStr = Replays.mapNames[i].prettyName;
			const std::string mapNameStrLower = Format::ToLower(mapNameStr);

			ImGui::PushID(i);

			// only render option if there's text in search box & it matches the key name
			if (searchBuffer != "")
			{
				if (mapNameStrLower.find(searchQuery) != std::string::npos)
				{
					if (ImGui::Selectable(mapNameStr.c_str(), Replays.dropdownPreviewIndex == i))
					{
						GAME_THREAD_EXECUTE_CAPTURE(
							Replays.ChangeMap(internalMapNameStr);
						, internalMapNameStr);
					}
				}
			}
			// if there's no text in search box, render all possible key options
			else
			{
				if (ImGui::Selectable(mapNameStr.c_str(), Replays.dropdownPreviewIndex == i))
				{
					GAME_THREAD_EXECUTE_CAPTURE(
						Replays.ChangeMap(internalMapNameStr);
					, internalMapNameStr);
				}
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}
}