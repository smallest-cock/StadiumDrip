#include "pch.h"
#include "Textures.hpp"
#include "Macros.hpp"
#include "Events.hpp"


using namespace Files;

// ##############################################################################################################
// #####################################    Ad customization state structs    ###################################
// ##############################################################################################################

void MICCustomizationState::fromJson(const json& j)
{
	constexpr const char* ENABLED_KEY = "enabled";
	constexpr const char* PARAMS_KEY  = "paramCustomizations";

	if (!j.contains(ENABLED_KEY) || !j.contains(PARAMS_KEY))
	{
		LOGERROR("JSON for MICCustomizationState is missing \"{}\" or \"{}\" key", PARAMS_KEY, ENABLED_KEY);
		return;
	}

	auto& enabledVal = j.at(ENABLED_KEY);
	auto& paramsVal  = j.at(PARAMS_KEY);

	if (!enabledVal.is_boolean())
	{
		LOG("[ERROR] Invalid \"{}\" value in JSON. It should be a bool", ENABLED_KEY);
		return;
	}

	if (!paramsVal.is_object())
	{
		LOG("[ERROR] Invalid \"{}\" value in JSON. It should be an object", PARAMS_KEY);
		return;
	}

	enabled = enabledVal.get<bool>();

	for (const auto& [paramName, val] : paramsVal.items())
	{
		constexpr const char* IMG_PATH_KEY = "imagePath";

		if (!val.is_object() || !val.contains(IMG_PATH_KEY) || !val.contains(ENABLED_KEY))
			continue;

		auto& enabledVal = val.at(ENABLED_KEY);
		auto& imgPathVal = val.at(IMG_PATH_KEY);

		if (!enabledVal.is_boolean() || !imgPathVal.is_string())
			continue;

		ParamCustomizationState paramCustomization;
		paramCustomization.enabled       = enabledVal.get<bool>();
		paramCustomization.img_info.path = imgPathVal.get<std::string>();
		paramCustomization.img_info.name = paramCustomization.img_info.path.stem().string();

		param_customizations[paramName] = paramCustomization;
	}
}

json MICCustomizationState::toJson() const
{
	json j;

	j["enabled"] = enabled;

	for (const auto& [param_name, param_data] : param_customizations)
	{
		j["paramCustomizations"][param_name]["enabled"]   = param_data.enabled;
		j["paramCustomizations"][param_name]["imagePath"] = param_data.img_info.path.string();
	}

	return j;
}

json MICData::toJson() const
{
	json j;

	j["enabled"] = customization_enabled;

	bool contains_an_img_path = false;
	for (const auto& [param_name, param_data] : params)
	{
		if (param_data.custom_img_info.path.empty())
			continue;

		contains_an_img_path = true;

		j["paramCustomizations"][param_name]["enabled"]   = param_data.customization_enabled;
		j["paramCustomizations"][param_name]["imagePath"] = param_data.custom_img_info.path.string();
	}

	return contains_an_img_path ? j : json();
}

void MICData::display(const std::string& micName, std::shared_ptr<GameWrapper>& gameWrapper)
{
	if (ImGui::Checkbox("Customize", &customization_enabled))
	{
		GAME_THREAD_EXECUTE(Textures.writeMicCustomizationsToJson(););
	}

	GUI::SameLineSpacing_relative(10);

	if (ImGui::CollapsingHeader(micName.c_str()))
	{
		ImGui::Indent(50);

		for (auto& [paramName, paramData] : params)
		{
			GUI::ScopedID id{&paramData};

			if (ImGui::Checkbox("Customize", &paramData.customization_enabled))
			{
				GAME_THREAD_EXECUTE(Textures.writeMicCustomizationsToJson(););
			}

			GUI::SameLineSpacing_relative(10);

			Textures.display_adTexDropdown(paramData, paramName);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Default image size: %ix%i", paramData.default_x_size, paramData.default_y_size);

			GUI::SameLineSpacing_relative(20);

			if (ImGui::Button("Refresh"))
			{
				GAME_THREAD_EXECUTE(Textures.findAvailableAdImages(););
			}
		}

		ImGui::Unindent(50);
	}
}



// ##############################################################################################################
// ###############################################    INIT    ###################################################
// ##############################################################################################################

void TexturesComponent::Initialize(const std::shared_ptr<GameWrapper>& gw)
{
	gameWrapper = gw;

	initPaths();
	findAvailableAdImages();
	loadAdCustomizationsFromJson();
	initCvars();
	initHooks();
	initCommands();
}

void TexturesComponent::initPaths()
{
	auto sd_data_folder = gameWrapper->GetDataFolder() / "StadiumDrip";

	m_adTexturesFolder     = sd_data_folder / "AdTextures";
	m_adCustomizationsJson = sd_data_folder / "ad_customizations.json";

	if (!fs::is_directory(m_adTexturesFolder))
	{
		fs::create_directories(m_adTexturesFolder); // creates sub folders as well. noice
		LOG("\"{}\" didn't exist, so it was created...", m_adTexturesFolder.string());
	}

	if (!fs::is_regular_file(m_adCustomizationsJson))
	{
		// Create the file (if it doesn't exist)
		std::ofstream file(m_adCustomizationsJson); // Opens the file for writing
		if (!file)
		{
			LOGERROR("Failed to create file: \"{}\"", m_adCustomizationsJson.string());
			return;
		}

		file << "{}";
		LOG("\"{}\" didn't exist, so it was created with empty JSON...", m_adCustomizationsJson.string());
	}
}

// clang-format off
void TexturesComponent::initCvars()
{
	// bools
	registerCvar_bool(Cvars::useCustomAds,		false).bindTo(m_useCustomAds);
	registerCvar_bool(Cvars::useSingleAdImage,	true).bindTo(m_useSingleAdImage);
	registerCvar_bool(Cvars::blockPromoAds,		true).bindTo(m_blockPromoAds);

	// strings
	registerCvar_string(Cvars::selectedAdName,	"").bindTo(m_selectedAdName);
}

void TexturesComponent::initHooks()
{
	hookEventPost(Events::SeqAct_MainMenuSwitch_TA_Activated, [this](...)
	{
		runCommand(Commands::apply_ad_texture, 0.5f);
	});

	hookWithCaller(Events::AdManager_TA_AddBillboardMeshRequest, [this](ActorWrapper Caller, void* Params, ...)
	{
		auto params = reinterpret_cast<UAdManager_TA_execAddBillboardMeshRequest_Params*>(Params);
		if (!params || !validUObject(params->Mesh))
			return;

		updateMicDataFromMesh(params->Mesh); // find and store all material/params texture data from billboard mesh

		if (!*m_blockPromoAds)
			return;

		params->Mesh = nullptr; // effectively blocks RL promo ads
	});
}

void TexturesComponent::initCommands()
{
	registerCommand(Commands::apply_ad_texture, [this](...)
	{
		applyAds();
	});
}
// clang-format on



// ##############################################################################################################
// ###############################################    FUNCTIONS    ##############################################
// ##############################################################################################################

void TexturesComponent::handleLoadingScreenStart(const UEngineShare_X_execEventPreLoadMap_Params* params)
{
	if (!params)
		return;

	if (params->MapName == L"MENU_Main_p")
	{
		// TODO: try to get get the acutal map name here instead of MENU_Main_p
		//	- Doing it here might be too early bc this func fires right when the map changes...
		//	  so maybe would have to be done in a later hook?
	}
	else
	{
		m_currentMapName = params->MapName.ToString();

		if (!m_setCurrentMapName)
			m_setCurrentMapName = true;
	}

	LOG("Set current map name: {}", m_currentMapName);
}

void TexturesComponent::handleLoadingScreenEnd()
{
	searchForLeftoverAdMics();

	if (*m_useSingleAdImage)
		applySelectedTexToAllAds();
	else
		applyAdsToSpecificLocations();
}

void TexturesComponent::applyAds()
{
	if (!*m_useCustomAds)
		return;

	if (*m_useSingleAdImage)
		Textures.applySelectedTexToAllAds();
	else
		Textures.applyAdsToSpecificLocations();
}

void TexturesComponent::loadAdCustomizationsFromJson()
{
	json j = Files::get_json(m_adCustomizationsJson);
	if (j.empty())
	{
		LOG("\"{}\" is empty", m_adCustomizationsJson.string());
		return;
	}

	for (const auto& [map_name, mic_customizations] : j.items())
	{
		if (!mic_customizations.is_object())
			continue;

		for (const auto& [mic_name, customization_data] : mic_customizations.items())
		{
			if (!customization_data.is_object())
				continue;

			MICCustomizationState data;
			data.fromJson(customization_data);
			if (data.param_customizations.empty())
				continue;

			m_onloadJsonData[map_name][mic_name] = data;
		}
	}

	LOG("Loaded {} ad customizations from JSON", m_onloadJsonData.size());
}

void TexturesComponent::writeMicCustomizationsToJson()
{
	json j;

	// save data from ad_mic_data
	for (const auto& [map_name, mic_customizations] : m_mapAdMicData)
	{
		for (const auto& [mic_name, mic_data] : mic_customizations)
		{
			json mic_customization_json = mic_data.toJson();
			if (mic_customization_json.empty())
				continue;

			j[map_name][mic_name] = mic_customization_json;
		}
	}

	// save any remaining data from onload_json_data (so it persists)
	for (const auto& [map_name, mic_customizations] : m_onloadJsonData)
	{
		for (const auto& [mic_name, mic_data] : mic_customizations)
		{
			if (j.contains(mic_name)) // prefer data from ad_mic_data over onload_json_data
				continue;

			j[map_name][mic_name] = mic_data.toJson();
		}
	}

	Files::write_json(m_adCustomizationsJson, j);
	LOG("Wrote data to JSON file: \"{}\"", m_adCustomizationsJson.string());
}

void TexturesComponent::searchForLeftoverAdMics()
{
	for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; --i)
	{
		UObject* uObject = UObject::GObjObjects()->at(i);
		if (!validUObject(uObject) || !uObject->IsA<UMaterialInstanceConstant>())
			continue;

		auto mic = static_cast<UMaterialInstanceConstant*>(uObject);
		if (mic->TextureParameterValues.empty())
			continue;

		std::string mic_name = Instances.fullNameWithoutClass(mic);
		DEBUGLOG("This is MIC naem from leftover ad MIC searching shit: \"{}\"", mic_name); // regular LOG clutters console

		auto mic_name_lower = Format::ToLower(mic_name);

		UTexture* dummy = nullptr;
		if (!mic->GetTextureParameterValue(L"AdTexture", dummy) && mic_name_lower.find("billboard") == std::string::npos &&
		    mic_name_lower.find("advert") == std::string::npos)
			continue;

		getAdDataFromMic(mic);
	}
}

void TexturesComponent::updateMicDataFromMesh(UStaticMeshComponent* billboardMesh)
{
	if (!validUObject(billboardMesh))
		return;

	LOG("Billboard mesh: {}", billboardMesh->GetFullName());
	LOG("Billboard mesh materials size: {}", billboardMesh->Materials.size());

	for (UMaterialInterface* mat : billboardMesh->Materials)
	{
		if (!validUObject(mat) || !mat->IsA<UMaterialInstanceConstant>())
		{
			//LOG("UMaterialInterface* mat is invalid or not a UMaterialInstanceConstant");
			continue;
		}

		auto mic = static_cast<UMaterialInstanceConstant*>(mat);
		getAdDataFromMic(mic);
	}
}

void TexturesComponent::getAdDataFromMic(UMaterialInstanceConstant* mic)
{
	if (!validUObject(mic) || mic->TextureParameterValues.empty())
		return;

	if (!m_setCurrentMapName)
		return;

	std::string micName = Instances.fullNameWithoutClass(mic);

	auto mapIt = m_mapAdMicData.find(m_currentMapName);
	if (mapIt != m_mapAdMicData.end() && mapIt->second.contains(micName))
		return;

	// using non-const pointers bc we finna eventually use std::move on the data, which apparently isn't recommended with const
	std::unordered_map<std::string, MICCustomizationState>* micCustomizations      = nullptr;
	MICCustomizationState*                                  jsonCustomizationState = nullptr;

	auto mapJsonDataIt = m_onloadJsonData.find(m_currentMapName);
	if (mapJsonDataIt != m_onloadJsonData.end())
		micCustomizations = &mapJsonDataIt->second;

	if (micCustomizations)
	{
		auto jsonDataIt = micCustomizations->find(micName);
		if (jsonDataIt != micCustomizations->end())
		{
			jsonCustomizationState = &jsonDataIt->second;
		}
	}

	MICData micData;
	if (jsonCustomizationState)
		micData.customization_enabled = jsonCustomizationState->enabled;

	for (const auto& param : mic->TextureParameterValues)
	{
		if (!param.ParameterValue || !(param.ParameterValue->IsA<UTexture2D>() || param.ParameterValue->IsA<UTexture2DDynamic>()))
			continue;

		MICParamData info;
		std::string  paramName = param.ParameterName.ToString();

		if (param.ParameterValue->IsA<UTexture2D>())
		{
			auto tex            = static_cast<UTexture2D*>(param.ParameterValue);
			info.default_x_size = tex->SizeX;
			info.default_y_size = tex->SizeY;
		}
		else if (param.ParameterValue->IsA<UTexture2DDynamic>())
		{
			auto tex            = static_cast<UTexture2DDynamic*>(param.ParameterValue);
			info.default_x_size = tex->SizeX;
			info.default_y_size = tex->SizeY;
		}
		else
			continue;

		if (jsonCustomizationState && jsonCustomizationState->param_customizations.contains(paramName))
		{
			auto& param_customization  = jsonCustomizationState->param_customizations.at(paramName);
			info.customization_enabled = param_customization.enabled;
			info.custom_img_info       = std::move(param_customization.img_info);
		}

		micData.params[paramName] = info;
	}

	m_mapAdMicData[m_currentMapName][micName] = micData;

	if (jsonCustomizationState)
	{
		if (auto it = m_onloadJsonData.find(m_currentMapName); it != m_onloadJsonData.end())
		{
			it->second.erase(micName);
			if (it->second.empty())
				m_onloadJsonData.erase(it);
		}
	}

	LOG("Stored param data for \"{}\"", micName);
}

void TexturesComponent::findAvailableAdImages()
{
	m_adImgOptionsMap.clear();

	Files::FindPngImages(m_adTexturesFolder, m_adImgOptionsMap);
	LOG("{} ad images found", m_adImgOptionsMap.size());
}

/*
// unused atm, but was a neat lil function while testing
void TexturesComponent::fetchBillboardData(bool log)
{
	m_billboardData.clear(); // yeet old data

	UEngine* eng = Instances.IUEngine();
	if (!validUObject(eng) || !validUObject(eng->EngineShare) || !eng->EngineShare->IsA<UEngineShare_TA>())
		return;

	auto engShare = static_cast<UEngineShare_TA*>(eng->EngineShare);
	auto adMan    = engShare->AdManager;
	if (!validUObject(adMan))
		return;

	for (const auto& adLocationData : adMan->BillboardZoneMapping)
		m_billboardData.emplace_back(adLocationData);

	if (log)
	{
		for (const auto& data : m_billboardData)
			LOG("[{}] {}", data.zone_id, data.mat_name);
	}

	LOG("Updated stored billboard data");
}
*/

void TexturesComponent::applySelectedTexToAllAds()
{
	if (m_adImgOptionsMap.empty())
	{
		LOG("No ad images to apply ...");
		return;
	}

	if (!m_adImgOptionsMap.contains(*m_selectedAdName))
	{
		LOGERROR("\"{}\" not found in ad_img_options_map", *m_selectedAdName);
		return;
	}

	LOG("Finna apply \"{}\" ad image...", *m_selectedAdName);

	applyTexToAllAds(*m_selectedAdName);
}

void TexturesComponent::applyTexToAllAds(const std::string& selectedAdName)
{
	m_currentlyApplyingTex = true;

	UTexture* adTex = nullptr;
	if (auto it = m_cachedAdTextures.find(selectedAdName); it != m_cachedAdTextures.end())
		adTex = it->second;
	else
		adTex = createAndSaveTexture(selectedAdName);

	if (!validUObject(adTex))
	{
		LOGERROR("UTexture* for ad is null: {}", selectedAdName);
		return;
	}

	// apply texture
	auto materialsToReplace = Instances.FindAllObjectsThatMatch<UMaterialInstance>({"Advert", "StaticMeshActor"}, {"AdScreen_Goal_"});

	/*
	   Specific to Champions Field: UMaterialInstance's containing "AdScreen_Goal" in their name are the big animated screens
	   behind each goal. The ones containing "AdScreen_04" are the smaller animated screens on top of the arena. They're both unique bc
	   they're animated, which is achieved by zooming into different parts of the image for each "frame"... which is to say it looks scuffed
	   when the texture is replaced with a normal image. So skip applying custom textures to those
	*/
	static const std::vector<std::string> blacklistTerms = {"AdScreen_Goal_", "AdScreen_04"};

	DEBUGLOG("================ MatInterface Names ================");
	for (UMaterialInstance* stadiumAd : materialsToReplace)
	{
		if (!validUObject(stadiumAd))
			continue;

		auto matInterface = stadiumAd->Parent;
		if (validUObject(matInterface))
		{
			const std::string parentName = matInterface->GetFullName();
			LOG("Parent: {}", parentName);
			LOG("matInstance: {}", stadiumAd->GetFullName());

			if (!Format::check_string_using_filters(parentName, {}, blacklistTerms))
			{
				DEBUGLOG("*** SKIPPED ***");
				DEBUGLOG("--------------------------");
				continue;
			}
		}

		// change texture only if it has an "AdTexture" parameter (is an ad)
		UTexture* outTexture = nullptr;
		if (stadiumAd->GetTextureParameterValue(L"AdTexture", outTexture))
			stadiumAd->SetTextureParameterValue(L"AdTexture", adTex);

		DEBUGLOG("--------------------------");
	}
	DEBUGLOG("=============================================");

	m_currentlyApplyingTex = false;
}

void TexturesComponent::applyAdsToSpecificLocations()
{
	m_currentlyApplyingTex = true;

	LOG("Customizing individual stadium ads...");

	for (int32_t i = (UObject::GObjObjects()->size() - INSTANCES_INTERATE_OFFSET); i > 0; --i)
	{
		UObject* uObject = UObject::GObjObjects()->at(i);
		if (!validUObject(uObject) || !uObject->IsA<UMaterialInstanceConstant>())
			continue;

		auto mic = static_cast<UMaterialInstanceConstant*>(uObject);

		std::string full_mic_name = Instances.fullNameWithoutClass(mic);

		auto map_it = m_mapAdMicData.find(m_currentMapName);
		if (map_it == m_mapAdMicData.end())
			continue;

		auto it = map_it->second.find(full_mic_name);
		if (it == map_it->second.end())
			continue;
		// else
		//	LOG("\"{}\" found in ad_mic_data map", full_mic_name);

		const MICData& mic_data = it->second;
		if (!mic_data.customization_enabled)
			continue;

		for (const auto& tex_param : mic->TextureParameterValues)
		{
			std::string param_name = tex_param.ParameterName.ToString();

			auto param_it = mic_data.params.find(param_name);
			if (param_it == mic_data.params.end())
				continue;

			const MICParamData& param_customization_data = param_it->second;
			if (!param_customization_data.customization_enabled)
				continue;

			const auto& custom_img_info = param_customization_data.custom_img_info;

			// apply the texture ...
			UTexture* ad_tex    = nullptr;
			auto      cached_it = m_cachedAdTextures.find(custom_img_info.name);
			if (cached_it != m_cachedAdTextures.end())
				ad_tex = cached_it->second;
			else
				ad_tex = createAndSaveTexture(custom_img_info.name);

			if (!ad_tex)
			{
				LOG("[ERROR] UTexture* for image \"{}\" is null", custom_img_info.name);
				continue;
			}

			mic->SetTextureParameterValue(tex_param.ParameterName, ad_tex);

			LOG("Set \"{}\" texture for {}", param_name, full_mic_name);
		}
	}

	m_currentlyApplyingTex = false;
}

UTexture2DDynamic* TexturesComponent::createAndSaveTexture(const std::string& imgName)
{
	auto it = m_adImgOptionsMap.find(imgName);
	if (it == m_adImgOptionsMap.end())
		return nullptr;

	const auto& img_info = it->second;

	if (!fs::exists(img_info.path))
	{
		LOG("[ERROR] Image doesn't exist: \"{}\"", img_info.path.string());
		return nullptr;
	}

	std::string strPath = img_info.path.string();
	LOG("Attempting to turn \"{}\" into a texture...", strPath);

	auto tex = imgPathToTexture(strPath);
	if (!tex)
	{
		Messages.spawnNotification("stadium drip", "ERROR: Cannot create texture for '" + imgName + "'. Make sure it's a valid PNG", 4);
		LOG("[ERROR] Unable to create texture for '{}'", imgName);
	}
	else
	{
		// save newly created texture
		Instances.MarkInvincible(tex);
		m_cachedAdTextures[imgName] = tex;
	}

	return tex;
}

// TODO: maybe switch to rlsdk method
UTexture2DDynamic* TexturesComponent::imgPathToTexture(const std::string& imagePath)
{
	auto               image     = ImageWrapper(imagePath, true);
	UTexture2DDynamic* customTex = reinterpret_cast<UTexture2DDynamic*>(image.GetCanvasTex());
	if (!validUObject(customTex))
	{
		LOG("UTexture2DDynamic* from ImageWrapper is null!");
		return nullptr;
	}

	return customTex;
}

void TexturesComponent::clearSavedAdTextures()
{
	for (auto& [texName, texture] : m_cachedAdTextures)
		Instances.MarkForDestroy(texture);

	m_cachedAdTextures.clear();
}

void TexturesComponent::clearUnusedSavedTextures(const std::string& currentAdTexName)
{
	UTexture2DDynamic* currentTex = nullptr;

	// mark all unused texture instances to be destroyed by GC
	for (auto& [texName, texture] : m_cachedAdTextures)
	{
		if (!validUObject(texture))
			continue;

		if (texName == currentAdTexName)
		{
			currentTex = texture;
			continue;
		}

		Instances.MarkForDestroy(texture);
	}

	m_cachedAdTextures.clear();
	m_cachedAdTextures[currentAdTexName] = currentTex;
}



// ##############################################################################################################
// ##########################################    DISPLAY FUNCTIONS    ###########################################
// ##############################################################################################################

void TexturesComponent::display()
{
	auto use_custom_ads_cvar      = getCvar(Cvars::useCustomAds);
	auto use_single_ad_image_cvar = getCvar(Cvars::useSingleAdImage);
	auto block_promo_ads_cvar     = getCvar(Cvars::blockPromoAds);
	if (!use_custom_ads_cvar || !use_single_ad_image_cvar)
		return;

	GUI::Spacing(4);

	bool block_promo_ads = block_promo_ads_cvar.getBoolValue();
	if (ImGui::Checkbox("Block promotional ads", &block_promo_ads))
		block_promo_ads_cvar.setValue(block_promo_ads);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("If unchecked, RL promo ads will interfere with custom ads in certain locations. Idk why that would be "
		                  "desirable, but the option exists...");

	bool use_custom_ads = use_custom_ads_cvar.getBoolValue();
	if (ImGui::Checkbox("Use custom ads", &use_custom_ads))
		use_custom_ads_cvar.setValue(use_custom_ads);

	if (use_custom_ads)
	{
		GUI::SameLineSpacing_relative(100);

		// open AdTextures button
		if (ImGui::Button("Open AdTextures folder"))
			Files::OpenFolder(Textures.m_adTexturesFolder);
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
		GAME_THREAD_EXECUTE(runCommand(Commands::apply_ad_texture););
	}

	GUI::Spacing(4);

	if (use_single_ad_image)
	{
		auto selected_ad_name_cvar = getCvar(Cvars::selectedAdName);
		display_adTexPicker(selected_ad_name_cvar);

		ImGui::SameLine();

		// refresh/parse ad textures button
		if (ImGui::Button("Refresh"))
		{
			GAME_THREAD_EXECUTE(findAvailableAdImages(););
		}
	}
	else
	{
		auto it = Textures.m_mapAdMicData.find(Textures.m_currentMapName);
		if (it != Textures.m_mapAdMicData.end())
		{
			auto& micMap = it->second;

			for (auto& [micName, micData] : micMap)
			{
				GUI::ScopedID id{&micData};

				micData.display(micName, gameWrapper);
			}
		}

		GUI::Spacing(4);

		if (ImGui::CollapsingHeader("debug info"))
		{
			// testing
			ImGui::Text("m_onloadJsonData size: %zu", m_onloadJsonData.size());
			ImGui::Text("m_mapAdMicData size: %zu", m_mapAdMicData.size());
			ImGui::Text("m_adImgOptionsMap size: %zu", m_adImgOptionsMap.size());
		}
	}
}

void TexturesComponent::display_adTexPicker(CVarWrapper& cvarToUpdate)
{
	if (cvarToUpdate.IsNull())
	{
		ImGui::TextColored(GUI::Colors::LightRed, "ERROR: Cvar is null");
		return;
	}

	if (m_adImgOptionsMap.empty())
	{
		ImGui::Text("No PNG images found in AdTextures folder ....");
		return;
	}

	char        searchBuffer[128] = ""; // Buffer for the search input
	std::string cvarVal           = cvarToUpdate.getStringValue();
	const char* previewValue       = cvarVal.c_str();

	if (ImGui::BeginSearchableCombo("Ad image##adTextureDropdown", previewValue, searchBuffer, sizeof(searchBuffer), "search..."))
	{
		std::string searchStrLower = Format::ToLower(searchBuffer); // convert search text to lower

		for (const auto& [filename, imgInfo] : Textures.m_adImgOptionsMap)
		{
			GUI::ScopedID id{&imgInfo};

			if (!searchStrLower.empty()) // filter results if necessary
			{
				std::string adNameLower = Format::ToLower(imgInfo.name); // convert ad name text to lower
				if (adNameLower.find(searchStrLower) == std::string::npos)
					continue;

				if (ImGui::Selectable(imgInfo.name.c_str(), imgInfo.name == cvarVal))
				{
					cvarToUpdate.setValue(imgInfo.name); // update cvar string value

					GAME_THREAD_EXECUTE(runCommand(Commands::apply_ad_texture););
				}
			}
			else
			{
				if (ImGui::Selectable(imgInfo.name.c_str(), imgInfo.name == cvarVal))
				{
					cvarToUpdate.setValue(imgInfo.name); // update cvar string value

					GAME_THREAD_EXECUTE(runCommand(Commands::apply_ad_texture););
				}
			}
		}

		ImGui::EndCombo();
	}
}

void TexturesComponent::display_adTexDropdown(MICParamData& param_state, const std::string& param_name)
{
	if (m_adImgOptionsMap.empty())
	{
		ImGui::Text("No PNG images found in AdTextures folder ....");
		return;
	}

	static auto selectable = [this](const Files::ImageInfo& info, MICParamData& paramState)
	{
		if (ImGui::Selectable(info.name.c_str(), info.path == paramState.custom_img_info.path))
		{
			paramState.custom_img_info = info;

			GAME_THREAD_EXECUTE(
				applyAdsToSpecificLocations();
				writeMicCustomizationsToJson();
			);
		}
	};

	char        searchBuffer[128] = ""; // Buffer for the search input
	const char* previewStr        = param_state.custom_img_info.name.c_str();

	if (ImGui::BeginSearchableCombo(param_name.c_str(), previewStr, searchBuffer, sizeof(searchBuffer), "search..."))
	{
		std::string searchStrLower = Format::ToLower(searchBuffer); // convert search text to lower

		for (const auto& [filename, imgInfo] : m_adImgOptionsMap)
		{
			GUI::ScopedID id{&imgInfo};

			// filter results if necessary
			if (!searchStrLower.empty())
			{
				std::string adNameLower = Format::ToLower(imgInfo.name); // convert ad name text to lower
				if (adNameLower.find(searchStrLower) == std::string::npos)
					continue;

				selectable(imgInfo, param_state);
			}
			else
				selectable(imgInfo, param_state);
		}

		ImGui::EndCombo();
	}
}


class TexturesComponent Textures{};