#include "pch.h"
#include "StadiumDrip.hpp"
#include "Macros.hpp"
#include "components/Textures.hpp"
#include "components/Teams.hpp"
#include "components/Messages.hpp"
#include "components/MainMenu.hpp"
#include "components/Replays.hpp"

void StadiumDrip::RenderSettings()
{
	const float content_height = ImGui::GetContentRegionAvail().y - footer_height; // available height after accounting for footer

	if (ImGui::BeginChild("PluginSettingsSection", ImVec2(0, content_height)))
	{
		GUI::alt_settings_header(h_label.c_str(), pretty_plugin_version, gameWrapper);

		GUI::Spacing(4);

		// open bindings window button
		std::string openMenuCommand = "togglemenu " + GetMenuName();
		if (ImGui::Button("Open Menu"))
		{
			GAME_THREAD_EXECUTE_CAPTURE(cvarManager->executeCommand(openMenuCommand);, openMenuCommand);
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
		Teams.display();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Messages"))
	{
		Messages.display();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Main Menu"))
	{
		Mainmenu.display();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Ads"))
	{
		Textures.display();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Replays"))
	{
		Replays.display();
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
	auto unlock_all_menu_nodes_cvar = getCvar(Cvars::unlock_all_menu_nodes);
	if (!unlock_all_menu_nodes_cvar)
		return;

	GUI::Spacing(4);

	bool unlock_all_menu_nodes = unlock_all_menu_nodes_cvar.getBoolValue();
	if (ImGui::Checkbox("Unlock all menu nodes", &unlock_all_menu_nodes))
		unlock_all_menu_nodes_cvar.setValue(unlock_all_menu_nodes);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Enables all menu options in freeplay. Like creating a private match, joining tournaments, etc.");
}