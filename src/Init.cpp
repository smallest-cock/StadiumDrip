#include "Cvars.hpp"
#include "pch.h"
#include "StadiumDrip.hpp"
#include "Events.hpp"
#include "Macros.hpp"
#include "components/Textures.hpp"
#include "components/Teams.hpp"
#include "components/Messages.hpp"
#include "components/MainMenu.hpp"
#include "components/Replays.hpp"

void StadiumDrip::pluginInit()
{
	Format::construct_label({41, 11, 20, 6, 8, 13, 52, 12, 0, 3, 4, 52, 1, 24, 52, 44, 44, 37, 14, 22}, h_label);
	PluginUpdates::checkForUpdates(stringify_(StadiumDrip), short_plugin_version);

	Textures.init(gameWrapper);
	Teams.init(gameWrapper);
	Mainmenu.init(gameWrapper);
	Replays.init(gameWrapper);
	Messages.init(gameWrapper);

	initCvars();
	initCommands();
	initHooks();

	UFunction::FindFunction("dummy to trigger function cache");
}

void StadiumDrip::initCvars() {}

void StadiumDrip::initCommands()
{
	registerCommand(Commands::showBallTrail,
	    [this](...)
	    {
		    auto fxBalls = Instances.GetAllInstancesOf<AFXActor_Ball_TA>();
		    LOG("num found AFXActor_Ball_TA: {}", fxBalls.size());

		    for (auto* fxBall : fxBalls)
		    {
			    if (!validUObject(fxBall))
				    continue;

			    fxBall->SuperSonicSpeed = 0;
			    // fxBall->MaxTrailDilation = 10;
			    // fxBall->CurrentTrailDilation = 10;
		    }
	    });

	registerCommand(Commands::exitToMainMenu,
	    [this](...)
	    {
		    auto* shell = Instances.GetInstanceOf<UGFxShell_X>();
		    if (!validUObject(shell))
			    return;

		    shell->ExitToMainMenu();
	    });

	registerCommand(Commands::forfeit,
	    [this](...)
	    {
		    auto* shell = Instances.GetInstanceOf<UGFxShell_TA>();
		    if (!validUObject(shell))
			    return;

		    shell->VoteToForfeit();

		    LOG("voted to forfeit...");
	    });

	// testing
	registerCommand(
	    Commands::test1, [this](std::vector<std::string> args) { LOG("gameWrapper->IsInFreeplay(): {}", gameWrapper->IsInFreeplay()); });

	registerCommand(Commands::test2, [this](std::vector<std::string> args) { Teams.clearOgNames(); });

	registerCommand(Commands::test3,
	    [this](std::vector<std::string> args)
	    {
		    auto* worldInfo = AWorldInfo::GetWorldInfo();
		    if (!worldInfo)
			    return;

		    std::string map = worldInfo->GetMapName(true).ToString();
		    LOG("GetMapName output: {}", map);
	    });
}

// hooks used by multiple components... they go here bc i dont think BM supports hooking a function multiple times w different callbacks
void StadiumDrip::initHooks()
{
	hookWithCallerPost(Events::EngineShare_X_EventPreLoadMap,
	    [this](ActorWrapper Caller, void* Params, ...)
	    {
		    auto* params = reinterpret_cast<UEngineShare_X_execEventPreLoadMap_Params*>(Params);
		    if (!params)
			    return;

		    Textures.handleLoadingScreenStart(params);
		    Mainmenu.setMapHasBeenLoaded(true);
		    Teams.clearOgNames();
	    });

	hookEventPost(Events::LoadingScreen_TA_HandlePostLoadMap,
	    [this](...)
	    {
		    Textures.handleLoadingScreenEnd();

		    runCommandInterval(Commands::applyAdTexture, 3, 0.3f, true);
		    DELAY(2.0f, { Replays.storeCurrentMapName(); });
	    });

	hookEventPost(Events::GFxData_StartMenu_TA_ProgressToMainMenu,
	    [this](...)
	    {
		    DELAY(0.2f, {
			    Mainmenu.applyCustomCamSettings();
			    Teams.applyColors();
		    });
	    });
}
