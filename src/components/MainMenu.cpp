#include "pch.h"
#include "MainMenu.hpp"
#include "ModUtils/gui/GuiTools.hpp"
#include "components/Instances.hpp"
#include "logging.h"
#include "Cvars.hpp"
#include "Events.hpp"
#include "Macros.hpp"
#include <string_view>

// ##############################################################################################################
// ###############################################    INIT    ###################################################
// ##############################################################################################################

void MainMenuComponent::init(const std::shared_ptr<GameWrapper>& gw)
{
	gameWrapper = gw;

	initCvars();
	initHooks();
	initCommands();
	initMainMenuBgMapIds();

	// wait til cvars are loaded (1s), then update selected_bg_dropdown_index
	DELAY(1.0f, {
		EMainMenuBackground bgId           = static_cast<EMainMenuBackground>(*m_bgIndex);
		std::string         selectedBgName = m_reversedWorkingMMBGIds[bgId];

		for (int i = 0; i < m_bgDropdownNames.size(); ++i)
		{
			if (m_bgDropdownNames[i] == selectedBgName)
			{
				m_selectedBgDropdownIndex = i;
				break;
			}
		}
	});
}

void MainMenuComponent::initHooks()
{
	auto applyCamSettings = [this](std::string eventName)
	{
		LOG("HOOK: {}", eventName);

		if (gameWrapper->IsInFreeplay())
			return;

		applyCustomCamSettings();
	};
	hookEventPost(Events::GFxData_MainMenu_TA_OnEnteredMainMenu, applyCamSettings);
	hookEventPost(Events::GFxData_MainMenu_TA_MainMenuAdded, applyCamSettings);

	hookWithCallerPost(Events::GFxData_MainMenu_TA_OnRotatePreviewFinished,
	    [this](ActorWrapper Caller, ...)
	    {
		    auto* caller = reinterpret_cast<UGFxData_MainMenu_TA*>(Caller.memory_address);
		    if (!validUObject(caller))
			    return;

		    auto* shell = caller->Shell;
		    if (!validUObject(shell))
			    return;

		    if (!shell->Player || !shell->Player->Actor || !shell->Player->Actor->PlayerCamera ||
		        !shell->Player->Actor->PlayerCamera->IsA<ACamera_MainMenu_TA>())
			    return;

		    auto* cam = static_cast<ACamera_MainMenu_TA*>(shell->Player->Actor->PlayerCamera);
		    if (!cam->CurrentState || !cam->CurrentState->IsA<UCameraState_CarPreview_TA>())
			    return;

		    auto*     camState = static_cast<UCameraState_CarPreview_TA*>(cam->CurrentState);
		    FRotator& rot      = camState->RotationOffset;

		    auto mainMenuCamRotationPitch_cvar = getCvar(Cvars::mainMenuCamRotationPitch);
		    auto mainMenuCamRotationYaw_cvar   = getCvar(Cvars::mainMenuCamRotationYaw);
		    if (!mainMenuCamRotationPitch_cvar)
			    return;

		    mainMenuCamRotationPitch_cvar.setValue(rot.Pitch);
		    mainMenuCamRotationYaw_cvar.setValue(rot.Yaw);

		    // write cvar values to file (bc we arent using the auto "writeconfig" when user closes settings menu)
		    _globalCvarManager->executeCommand("writeconfig", false);

		    LOG("Updated camera rotation cvars: Pitch:{} - Yaw:{} - Roll:{}", rot.Pitch, rot.Yaw, rot.Roll);
	    });

	hookWithCallerPost(Events::PremiumGaragePreviewSet_TA_EnterPremiumGarage,
	    [this](ActorWrapper Caller, ...)
	    {
		    auto* premGarage = reinterpret_cast<UPremiumGaragePreviewSet_TA*>(Caller.memory_address);
		    restoreTurntableToPremiumGarage(premGarage);
	    });

	hookWithCallerPost(Events::PremiumGaragePreviewSet_TA_ExitPremiumGarage,
	    [this](ActorWrapper Caller, ...)
	    {
		    auto* premGarage = reinterpret_cast<UPremiumGaragePreviewSet_TA*>(Caller.memory_address);
		    restoreTurntableToMainmenu(premGarage);
	    });
}

void MainMenuComponent::initCvars()
{
	// bools
	registerCvar_bool(Cvars::useCustomMainMenuLocation, false).bindTo(m_useCustomLocation);
	registerCvar_bool(Cvars::preserveMainMenuCameraRotation, true).bindTo(m_preserveCamRotation);
	registerCvar_bool(Cvars::showCarTurntable, true).bindTo(m_showCarTurntable);

	// floats
	registerCvar_number(Cvars::mainMenuX, DEFAULT_CAR_X).bindTo(m_carLocationX);
	registerCvar_number(Cvars::mainMenuY, DEFAULT_CAR_Y).bindTo(m_carLocationY);
	registerCvar_number(Cvars::mainMenuZ, DEFAULT_CAR_Z).bindTo(m_carLocationZ);
	registerCvar_number(Cvars::customFov, DEFAULT_FOV, true, 1, 170).bindTo(m_customFOV);

	// ints
	registerCvar_number(Cvars::mainMenuBgIdx, 26, true, 0, 100).bindTo(m_bgIndex);
	registerCvar_number(Cvars::mainMenuCamRotationPitch, -700).bindTo(m_camRotationPitch);
	registerCvar_number(Cvars::mainMenuCamRotationYaw, 0).bindTo(m_camRotationYaw);
	registerCvar_number(Cvars::carRotationPitch, DEFAULT_CAR_PITCH).bindTo(m_carRotationPitch);
	registerCvar_number(Cvars::carRotationYaw, DEFAULT_CAR_YAW).bindTo(m_carRotationYaw);
	registerCvar_number(Cvars::carRotationRoll, DEFAULT_CAR_ROLL).bindTo(m_carRotationRoll);
}

void MainMenuComponent::initCommands()
{
	registerCommand(Commands::changeMainMenuBg,
	    [this](std::vector<std::string> args)
	    {
		    if (args.size() < 2)
		    {
			    LOG("Usage:\tchange_mainmenu_bg <background_id_num>");
			    return;
		    }

		    int int_id = std::stoi(args[1]);
		    if (int_id < 0 || int_id > 255)
			    return;

		    EMainMenuBackground bg_id = static_cast<EMainMenuBackground>(int_id);
		    setBackground(bg_id, true);

		    DELAY(1.0f, { runCommandInterval(Commands::applyAdTexture, 3, 1.0f, true); });
	    });
}

void MainMenuComponent::initMainMenuBgMapIds()
{
	// save the map/background names in a vector
	for (const auto& [mapName, id] : m_workingMMBGIds)
		m_bgDropdownNames.emplace_back(mapName);

	// update reversed_working_mmbg_ids
	for (const auto& [key, value] : m_workingMMBGIds)
		m_reversedWorkingMMBGIds[value] = key;
}

// ##############################################################################################################
// ###############################################    FUNCTIONS    ##############################################
// ##############################################################################################################

void MainMenuComponent::applyCustomCamSettings()
{
	// custom FOV
	setCameraFOV(*m_customFOV);

	// saved rotation
	if (*m_preserveCamRotation)
		setCameraRotation({*m_camRotationPitch, *m_camRotationYaw, 420});

	// custom location
	if (*m_useCustomLocation)
		setCarLocation({*m_carLocationX, *m_carLocationY, *m_carLocationZ});
}

void MainMenuComponent::setCameraRotation(const FRotator& rot, UCameraState_CarPreview_TA* camState, bool log)
{
	if (!camState)
	{
		auto* lp = Instances.IULocalPlayer();
		if (!lp)
		{
			LOG("[ERROR] ULocalPlayer* is null");
			return;
		}

		auto* pc = lp->Actor;
		if (!pc || !pc->PlayerCamera || !pc->PlayerCamera->IsA<ACamera_MainMenu_TA>())
		{
			LOG("ERROR setting cam rotation 1");
			return;
		}

		auto* cam = static_cast<ACamera_MainMenu_TA*>(pc->PlayerCamera);
		if (!cam->CurrentState || !cam->CurrentState->IsA<UCameraState_CarPreview_TA>())
		{
			LOG("ERROR setting cam rotation 2");
			return;
		}

		camState = static_cast<UCameraState_CarPreview_TA*>(cam->CurrentState);
	}

	camState->RotationOffset.Pitch = rot.Pitch;
	camState->RotationOffset.Yaw   = rot.Yaw;

	if (log)
		LOG("Set camera rotation: Pitch:{} - Yaw:{} - Roll: {}", rot.Pitch, rot.Yaw, rot.Roll);
}

void MainMenuComponent::setCarLocation(const FVector& newLocation, bool log)
{
	// // from voltage source ... slightly modified
	auto* garageGFX = Instances.GetInstanceOf<UGFxData_Garage_TA>();
	if (!validUObject(garageGFX))
		return;

	UCarPreviewSet_TA* previewSet = garageGFX->CarPreviewSet;
	if (!validUObject(previewSet))
		return;
	//
	// ACarPreviewActor_TA* car = previewSet->GetPlayerCarPreviewActor(Instances.IULocalPlayer());
	// if (!validUObject(car))
	// 	return;

	auto* car = Instances.getCarPreviewActor();
	if (!validUObject(car))
	{
		LOGERROR("Unable to get car preview actor");
		return;
	}

	auto* premGarage = Instances.GetInstanceOf<UPremiumGaragePreviewSet_TA>();
	if (!validUObject(premGarage))
		return;

	//// clear any old cars?
	// premGarage->CarPreviewActors.clear();

	// just make them hidden instead, so they aint yeeted and gone when user to goes to premium garage
	LOG("num ACarPreviewActor_TA* in premGarage->CarPreviewActors: {}", premGarage->CarPreviewActors.size());
	for (ACarPreviewActor_TA* car : premGarage->CarPreviewActors)
	{
		if (!validUObject(car))
			continue;
		car->SetHidden(true);
	}

	ATurnTableActor_TA* turntable = premGarage->GetTurntable();
	if (!validUObject(turntable))
		return;

	bool showTuntable = *m_showCarTurntable && shouldShowTurntable(newLocation);
	setTurntableLocation(newLocation, showTuntable, turntable);
	if (showTuntable)
	{
		// bind turntable to the car
		car->SetTurntableActor(turntable, car->Rotation);
		DEBUGLOG("Binded turntable to local player car");

		// turntable is an extra 6 units off the ground compared to car, so raise car up 6 units to compensate & avoid clipping
		FVector newCarLocation = {newLocation.X, newLocation.Y, newLocation.Z + 6};
		car->SetLocation(newCarLocation); // set car location
	}
	else
		car->SetLocation(newLocation); // set car location

	if (log)
		LOG("Set main menu location to X:{} - Y:{} - Z:{}", newLocation.X, newLocation.Y, newLocation.Z);
}

void MainMenuComponent::setTurntableLocation(const FVector& newLocation, bool makeVisible, ATurnTableActor_TA* turntable)
{
	if (!turntable)
	{
		auto* premGarage = Instances.GetInstanceOf<UPremiumGaragePreviewSet_TA>();
		if (!validUObject(premGarage))
			return;
		turntable = premGarage->GetTurntable();
	}

	turntable->SetHidden(!makeVisible);
	turntable->SetLocation(newLocation);
	m_mmTurntableLocation = newLocation; // update state
}

bool MainMenuComponent::shouldShowTurntable(const FVector& location)
{
	// only show turntable if new location is outside the bounds of arena floor:
	// - X coordinate is above 3900 or below -3900
	// - Y coordinate is above 5000 or below -5000
	// - Z coordinate is above 0 or below -5 (ground level is about -1.5 units)
	return location.X < -3900 || location.X > 3900 || location.Y < -5000 || location.Y > 5000 || location.Z < -5 || location.Z > 0;
}

void MainMenuComponent::setRotation(const FRotator& rot)
{
	auto* car = Instances.getCarPreviewActor();
	if (!car)
		return;

	// NOTE: setting rotation like this affects the camera as well, which isn't desirable
	// TODO: maybe eventually set car rotation without affecting the camera
	car->Rotation = rot;

	// update turntable rotation, if turntable exists
	if (validUObject(car->TurntableActor))
		car->TurntableActor->HandleRotationChanged(car->TurntableActor->RotateComponent, rot);
}

void MainMenuComponent::resetLocation(bool log)
{
	setCarLocation(DEFAULT_CAR_LOCATION);

	if (log)
		LOG("Reset main menu location to X:{} - Y:{} - Z:{}", DEFAULT_CAR_LOCATION.X, DEFAULT_CAR_LOCATION.Y, DEFAULT_CAR_LOCATION.Z);
}

void MainMenuComponent::setCameraFOV(float newFOV, bool log)
{
	// works in all game states (main menu, freeplay, in-game)
	auto* pc = Instances.GetInstanceOf<APlayerController>();
	if (!validUObject(pc))
	{
		LOGERROR("APlayerController* is invalid");
		return;
	}

	ACamera* cam = pc->PlayerCamera;
	if (!validUObject(cam))
	{
		LOG("ACamera* (pc->PlayerCamera) is invalid");
		return;
	}

	cam->bLockedFOV = false;
	cam->bEditable  = true;

	cam->DefaultFOV = newFOV;
	cam->LockedFOV  = newFOV;
	cam->SetFOV(newFOV);

	if (log)
		LOG("Set FOV to: {}", newFOV);

	//// only seems to work in main menu and freeplay...
	// ACamera_X* cam = Instances.GetInstanceOf<ACamera_X>();
	// if (!cam) {
	//	LOG("ACamera_X* is null");
	//	return;
	// }

	// cam->bLockedFOV = false;
	// cam->bEditable = true;

	// cam->DefaultFOV = newFOV;
	// cam->LockedFOV = newFOV;
	// cam->SetFOV(newFOV);

	// if (log) {
	//	LOG("Set FOV to: {}", newFOV);
	// }
}

void MainMenuComponent::resetCameraFOV(bool log)
{
	setCameraFOV(DEFAULT_FOV);

	if (log)
		LOG("Reset main menu camera FOV to: {}", DEFAULT_FOV);
}

EMainMenuBackground MainMenuComponent::getSelectedBackground()
{
	EMainMenuBackground bg = EMainMenuBackground::MMBG_END;

	if (m_selectedBgDropdownIndex >= m_bgDropdownNames.size())
	{
		LOGERROR("m_selectedBgDropdownIndex ({}) isn't a valid index for m_bgDropdownNames (size {})",
		    m_selectedBgDropdownIndex,
		    m_bgDropdownNames.size());
		return bg;
	}

	std::string& bgName = m_bgDropdownNames[m_selectedBgDropdownIndex];
	auto         it     = m_workingMMBGIds.find(bgName);
	if (it == m_workingMMBGIds.end())
	{
		LOGERROR("\"{}\" not a valid key for m_workingMMBGIds", bgName);
		return bg;
	}

	return it->second;
}

// only works after main menu has been loaded after a loading screen, not when you first start RL... why
void MainMenuComponent::setBackground(EMainMenuBackground backgroundID, bool log)
{
	if (backgroundID == EMainMenuBackground::MMBG_END)
		return;

	LOG("Setting main menu background map...");

	// voltage method ...
	auto* config = Instances.GetInstanceOf<UUIConfig_TA>();
	if (!validUObject(config))
	{
		LOGERROR("UUIConfig_TA* is null");
		return;
	}

	EMainMenuBackground previousBG = config->MainMenuBG;

	LOG("config->MainMenuBG (previous bg): {}", static_cast<uint8_t>(previousBG));
	LOG("config->CurrentActiveSeason: {}", config->CurrentActiveSeason);

	// config->CurrentActiveSeason = std::stoi(args[2]);		// can also set active season, if we ever wanna do that
	config->MainMenuBG = backgroundID;
	config->Apply();
	config->__MainMenuBG__ChangeNotifyFunc();

	auto* mainMenuSwitched = Instances.GetOrCreateInstance<USeqEvent_MainMenuSwitched_TA>();
	if (!validUObject(mainMenuSwitched))
	{
		LOG("USeqEvent_MainMenuSwitched_TA* from GetOrCreateInstance is null");
		return;
	}

	mainMenuSwitched->PrevBackground = previousBG;
	mainMenuSwitched->HandleMenuBGChange();
	mainMenuSwitched->eventRegisterEvent();
	mainMenuSwitched->__SeqEvent_MainMenuSwitched_TA__RegisterEvent_0x1(config);
	mainMenuSwitched->__SeqEvent_MainMenuSwitched_TA__RegisterEvent_0x2(config);
	mainMenuSwitched->eventToggled();

	if (!m_aMapHasLoaded)
		runCommand(Commands::exitToMainMenu);

	if (log)
		LOG("Set new main menu background... ID: {}", static_cast<uint8_t>(backgroundID));
}

void MainMenuComponent::restoreTurntableToPremiumGarage(UPremiumGaragePreviewSet_TA* premGarage)
{
	if (!validUObject(premGarage))
	{
		premGarage = Instances.GetInstanceOf<UPremiumGaragePreviewSet_TA>();
		if (!validUObject(premGarage))
		{
			LOG("[ERROR] UPremiumGaragePreviewSet_TA* is null");
			return;
		}
	}

	ACarPreviewActor_TA* preview_actor = premGarage->CurrentCar;
	if (!validUObject(preview_actor))
	{
		LOG("[ERROR] ACarPreviewActor_TA* is null");
		return;
	}
	LOG("premium_garage->CurrentCar: {}", preview_actor->GetFullName());

	ATurnTableActor_TA* turntable = premGarage->GetTurntable();
	if (!validUObject(turntable))
	{
		LOG("[ERROR] ATurnTableActor_TA* is null");
		return;
	}

	turntable->SetHidden(false); // unhide turntable if hidden
	turntable->SetLocation(DEFAULT_TURNTABLE_LOCATION);
	LOG("Set turntable location..... X:{} - Y:{} - Z:{}",
	    DEFAULT_TURNTABLE_LOCATION.X,
	    DEFAULT_TURNTABLE_LOCATION.Y,
	    DEFAULT_TURNTABLE_LOCATION.Z);

	// ig the stuff below needs to be executed AFTER the post hook callback. Or maybe just needs a lil delay which gw->Execute provides?
	gameWrapper->Execute(
	    [this, preview_actor](GameWrapper* gw)
	    {
		    preview_actor->SetHidden(false);

		    preview_actor->SetLocation(DEFAULT_PREM_GARAGE_CAR_LOCATION);
		    LOG("(from the callback) Set premium garage car preview actor location..... X:{} - Y:{} - Z:{}",
		        DEFAULT_PREM_GARAGE_CAR_LOCATION.X,
		        DEFAULT_PREM_GARAGE_CAR_LOCATION.Y,
		        DEFAULT_PREM_GARAGE_CAR_LOCATION.Z);

		    preview_actor->ApplyTurntableBase();
		    LOG("AppliedTurntableBase");
	    });
}

void MainMenuComponent::restoreTurntableToMainmenu(UPremiumGaragePreviewSet_TA* premGarage)
{
	if (!validUObject(premGarage))
	{
		premGarage = Instances.GetInstanceOf<UPremiumGaragePreviewSet_TA>();
		if (!validUObject(premGarage))
		{
			LOG("[ERROR] UPremiumGaragePreviewSet_TA* is null");
			return;
		}
	}

	// hide the premium garage cars
	for (ACarPreviewActor_TA* car : premGarage->CarPreviewActors)
	{
		if (!validUObject(car))
			continue;
		car->SetHidden(true);
	}

	ATurnTableActor_TA* turntable = premGarage->GetTurntable();
	if (!validUObject(turntable))
	{
		LOG("[ERROR] ATurnTableActor_TA* is invalid");
		return;
	}

	if (m_mmTurntableLocation.X < -3900 || m_mmTurntableLocation.X > 3900 || m_mmTurntableLocation.Y < -5000 ||
	    m_mmTurntableLocation.Y > 5000 || m_mmTurntableLocation.Z < -5 || m_mmTurntableLocation.Z > 0)
	{
		// set turntable location
		turntable->SetHidden(false); // unhide
		turntable->SetLocation(m_mmTurntableLocation);
	}
	else
	{
		turntable->SetHidden(true); // hide
	}
}

// ##############################################################################################################
// ##########################################    DISPLAY FUNCTIONS    ###########################################
// ##############################################################################################################

void MainMenuComponent::display()
{
	auto useCustomMainMenuLocation_cvar      = getCvar(Cvars::useCustomMainMenuLocation);
	auto showCarTurntable_cvar               = getCvar(Cvars::showCarTurntable);
	auto mainMenuX_cvar                      = getCvar(Cvars::mainMenuX);
	auto mainMenuY_cvar                      = getCvar(Cvars::mainMenuY);
	auto mainMenuZ_cvar                      = getCvar(Cvars::mainMenuZ);
	auto customFOV_cvar                      = getCvar(Cvars::customFov);
	auto preserveMainMenuCameraRotation_cvar = getCvar(Cvars::preserveMainMenuCameraRotation);
	auto carRotationPitch_cvar               = getCvar(Cvars::carRotationPitch);
	auto carRotationYaw_cvar                 = getCvar(Cvars::carRotationYaw);
	auto carRotationRoll_cvar                = getCvar(Cvars::carRotationRoll);
	if (!useCustomMainMenuLocation_cvar)
		return;

	const float mm_location_height = ImGui::GetContentRegionAvail().y * 0.6f;
	{
		GUI::ScopedChild c{"mainMenuLocation", ImVec2(0, mm_location_height), true};

		if (ImGui::CollapsingHeader("Car Location"))
		{
			GUI::ScopedIndent indent{20.0f};

			// enable custom team names checkbox
			bool useCustomMainMenuLoc = useCustomMainMenuLocation_cvar.getBoolValue();
			if (ImGui::Checkbox("Custom location", &useCustomMainMenuLoc))
				useCustomMainMenuLocation_cvar.setValue(useCustomMainMenuLoc);

			if (useCustomMainMenuLoc)
			{
				static constexpr auto NUM_INPUT_WIDTH = 100.0f;

				bool showCarTurntable = showCarTurntable_cvar.getBoolValue();
				if (ImGui::Checkbox("Show turntable", &showCarTurntable))
				{
					showCarTurntable_cvar.setValue(showCarTurntable);
					gameWrapper->Execute(
					    [this](GameWrapper* gw)
					    {
						    setTurntableLocation(m_mmTurntableLocation, *m_showCarTurntable && shouldShowTurntable(m_mmTurntableLocation));
					    });
				}

				GUI::Spacing(2);

				// X
				float mainMenuX = mainMenuX_cvar.getIntValue();
				if (ImGui::SliderFloat("X##sliderX", &mainMenuX, -100000, 100000, "%.0f"))
					mainMenuX_cvar.setValue(mainMenuX);
				ImGui::SameLine();
				ImGui::PushItemWidth(NUM_INPUT_WIDTH);
				if (ImGui::InputFloat("##inputX", &mainMenuX, 10.0f, 100.0f, "%.0f"))
					mainMenuX_cvar.setValue(mainMenuX);
				ImGui::PopItemWidth();

				// Y
				float mainMenuY = mainMenuY_cvar.getIntValue();
				if (ImGui::SliderFloat("Y##sliderY", &mainMenuY, -100000, 100000, "%.0f"))
					mainMenuY_cvar.setValue(mainMenuY);
				ImGui::SameLine();
				ImGui::PushItemWidth(NUM_INPUT_WIDTH);
				if (ImGui::InputFloat("##inputY", &mainMenuY, 10.0f, 100.0f, "%.0f"))
					mainMenuY_cvar.setValue(mainMenuY);
				ImGui::PopItemWidth();

				// Z
				float mainMenuZ = mainMenuZ_cvar.getFloatValue();
				if (ImGui::SliderFloat("Z##sliderZ", &mainMenuZ, -100000, 100000, "%.0f"))
					mainMenuZ_cvar.setValue(mainMenuZ);
				ImGui::SameLine();
				ImGui::PushItemWidth(NUM_INPUT_WIDTH);
				if (ImGui::InputFloat("##inputZ", &mainMenuZ, 10.0f, 100.0f, "%.0f"))
					mainMenuZ_cvar.setValue(mainMenuZ);
				ImGui::PopItemWidth();

				GUI::Spacing(2);

				if (ImGui::Button("Apply##mainMenuLocation"))
				{
					FVector newLocation = {mainMenuX, mainMenuY, mainMenuZ};

					GAME_THREAD_EXECUTE({ setCarLocation(newLocation); }, newLocation);
				}

				GUI::SameLineSpacing_relative(25);

				if (ImGui::Button("Reset##mainMenuLocation"))
				{
					// reset cvar values
					mainMenuX_cvar.setValue(DEFAULT_CAR_X);
					mainMenuY_cvar.setValue(DEFAULT_CAR_Y);
					mainMenuZ_cvar.setValue(DEFAULT_CAR_Z);

					GAME_THREAD_EXECUTE({ resetLocation(true); });
				}
			}

			GUI::Spacing(2);
		}

		static constexpr std::string_view ROTATION_TOOLTIP = "This feature is kinda scuffed rn\n\nRotation affects your camera angle :(";

		if (ImGui::CollapsingHeader("Car Rotation"))
		{
			GUI::ToolTip(ROTATION_TOOLTIP);
			GUI::ScopedIndent indent{20.0f};

			bool shouldApplyRotation = false;

			int carRotationPitch = carRotationPitch_cvar.getIntValue();
			if (ImGui::SliderInt("Pitch", &carRotationPitch, -16364, 16340))
			{
				carRotationPitch_cvar.setValue(carRotationPitch);
				shouldApplyRotation = true;
			}
			GUI::SameLineSpacing_relative(10.0f);
			if (ImGui::Button("Reset##pitch"))
			{
				carRotationPitch_cvar.setValue(DEFAULT_CAR_PITCH);
				GAME_THREAD_EXECUTE({ setRotation({*m_carRotationPitch, *m_carRotationYaw, *m_carRotationRoll}); });
			}

			int carRotationYaw = carRotationYaw_cvar.getIntValue();
			if (ImGui::SliderInt("Yaw", &carRotationYaw, -32768, 32764))
			{
				carRotationYaw_cvar.setValue(carRotationYaw);
				shouldApplyRotation = true;
			}
			GUI::SameLineSpacing_relative(10.0f);
			if (ImGui::Button("Reset##yaw"))
			{
				carRotationYaw_cvar.setValue(DEFAULT_CAR_YAW);
				GAME_THREAD_EXECUTE({ setRotation({*m_carRotationPitch, *m_carRotationYaw, *m_carRotationRoll}); });
			}

			int carRotationRoll = carRotationRoll_cvar.getIntValue();
			if (ImGui::SliderInt("Roll", &carRotationRoll, -32768, 32764))
			{
				carRotationRoll_cvar.setValue(carRotationRoll);
				shouldApplyRotation = true;
			}
			GUI::SameLineSpacing_relative(10.0f);
			if (ImGui::Button("Reset##roll"))
			{
				carRotationRoll_cvar.setValue(DEFAULT_CAR_ROLL);
				GAME_THREAD_EXECUTE({ setRotation({*m_carRotationPitch, *m_carRotationYaw, *m_carRotationRoll}); });
			}

			if (shouldApplyRotation)
				GAME_THREAD_EXECUTE({ setRotation({*m_carRotationPitch, *m_carRotationYaw, *m_carRotationRoll}); });

			GUI::Spacing(2);

			if (ImGui::Button("Reset"))
			{
				carRotationPitch_cvar.setValue(DEFAULT_CAR_PITCH);
				carRotationYaw_cvar.setValue(DEFAULT_CAR_YAW);
				carRotationRoll_cvar.setValue(DEFAULT_CAR_ROLL);
				GAME_THREAD_EXECUTE({ setRotation(DEFAULT_CAR_ROTATION); });
			}
		}
		else
			GUI::ToolTip(ROTATION_TOOLTIP);
	}

	{
		GUI::ScopedChild c{"customFOV", ImGui::GetContentRegionAvail(), true};

		bool remember_mm_camera_rotation = preserveMainMenuCameraRotation_cvar.getBoolValue();
		if (ImGui::Checkbox("Remember camera rotation", &remember_mm_camera_rotation))
			preserveMainMenuCameraRotation_cvar.setValue(remember_mm_camera_rotation);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Rotation will only be saved if you use the mouse to rotate... not a controller stick\n\nbecause Psyonix");

		GUI::Spacing(2);

		int customFOV = customFOV_cvar.getIntValue();
		if (ImGui::SliderInt("FOV", &customFOV, 1, 170))
		{
			customFOV_cvar.setValue(customFOV);

			// TODO: move to a cvar callback function
			GAME_THREAD_EXECUTE({ setCameraFOV(customFOV); }, customFOV);
		}

		GUI::SameLineSpacing_relative(25);

		if (ImGui::Button("Reset##mainMenuFOV"))
		{
			// reset cvar values
			customFOV_cvar.setValue(DEFAULT_FOV);

			GAME_THREAD_EXECUTE({ resetCameraFOV(true); });
		}

		GUI::Spacing(2);

		display_bgDropdown();
	}
}

void MainMenuComponent::display_bgDropdown()
{
	if (m_bgDropdownNames.empty())
	{
		ImGui::TextUnformatted("No main menu backgrounds found....");
		return;
	}

	char               searchBuffer[128] = ""; // Buffer for the search input
	const std::string& selectedBgName    = m_bgDropdownNames[m_selectedBgDropdownIndex];

	if (ImGui::BeginSearchableCombo(
	        "Background##backgroundsDropdown", selectedBgName.c_str(), searchBuffer, sizeof(searchBuffer), "search..."))
	{
		std::string searchQueryLower = Format::ToLower(searchBuffer); // convert search text to lower

		for (int i = 0; i < m_bgDropdownNames.size(); ++i)
		{
			GUI::ScopedID id{i};

			std::string& bgName      = m_bgDropdownNames[i];
			std::string  bgNameLower = Format::ToLower(bgName);

			if (!searchQueryLower.empty()) // filter results if necessary
			{
				if (bgNameLower.find(searchQueryLower) != std::string::npos)
				{
					if (ImGui::Selectable(bgName.c_str(), m_selectedBgDropdownIndex == i))
					{
						m_selectedBgDropdownIndex = i;

						GAME_THREAD_EXECUTE({
							if (!isInMainMenu())
								return;

							setBackground(getSelectedBackground(), true);
							DELAY(1.0f, { runCommandInterval(Commands::applyAdTexture, 3, 1.0f, true); });
						});
					}
				}
			}
			else
			{
				if (ImGui::Selectable(bgName.c_str(), m_selectedBgDropdownIndex == i))
				{
					m_selectedBgDropdownIndex = i;

					GAME_THREAD_EXECUTE({
						if (!isInMainMenu())
							return;

						setBackground(getSelectedBackground(), true);
						DELAY(1.0f, { runCommandInterval(Commands::applyAdTexture, 3, 1.0f, true); });
					});
				}
			}
		}

		ImGui::EndCombo();
	}
}

class MainMenuComponent Mainmenu{};
