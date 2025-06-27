#pragma once
#include "Component.hpp"


struct OgTeamColorBackup
{
	std::array<FLinearColor, 3> currentColors; // prolly need only one of these arrays
	std::array<FLinearColor, 3> defaultColors;
	FLinearColor                fontColor;
};

// can be useful if we ever wanna refactor more, so leaving it here
template <typename T>
concept TeamsContainer = requires(T a)
{
	{ a.size() } -> std::convertible_to<std::size_t>;
	{ a[0] } -> std::same_as<ATeam_TA*>;
};


class TeamsComponent : Component<TeamsComponent>
{
public:
	TeamsComponent() {}
	~TeamsComponent() {}

	static constexpr std::string_view componentName = "Teams";
	void Initialize(const std::shared_ptr<GameWrapper>& gw);

private:
	void initCvars();
	void initHooks();
	void initCommands();

private:
	// cvar values
	std::shared_ptr<bool>        m_useCustomTeamColors    = std::make_shared<bool>(true);
	std::shared_ptr<bool>        m_useSingleFreeplayColor = std::make_shared<bool>(false);
	std::shared_ptr<bool>        m_useRGBFreeplayColors   = std::make_shared<bool>(false);
	std::shared_ptr<bool>        m_useCustomTeamNames     = std::make_shared<bool>(true);

	std::shared_ptr<std::string> m_blueName               = std::make_shared<std::string>("crips");
	std::shared_ptr<std::string> m_orangeName             = std::make_shared<std::string>("bloods");

	std::shared_ptr<int>         m_rgbSpeed               = std::make_shared<int>(0);

	std::shared_ptr<LinearColor> m_blueColor              = std::make_shared<LinearColor>(0.0f, 0.0f, 0.0f, 0.0f); // #FF00BC
	std::shared_ptr<LinearColor> m_orangeColor            = std::make_shared<LinearColor>(0.0f, 0.0f, 0.0f, 0.0f); // #FFF800
	std::shared_ptr<LinearColor> m_solidFreeplayColor     = std::make_shared<LinearColor>(0.0f, 0.0f, 0.0f, 0.0f); // #FF2222

	// constants
	static constexpr int   DEFAULT_RGB_SPEED       = 0;
	static constexpr float ITEM_WIDTH              = 200;
	static constexpr float SAMELINE_SPACING        = ITEM_WIDTH / 8; // ~25 px
	static constexpr int   COLOR_ARRAY_SIZE        = 3;

	// flags
	bool m_backedUpOgBlueColor   = false;
	bool m_backedUpOgOrangeColor = false;
	bool m_inUnload              = false;

	// values
	OgTeamColorBackup m_ogBlueColor   = {};
	OgTeamColorBackup m_ogOrangeColor = {};
	std::string       m_ogBlueName;
	std::string       m_ogOrangeName;

private:
	// ####################################### team colors ########################################
	// apply colors
	void applyRgbColor(AGameEvent_Team_TA* event); // to be called every tick when RGB colors are active
	void applySolidFreeplayColor();
	void applyCustomColors(EGameStates state);
	void applyOgColors(EGameStates state);

	void applyColorsToArchetypes(AGameEvent_Team_TA* event = nullptr);
	void applyColorsToArchetype(ATeam_TA* team);

	// live color changing (might be able to be abstracted even further for D R Y)
	void changeColorLiveInMatch(ATeam_TA* team, const FLinearColor& color, bool log = false);
	void changeColorLiveInMatch(ATeam_TA* team, const std::array<FLinearColor, 3>& colors, bool log = false);
	void changeColorLiveOutsideMatch(ATeam_TA* team, const FLinearColor& color, USeqAct_SetStadiumTeamColors_TA* act = nullptr, bool log = false);
	void changeColorLiveOutsideMatch(
	    ATeam_TA* team, const std::array<FLinearColor, 3>& colors, USeqAct_SetStadiumTeamColors_TA* act = nullptr, bool log = false);

	// backup OG colors
	void backupOgColors();
	void backupOgColorsForTeam(const ATeam_TA* team);
	
	// misc
	void tickRGB();
	FLinearColor getCustomColor(int teamNum);


	// ######################################## team names ########################################

	void changeTeamNames(const std::string& blueTeamName = "Blue", const std::string& orangeTeamName = "Orange");
	void changeNamesFromGameEvent(AGameEvent_Team_TA* gameEvent = nullptr);

	static UGFxDataStore_X* getDatastore(AGameEvent_TA* event);
	static UGFxDataStore_X* getDatastore();

public:
	void applyColors();
	void restoreOgColorsInUnload(); // to be called in plugin unload
	void clearOgNames();

public:
	// gui
	void display();
};


extern class TeamsComponent Teams;