#include "pch.h"
#include "StadiumDrip.hpp"
#include "Cvars.hpp"
#include "Macros.hpp"
#include <ModUtils/gui/GuiTools.hpp>
#include "components/Textures.hpp"
#include "components/Teams.hpp"
#include "components/Messages.hpp"
#include "components/MainMenu.hpp"
#include "components/Replays.hpp"

void StadiumDrip::RenderSettings()
{
	const float contentHeight = ImGui::GetContentRegionAvail().y - FOOTER_HEIGHT; // available height after accounting for footer

	{
		GUI::ScopedChild c{"PluginSettingsSection", ImVec2(0, contentHeight)};

		GUI::alt_settings_header(h_label.c_str(), pretty_plugin_version, gameWrapper);

		GUI::Spacing(4);

		// open bindings window button
		std::string openMenuCommand = "togglemenu " + GetMenuName();
		if (ImGui::Button("Open Menu"))
		{
			GAME_THREAD_EXECUTE({ cvarManager->executeCommand(openMenuCommand); }, openMenuCommand);
		}

		GUI::Spacing(8);

		ImGui::Text("or bind this command:  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(150.0f);
		ImGui::InputText("", &openMenuCommand, ImGuiInputTextFlags_ReadOnly);
		GUI::CopyButton("Copy", openMenuCommand.c_str());
	}

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
	auto unlockAllMenuNodes_cvar = getCvar(Cvars::unlockAllMenuNodes);
	if (!unlockAllMenuNodes_cvar)
		return;

	GUI::Spacing(4);

	bool unlockAllMenuNodes = unlockAllMenuNodes_cvar.getBoolValue();
	if (ImGui::Checkbox("Unlock all menu nodes", &unlockAllMenuNodes))
		unlockAllMenuNodes_cvar.setValue(unlockAllMenuNodes);
	GUI::ToolTip("Enables all menu options in freeplay. Like creating a private match, joining tournaments, etc.");
}