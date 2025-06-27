#pragma once
#include "Component.hpp"



class MainMenuComponent : Component<MainMenuComponent>
{
public:
	MainMenuComponent() {}
	~MainMenuComponent() {}

	static constexpr std::string_view componentName = "MainMenu";
	void Initialize(const std::shared_ptr<GameWrapper>& gw);

private:
	void initCvars();
	void initHooks();
	void initCommands();
	void initMainMenuBgMapIds();

private:
	// constants
	static constexpr float   DEFAULT_CAR_X                       = -1345.8276f;
	static constexpr float   DEFAULT_CAR_Y                       = -1587.3844f;
	static constexpr float   DEFAULT_CAR_Z                       = 0.0f;
	static constexpr FVector DEFAULT_CAR_LOCATION                = {DEFAULT_CAR_X, DEFAULT_CAR_Y, DEFAULT_CAR_Z};

	static constexpr float   DEFAULT_FOV                         = 55.0f;
	static constexpr FVector default_turntable_location          = {198712.0f, -62.0f, 43.0f};
	static constexpr FVector default_premium_garage_car_location = {198712.0f, -62.0f, 49.0f};

	// cvar values
	std::shared_ptr<bool>  m_useCustomLocation   = std::make_shared<bool>(false);
	std::shared_ptr<bool>  m_rememberCamRotation = std::make_shared<bool>(true);
	std::shared_ptr<float> m_carLocationX        = std::make_shared<float>(DEFAULT_CAR_X);
	std::shared_ptr<float> m_carLocationY        = std::make_shared<float>(DEFAULT_CAR_Y);
	std::shared_ptr<float> m_carLocationZ        = std::make_shared<float>(DEFAULT_CAR_Z);
	std::shared_ptr<float> m_customFOV           = std::make_shared<float>(DEFAULT_FOV);
	std::shared_ptr<int>   m_bgIndex             = std::make_shared<int>(26);
	std::shared_ptr<int>   m_camRotationPitch    = std::make_shared<int>(-700);
	std::shared_ptr<int>   m_camRotationYaw      = std::make_shared<int>(0);
	
	// flags
	bool m_aMapHasLoaded = false; // idk how to determine this, but can be useful for changing bg map when 1st started RL

	// values
	std::map<std::string, EMainMenuBackground> m_workingMMBGIds =
	{
		{ "Mannfield (Dusk)",					EMainMenuBackground::MMBG_EuroDusk },
		{ "AquaDome (Salty Shallows)",			EMainMenuBackground::MMBG_AquaGrass },
		{ "Salty Shores (Salty Fest)",			EMainMenuBackground::MMBG_BeachNightGrass },
		{ "Drift Woods",						EMainMenuBackground::MMBG_Woods },
		{ "Drift Woods (Night)",				EMainMenuBackground::MMBG_WoodsNight },
		{ "Neo Tokyo (Arcade)",					EMainMenuBackground::MMBG_TokyoArcade },
		{ "Futura Garden",						EMainMenuBackground::MMBG_FutureUtopia },
	    { "DFH Stadium (10th Anniversary)",		EMainMenuBackground::MMBG_Anniversary }
	};
	std::map<EMainMenuBackground, std::string> m_reversedWorkingMMBGIds;
	FVector                                    m_mmTurntableLocation = DEFAULT_CAR_LOCATION;

	// for ImGui
	std::vector<std::string> m_bgDropdownNames;
	int m_selectedBgDropdownIndex = 0; 

private:
	void setBackground(EMainMenuBackground backgroundID, bool log = false);
	void setLocation(const FVector& newLocation, bool log = false);
	void resetLocation(bool log = false);
	void setCameraFOV(float newFOV, bool log = false);
	void resetCameraFOV(bool log = false);
	void setCameraRotation(const FRotator& rot, UCameraState_CarPreview_TA* camState = nullptr, bool log = true);
	EMainMenuBackground getSelectedBackground();

	void restoreTurntableToPremiumGarage(UPremiumGaragePreviewSet_TA* premiumGarage = nullptr);
	void restoreTurntableToMainmenu(UPremiumGaragePreviewSet_TA* premiumGarage = nullptr);

public:
	void applyCustomCamSettings();
	void setMapHasBeenLoaded(bool val) { m_aMapHasLoaded = val; }

private:
	// gui
	void display_bgDropdown();

public:
	// gui
	void display();
};


extern class MainMenuComponent Mainmenu;