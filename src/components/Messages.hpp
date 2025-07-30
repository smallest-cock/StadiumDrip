#pragma once
#include "Component.hpp"
#include <ModUtils/wrappers/GFxWrapper.hpp>


class MessagesComponent : Component<MessagesComponent>
{
public:
	MessagesComponent() {}
	~MessagesComponent() {}

	static constexpr std::string_view componentName = "Messages";
	void Initialize(const std::shared_ptr<GameWrapper>& gw);

private:
	void initCvars();
	void initHooks();
	void initCommands();

private:
	// constants
	static constexpr auto DEFAULT_SCORE_MSG = "{Player} scored!";

	// cvar values
	std::shared_ptr<bool>		 m_enableMotd             = std::make_shared<bool>(false);
	std::shared_ptr<bool>		 m_useSolidMotdColor      = std::make_shared<bool>(false);
	std::shared_ptr<bool>		 m_useGradienMotdColor    = std::make_shared<bool>(true);
	std::shared_ptr<bool>		 m_useCustomGameMessages  = std::make_shared<bool>(true);
	std::shared_ptr<bool>		 m_unlockAllMenuNodes     = std::make_shared<bool>(false);

	std::shared_ptr<std::string> m_motd                   = std::make_shared<std::string>("sub to Scrimpf on YT <3");
	std::shared_ptr<std::string> m_motdRawHtml            = std::make_shared<std::string>("sub to Scrimpf on YT &lt;3");
	std::shared_ptr<int>         m_motdFontSize           = std::make_shared<int>(20);
	std::shared_ptr<LinearColor> m_motdSolidColor         = std::make_shared<LinearColor>(0.0f, 0.0f, 0.0f, 0.0f); // #FF00FF
	std::shared_ptr<LinearColor> m_motdGradientColorBegin = std::make_shared<LinearColor>(0.0f, 0.0f, 0.0f, 0.0f); // #FFFFFF
	std::shared_ptr<LinearColor> m_motdGradientColorEnd   = std::make_shared<LinearColor>(0.0f, 0.0f, 0.0f, 0.0f); // #FF00E9

	std::shared_ptr<std::string> m_countdownMsg3          = std::make_shared<std::string>("get");
	std::shared_ptr<std::string> m_countdownMsg2          = std::make_shared<std::string>("ready");
	std::shared_ptr<std::string> m_countdownMsg1          = std::make_shared<std::string>("to");
	std::shared_ptr<std::string> m_countdownMsgGo         = std::make_shared<std::string>("cook!");
	std::shared_ptr<std::string> m_userScoredMsg          = std::make_shared<std::string>("i'm that guy pal!");
	std::shared_ptr<std::string> m_teammateScoredMsg      = std::make_shared<std::string>("{Player} just peaked!");
	std::shared_ptr<std::string> m_oppScoredMsg           = std::make_shared<std::string>("{Player} is a tryhard!");

	// values
	std::string m_currentMotdText; // the current/calculated motd text being used

private:
	void setCountdownMessages(AGameEvent_TA* gameEvent, const std::vector<std::string>& countdownMsgs);
	void resetCountdownMessages(AGameEvent_TA* gameEvent);
	void setGoalScoredMessage(APlayerController_TA* pc, const FMessagePacket& msgPacket);
	void resetGoalScoredMessage(UMessage_TA* msg);
	void setMessageText(UMessage_TA* msg, const std::string& newText);
	
	void calculateMotdText();
	std::string getCustomMotdText();
	void applyMotdUsingCommunityGfx(UGFxData_Community_TA* community, const FString& motd_fstr);


	static std::string getGradientMotdString(const std::string& text, int size, const LinearColor& start_col, const LinearColor& end_col);
	static std::vector<std::string> getGradientColors(const std::string& text, const FLinearColor& start_col, const FLinearColor& end_col);

public:
	void applyCustomMotd(UGFxData_Community_TA* community = nullptr, UGFxDataStore_X* datastore = nullptr);
	void spawnNotification(const std::string& title, const std::string& content, float duration, bool log = false);

public:
	// gui
	void display();
};


extern class MessagesComponent Messages;