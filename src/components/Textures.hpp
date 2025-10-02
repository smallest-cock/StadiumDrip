#pragma once
#include "Component.hpp"

// holds json data from onload
struct ParamCustomizationState
{
	bool             enabled = false;
	Files::ImageInfo img_info;
};

struct MICCustomizationState
{
	bool                                                     enabled = false;
	std::unordered_map<std::string, ParamCustomizationState> param_customizations;

	void fromJson(const json& j);
	json toJson() const;
};

// holds all data necessary for ad changing functionality. for use in imgui, applying texture, etc.
struct MICParamData
{
	int              default_x_size        = 0;
	int              default_y_size        = 0;
	bool             customization_enabled = false;
	Files::ImageInfo custom_img_info;
};

struct MICData
{
	bool                                          customization_enabled = false;
	std::unordered_map<std::string, MICParamData> params;

	json toJson() const;
	void display(const std::string& mic_name, std::shared_ptr<GameWrapper>& gameWrapper);
};

class TexturesComponent : Component<TexturesComponent>
{
public:
	TexturesComponent() {}
	~TexturesComponent() {}

	static constexpr std::string_view componentName = "Textures";
	void                              init(const std::shared_ptr<GameWrapper>& gw);

private:
	void initPaths();
	void initCvars();
	void initHooks();
	void initCommands();

private:
	// cvar values
	std::shared_ptr<bool>        m_useCustomAds     = std::make_shared<bool>(false);
	std::shared_ptr<bool>        m_useSingleAdImage = std::make_shared<bool>(true);
	std::shared_ptr<bool>        m_blockPromoAds    = std::make_shared<bool>(true);
	std::shared_ptr<std::string> m_selectedAdName   = std::make_shared<std::string>("");

	// flags
	bool m_currentlyApplyingTex = false;
	bool m_freeToSaveTextures   = true;
	bool m_setCurrentMapName    = false;

	// values
	fs::path                                            m_acBallTexturesFolder;
	fs::path                                            m_adTexturesFolder;
	fs::path                                            m_adCustomizationsJson;
	std::string                                         m_currentMapName;
	std::map<std::string, Files::ImageInfo>             m_adImgOptionsMap;
	std::unordered_map<std::string, UTexture2DDynamic*> m_cachedAdTextures;
	// std::unordered_map<std::string, TArray<uint8_t>>    savedImgBytes; // can be used when switching to rlsdk method of creating textures

	// used to store loaded json data when plugin first loads, which will be lazily consumed (eventually loaded into m_mapAdMicData)
	std::unordered_map<std::string, std::unordered_map<std::string, MICCustomizationState>> m_onloadJsonData;

	// map name --> MICData (aka the "master" state which stores all necessary data for changing ads)
	std::unordered_map<std::string, std::map<std::string, MICData>> m_mapAdMicData;

private:
	UTexture2DDynamic* imgPathToTexture(const std::string& imagePath);
	UTexture2DDynamic* createAndSaveTexture(const std::string& imgName);
	void               clearUnusedSavedTextures(const std::string& currentAdTexName);
	void               clearSavedAdTextures();
	void               applyTexToAllAds(const std::string& adImgName);

	void loadAdCustomizationsFromJson();

	void updateMicDataFromMesh(UStaticMeshComponent* billboardMesh);
	void getAdDataFromMic(UMaterialInstanceConstant* mic);
	void searchForLeftoverAdMics();

	void applySelectedTexToAllAds();
	void applyAdsToSpecificLocations();

	void applyAds();

public:
	void findAvailableAdImages();
	void writeMicCustomizationsToJson();

	void handleLoadingScreenStart(const UEngineShare_X_execEventPreLoadMap_Params* params);
	void handleLoadingScreenEnd();

public:
	// gui
	void display();
	void display_adTexPicker(CVarWrapper& cvarToUpdate);
	void display_adTexDropdown(MICParamData& param_state, const std::string& param_name);
};

extern class TexturesComponent Textures;
