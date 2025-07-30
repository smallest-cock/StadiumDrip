#include "pch.h"
#include "Messages.hpp"
#include "Macros.hpp"
#include "Events.hpp"
#include <ModUtils/wrappers/GFxWrapper.hpp>



// ##############################################################################################################
// ###############################################    INIT    ###################################################
// ##############################################################################################################

void MessagesComponent::Initialize(const std::shared_ptr<GameWrapper>& gw)
{
	gameWrapper = gw;

	initCvars();
	initHooks();
	initCommands();
}

void MessagesComponent::initCvars()
{
	// bools
	auto enableMotd_cvar = registerCvar_bool(Cvars::enable_motd, false);
	enableMotd_cvar.bindTo(m_enableMotd);
	enableMotd_cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper updatedCvar)
	{
		// if (pluginHasJustBeenLoaded) return;

		bool updatedVal = updatedCvar.getBoolValue();

		if (updatedVal)
		{
			GAME_THREAD_EXECUTE(Messages.applyCustomMotd();
			    // RunCommand(Commands::apply_motd);
			);
		}
		// else
		//{
		//	GAME_THREAD_EXECUTE(
		//		Messages.ClearMOTD();
		//	);
		// }

		LOG("The callback worked for {}", cvarName);
		LOG("Set value to: {}", updatedVal);
	});
	
	registerCvar_bool(Cvars::use_single_motd_color, false).bindTo(m_useSolidMotdColor);
	registerCvar_bool(Cvars::use_gradient_motd_color, true).bindTo(m_useGradienMotdColor);
	registerCvar_bool(Cvars::use_custom_game_messages, true).bindTo(m_useCustomGameMessages);
	registerCvar_bool(Cvars::unlock_all_menu_nodes, false).bindTo(m_unlockAllMenuNodes);

	// numbers
	registerCvar_number(Cvars::motd_font_size, 20, true, 1, 300).bindTo(m_motdFontSize);

	// strings
	registerCvar_string(Cvars::user_scored_msg, "i'm that guy pal!").bindTo(m_userScoredMsg);
	registerCvar_string(Cvars::teammate_scored_msg, "{Player} just peaked!").bindTo(m_teammateScoredMsg);
	registerCvar_string(Cvars::opponent_scored_msg, "{Player} is a tryhard!").bindTo(m_oppScoredMsg);
	registerCvar_string(Cvars::motd, "sub to Scrimpf on YT <3").bindTo(m_motd);
	registerCvar_string(Cvars::raw_html_motd, "sub to Scrimpf on YT &lt;3").bindTo(m_motdRawHtml);
	registerCvar_string(Cvars::countdown_msg_3, "get").bindTo(m_countdownMsg3);
	registerCvar_string(Cvars::countdown_msg_2, "ready").bindTo(m_countdownMsg2);
	registerCvar_string(Cvars::countdown_msg_1, "to").bindTo(m_countdownMsg1);
	registerCvar_string(Cvars::go_message, "cook!").bindTo(m_countdownMsgGo);

	// colors
	registerCvar_color(Cvars::motd_single_color, "#FF00FF").bindTo(m_motdSolidColor);
	registerCvar_color(Cvars::motd_gradient_color_begin, "#FFFFFF").bindTo(m_motdGradientColorBegin);
	registerCvar_color(Cvars::motd_gradient_color_end, "#FF00E9").bindTo(m_motdGradientColorEnd);
}

void MessagesComponent::initHooks()
{
	// idk why this in Messages component, but whatever
	hookWithCallerPost(Events::GFxData_MenuTreeNode_TA_CanShowEngagementEventType, [this](ActorWrapper Caller, ...)
	{
		if (!*m_unlockAllMenuNodes)
			return;

		auto* caller = reinterpret_cast<UGFxData_MenuTreeNode_TA*>(Caller.memory_address);
		if (!validUObject(caller))
			return;

		GfxWrapper gfx_node{caller};
		gfx_node.set_bool(L"bEnabled", true);
		gfx_node.set_int(L"BannerType", 0);
	});

	hookWithCaller(Events::PlayerController_TA_ReceiveMessage, [this](ActorWrapper Caller, void* Params, ...)
	{
		if (!*m_useCustomGameMessages)
			return;

		auto params = reinterpret_cast<APlayerController_TA_execReceiveMessage_Params*>(Params);
		if (!params)
			return;

		auto caller = reinterpret_cast<APlayerController_TA*>(Caller.memory_address);
		if (!validUObject(caller))
		{
			LOGERROR("APlayerController_TA* from caller is invalid");
			return;
		}

		setGoalScoredMessage(caller, params->Packet);
	});
	
	hookWithCaller(Events::GameEvent_TA_Countdown_BeginState, [this](ActorWrapper Caller, ...)
	{
		if (!*m_useCustomGameMessages)
			return;

		auto caller = reinterpret_cast<AGameEvent_TA*>(Caller.memory_address);
		if (!validUObject(caller))
		{
			LOGERROR("AGameEvent_TA* from caller is null");
			return;
		}

		setCountdownMessages(caller, {*m_countdownMsgGo, *m_countdownMsg1, *m_countdownMsg2, *m_countdownMsg3});
	});

	auto setMotdUsingDatastore = [this](ActorWrapper Caller, ...)
	{
		if (!*m_enableMotd)
			return;

		auto caller = reinterpret_cast<UGFxData_StartMenu_TA*>(Caller.memory_address);
		if (!caller)
			return;

		GfxWrapper startmenu{caller};
		auto       ds = startmenu.get_datastore();
		if (!validUObject(ds))
		{
			LOGERROR("Datastore instance from startmenu GfxWrapper is invalid");
			return;
		}

		applyCustomMotd(nullptr, ds);
	};
	hookWithCaller(Events::PushMenu, setMotdUsingDatastore);
	hookWithCaller(Events::GFxData_StartMenu_TA_ProgressToMainMenu, setMotdUsingDatastore);

	auto setMotdUsingGFxData_Community_TA = [this] (ActorWrapper Caller, ...)
	{
		if (!*m_enableMotd)
			return;

		auto caller = reinterpret_cast<UGFxData_Community_TA*>(Caller.memory_address);
		if (!validUObject(caller))
			return;

		applyCustomMotd(caller);
	};
	hookWithCaller(Events::GFxData_Community_TA_OnShellSet, setMotdUsingGFxData_Community_TA);
	hookWithCallerPost(Events::GFxData_Community_TA_SetInitialLoadCompleted, setMotdUsingGFxData_Community_TA);
}

void MessagesComponent::initCommands()
{
	registerCommand(Commands::apply_motd, [this](std::vector<std::string> args)
	{
		applyCustomMotd();
	});
}



// ##############################################################################################################
// ###############################################    FUNCTIONS    ##############################################
// ##############################################################################################################

void MessagesComponent::spawnNotification(const std::string& title, const std::string& content, float duration, bool log)
{
	auto notificationManager = Instances.GetInstanceOf<UNotificationManager_TA>();
	if (!validUObject(notificationManager))
		return;

	static UClass* notificationClass = nullptr;
	if (!validUObject(notificationClass))
		notificationClass = UGenericNotification_TA::StaticClass();

	UNotification_TA* notification = notificationManager->PopUpOnlyNotification(notificationClass);
	if (!validUObject(notification))
		return;

	notification->SetTitle(FString::create(title));
	notification->SetBody(FString::create(content));
	notification->PopUpDuration = duration;

	if (log)
		LOG("[{}] {}", title.c_str(), content.c_str());
}

std::string MessagesComponent::getCustomMotdText()
{
	if (m_currentMotdText.empty())
		calculateMotdText();

	return m_currentMotdText;
}

void MessagesComponent::calculateMotdText()
{
	if (!*m_enableMotd)
		return;

	// basically unescapes my shitty "encoding" used to safely store a cvar string containing # and " in a .cfg file
	const std::string rawText = Format::UnescapeQuotesHTML((*m_useSolidMotdColor || *m_useGradienMotdColor) ? *m_motd : *m_motdRawHtml);
	std::string calculatedText = rawText;

	if (*m_useSolidMotdColor)
	{
		const std::string hex_color = Format::LinearColorToHex(*m_motdSolidColor, false);
		calculatedText = std::format("<font size=\"{}\" color=\"{}\">{}</font>", *m_motdFontSize, hex_color, Format::EscapeForHTML(rawText));
	}
	else if (*m_useGradienMotdColor)
	{
		calculatedText = MessagesComponent::getGradientMotdString(
			rawText, *m_motdFontSize, *m_motdGradientColorBegin, *m_motdGradientColorEnd);
	}

	m_currentMotdText = calculatedText;
	LOG("Calculated MotD text: {}", m_currentMotdText);
}

void MessagesComponent::applyCustomMotd(UGFxData_Community_TA* community, UGFxDataStore_X* datastore)
{
	if (!*m_enableMotd)
		return;

	FString motdFstr = FString::create(getCustomMotdText());

	if (validUObject(community))
	{
		applyMotdUsingCommunityGfx(community, motdFstr);
		DEBUGLOG("Set MotD using UGFxData_Community_TA instance");
	}
	else if (validUObject(datastore))
	{
		datastore->SetStringValue(L"Community", 0, L"MotD", motdFstr);
		
		DEBUGLOG("Set MotD using UGFxDataStore_X instance");
	}
	else
	{
		auto community_gfx = Instances.GetInstanceOf<UGFxData_Community_TA>();
		if (!validUObject(community_gfx))
		{
			LOGERROR("UGFxData_Community_TA* is invalid");
			return;
		}

		applyMotdUsingCommunityGfx(community_gfx, motdFstr);
		DEBUGLOG("Set MotD using no instances");
	}
}

void MessagesComponent::applyMotdUsingCommunityGfx(UGFxData_Community_TA* community, const FString& motd)
{
	if (!validUObject(community))
		return;

	community->MotD = motd;

	UBlogConfig_X* blogConfig = community->Config;
	if (blogConfig && blogConfig->IsA<UBlogConfig_X>())
	{
		blogConfig->MotD = motd;
		blogConfig->Apply();

		//blog_config->ModifyObjects(blog_config->StaticClass(), FScriptDelegate{}, FScriptDelegate{});
	}

	GfxWrapper communityGfx{ community };
	communityGfx.set_string(L"MotD", motd);
}

std::string MessagesComponent::getGradientMotdString(const std::string& text, int size, const LinearColor& start_col, const LinearColor& end_col)
{
	auto colors = getGradientColors(
		text,
		{ start_col.R, start_col.G , start_col.B , start_col.A },
		{ end_col.R, end_col.G , end_col.B , end_col.A }
	);

	if (colors.size() != text.length())
	{
		LOGERROR("Gradient MOTD mismatch. Text string size = {}, Color list size = {}", text.length(), colors.size());
		return text;
	}

	std::string motd_string;
	auto len = text.length();

	for (int i = 0; i < len; ++i)
	{
		const char& ch = text[i];
		const std::string& hex_col = colors[i];

		std::string escaped_char = Format::EscapeCharForHTML(ch);

		motd_string += std::format("<font size=\"{}\" color=\"{}\">{}</font>", size, hex_col, escaped_char);
	}

	return motd_string;
}

std::vector<std::string> MessagesComponent::getGradientColors(const std::string& text, const FLinearColor& start_col, const FLinearColor& end_col)
{
	std::vector<std::string> result;
	result.reserve(text.length());

	if (text.empty())
		return result;

	static auto flinear_to_linear = [](const FLinearColor & c) { return LinearColor(c.R, c.G, c.B, c.A); };

	auto len = text.length();
	for (int i = 0; i < len; ++i)
	{
		float t = (len == 1) ? 0.0f : static_cast<float>(i) / (len - 1);
		FLinearColor interpolated = UObject::LinearColorLerp(start_col, end_col, t);
		result.push_back(Format::LinearColorToHex(flinear_to_linear(interpolated), false));
	}

	return result;
}

void MessagesComponent::setCountdownMessages(AGameEvent_TA* gameEvent, const std::vector<std::string>& countdownMsgs)
{
	if (!validUObject(gameEvent))
		return;

	int numCountdownMsgs = countdownMsgs.size();
	if (numCountdownMsgs < 4)
		return;

	UMessage_TA* goMsg = gameEvent->GoMessage;
	if (!validUObject(goMsg))
	{
		LOGERROR("Go message UMessage_TA* is invalid");
		return;
	}

	goMsg->LocalizedMessage = FString::create(countdownMsgs[0]);

	auto cdMsgs = gameEvent->CountdownMessages;
	int numMsgs = cdMsgs.size();
	LOG("Num UMessage_TA* in CountdownMessages: {}", numMsgs);

	for (int i = 0; i < numMsgs; ++i)
	{
		UMessage_TA* msg = cdMsgs[i];
		if (!validUObject(msg))
		{
			LOG("CountdownMessages[{}] UMessage_TA* is null!", i);
			continue;
		}

		if (i >= 1 && i < numCountdownMsgs)	// dont try to change the first message in TArray bc it's always null for some reason
			msg->LocalizedMessage = FString::create(countdownMsgs[i]);
	}
}

void MessagesComponent::resetCountdownMessages(AGameEvent_TA* gameEvent)
{
	if (!validUObject(gameEvent))
		return;

	static const std::vector<std::string> countdownMsgs =
	{
		"Go!",
		"{Seconds}",
		"{Seconds}",
		"{Seconds}"
	};

	setCountdownMessages(gameEvent, countdownMsgs);
}

void MessagesComponent::resetGoalScoredMessage(UMessage_TA* msg)
{
	setMessageText(msg, DEFAULT_SCORE_MSG);
}

void MessagesComponent::setGoalScoredMessage(APlayerController_TA* pc, const FMessagePacket& msgPacket)
{
	if (!validUObject(pc))
		return;

	APRI_TA* playercontrollerPri = pc->PRI;
	if (!validUObject(playercontrollerPri) || !playercontrollerPri->IsA<APRI_TA>())
	{
		LOG("APRI_TA* is null from APlayerController_TA");
		return;
	}

	if (!validUObject(playercontrollerPri->GameEvent) || !playercontrollerPri->GameEvent->IsA<AGameEvent_Soccar_TA>())
	{
		LOGERROR("Couldn't cast AGameEvent_TA* to AGameEvent_Soccar_TA*");
		return;
	}
	auto* gameEvent = static_cast<AGameEvent_Soccar_TA*>(playercontrollerPri->GameEvent);

	DEBUGLOG("msgPacket.Values size: {}", msgPacket.Values.size());

	// find scorer's APRI_TA in message packet
	APRI_TA* scorerPri = nullptr;
	for (const auto& val : msgPacket.Values)
	{
		if (val.NameValue != L"Player")
			continue;

		DEBUGLOG("NameValue: {}", val.NameValue.ToString());
		DEBUGLOG("IntValue: {}", val.IntValue);
		DEBUGLOG("StringValue: {}", val.StringValue.ToString());
		DEBUGLOG("ValueType: {}", val.ValueType);
		DEBUGLOG("ObjectValue: {}", Format::ToHexString(val.ObjectValue));

		if (!validUObject(val.ObjectValue) || !val.ObjectValue->IsA<APRI_TA>())
			continue;

		scorerPri = static_cast<APRI_TA*>(val.ObjectValue);
		break;
	}

	if (!scorerPri)
	{
		LOGERROR("Unable to get APRI_TA* from message packet! Setting default goal scored message...");
		//Messages.SpawnNotification("stadium drip", "PRI from msg packet was null!", 3);		// uncomment for testing

		resetGoalScoredMessage(gameEvent->GoalScoredMessage);
		return;
	}

	LOG("=== Scorer's APRI_TA info ===");
	LOG("PlayerName: {}", scorerPri->PlayerName.ToString());
	LOG("IsPlayer: {}", scorerPri->IsPlayer());
	LOG("GetBotName: {}", scorerPri->GetBotName().ToString());
	
	// find scorer's team
	ATeamInfo* scoredPriTeam = scorerPri->Team;
	if (!validUObject(scoredPriTeam))
	{
		LOGERROR("Couldn't resolve scorer's team num...");
		//Messages.SpawnNotification("stadium drip", "[ERROR] Couldn't resolve scorer's team num...", 3);
		resetGoalScoredMessage(gameEvent->GoalScoredMessage);
		return;
	}

	// find user's team
	ATeamInfo* userTeam = playercontrollerPri->Team;
	if (!validUObject(userTeam))
	{
		LOGERROR("Couldn't resolve user's team num...");
		//Messages.SpawnNotification("stadium drip", "[ERROR] Couldn't resolve user's team num...", 3);
		resetGoalScoredMessage(gameEvent->GoalScoredMessage);
		return;
	}

	const auto& scorerTeam = scoredPriTeam->TeamIndex; // this line causes crash when there's no PRI that scored (like own goal nobody touched it)
	const auto& userTeamNum = userTeam->TeamIndex;
	DEBUGLOG("User's team num: {}", userTeamNum);
	DEBUGLOG("Scorer's team num: {}", scorerTeam);

	// determine goal scored message
	std::string scoredMessage = DEFAULT_SCORE_MSG;
	if (scorerTeam == userTeamNum)
	{
		// assumes playercontrollerPri is the user's PRI (not 100% sure)
		bool sameUid = sameId(playercontrollerPri->UniqueId, scorerPri->UniqueId);
		
		scoredMessage = sameUid ? *m_userScoredMsg : *m_teammateScoredMsg;
		DEBUGLOG("Scorer and user UID is the same (user scored): {}", sameUid);
	}
	else
		scoredMessage = *m_oppScoredMsg;

	setMessageText(gameEvent->GoalScoredMessage, scoredMessage);
}

void MessagesComponent::setMessageText(UMessage_TA* msg, const std::string& newText)
{
	if (!validUObject(msg))
		return;

	msg->LocalizedMessage = FString::create(newText);
}



// ##############################################################################################################
// ##########################################    DISPLAY FUNCTIONS    ###########################################
// ##############################################################################################################

void MessagesComponent::display()
{
	auto enable_motd_cvar               = getCvar(Cvars::enable_motd);
	auto motd_cvar                      = getCvar(Cvars::motd);
	auto raw_html_motd_cvar             = getCvar(Cvars::raw_html_motd);
	auto use_single_motd_color_cvar     = getCvar(Cvars::use_single_motd_color);
	auto motd_single_color_cvar         = getCvar(Cvars::motd_single_color);
	auto motd_font_size_cvar            = getCvar(Cvars::motd_font_size);
	auto use_gradient_motd_color_cvar   = getCvar(Cvars::use_gradient_motd_color);
	auto motd_gradient_color_begin_cvar = getCvar(Cvars::motd_gradient_color_begin);
	auto motd_gradient_color_end_cvar   = getCvar(Cvars::motd_gradient_color_end);
	if (!enable_motd_cvar)
		return;

	auto use_custom_game_messages_cvar = getCvar(Cvars::use_custom_game_messages);
	auto countdown_msg_3_cvar          = getCvar(Cvars::countdown_msg_3);
	auto countdown_msg_2_cvar          = getCvar(Cvars::countdown_msg_2);
	auto countdown_msg_1_cvar          = getCvar(Cvars::countdown_msg_1);
	auto go_message_cvar               = getCvar(Cvars::go_message);
	auto user_scored_msg_cvar          = getCvar(Cvars::user_scored_msg);
	auto teammate_scored_msg_cvar      = getCvar(Cvars::teammate_scored_msg);
	auto opponent_scored_msg_cvar      = getCvar(Cvars::opponent_scored_msg);
	if (!use_custom_game_messages_cvar)
		return;

	const float motd_height = ImGui::GetContentRegionAvail().y * 0.5f;

	{
		GUI::ScopedChild c{"customMOTD", ImVec2(0, motd_height), true};

		GUI::Spacing(2);

		// enable custom team names checkbox
		bool enableMotD = enable_motd_cvar.getBoolValue();
		if (ImGui::Checkbox("Custom message of the day (MOTD)", &enableMotD))
			enable_motd_cvar.setValue(enableMotD);

		if (enableMotD)
		{
			GUI::Spacing(2);

			bool useSingleMotdColor      = use_single_motd_color_cvar.getBoolValue();
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

			if (ImGui::RadioButton("Custom HTML", &radioState, 0))
			{
				use_single_motd_color_cvar.setValue(false);
				use_gradient_motd_color_cvar.setValue(false);
			}
			if (ImGui::IsItemHovered())
			{
				constexpr const char* tooltip = "Put regular text or use HTML tags to customize the appearance:\n\n"
					"\t\t<font size=\"25\" color=\"#FF0000\">Big red text</font>"
					"\n\nYou can even wrap individual characters in a <font> tag to make colorful designs";

				ImGui::SetTooltip(tooltip);
			}

			if (ImGui::RadioButton("Colored text", &radioState, 1))
			{
				use_single_motd_color_cvar.setValue(true);
				use_gradient_motd_color_cvar.setValue(false);
			}
			if (ImGui::RadioButton("Gradient color text", &radioState, 2))
			{
				use_single_motd_color_cvar.setValue(false);
				use_gradient_motd_color_cvar.setValue(true);
			}

			GUI::Spacing(2);

			std::string motd = Format::UnescapeQuotesHTML(
				radioState == 0 ? raw_html_motd_cvar.getStringValue() : motd_cvar.getStringValue());
			if (ImGui::InputText("Message", &motd))
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
				GAME_THREAD_EXECUTE(Messages.calculateMotdText(); Messages.applyCustomMotd(););
			}

			if (useSingleMotdColor)
			{
				// color picker
				LinearColor motdSingleColor = motd_single_color_cvar.getColorValue() / 255; // converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("Color##motdSingleColor", &motdSingleColor.R, ImGuiColorEditFlags_NoInputs))
				{
					motd_single_color_cvar.setValue(motdSingleColor * 255);
				}

				GUI::SameLineSpacing_absolute(150);
				ImGui::SetNextItemWidth(100);

				int motd_font_size = motd_font_size_cvar.getIntValue();
				if (ImGui::InputInt("Font size", &motd_font_size))
				{
					motd_font_size_cvar.setValue(motd_font_size);
				}
			}
			else if (use_gradient_motd_color)
			{
				// 2 color pickers
				LinearColor motd_gradient_color_begin = motd_gradient_color_begin_cvar.getColorValue() /
					255; // converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("Start##motdGradientColor", &motd_gradient_color_begin.R, ImGuiColorEditFlags_NoInputs))
				{
					motd_gradient_color_begin_cvar.setValue(motd_gradient_color_begin * 255);
				}

				GUI::SameLineSpacing_absolute(150);
				ImGui::SetNextItemWidth(100);

				int motd_font_size = motd_font_size_cvar.getIntValue();
				if (ImGui::InputInt("Font size", &motd_font_size))
				{
					motd_font_size_cvar.setValue(motd_font_size);
				}

				LinearColor motd_gradient_color_end = motd_gradient_color_end_cvar.getColorValue() /
					255; // converts from 0-255 color to 0.0-1.0 color
				if (ImGui::ColorEdit3("End##motdGradientColor", &motd_gradient_color_end.R, ImGuiColorEditFlags_NoInputs))
				{
					motd_gradient_color_end_cvar.setValue(motd_gradient_color_end * 255);
				}
			}
		}
	}

	{
		GUI::ScopedChild c{"gameMessages", ImGui::GetContentRegionAvail(), true};

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
}


class MessagesComponent Messages{};