#include "pch.h"
#include "StadiumDrip.h"


void StadiumDrip::pluginInit()
{
	Format::construct_label({41, 11, 20, 6, 8, 13, 52, 12, 0, 3, 4, 52, 1, 24, 52, 44, 44, 37, 14, 22}, h_label);
	PluginUpdates::check_for_updates(stringify_(StadiumDrip), short_plugin_version);

	Textures.Initialize(gameWrapper);
	Teams.Initialize(gameWrapper);
	Mainmenu.Initialize(gameWrapper);
	Replays.Initialize(gameWrapper);
	Messages.Initialize(gameWrapper);

	initCvars();
	initCommands();
	initHooks();

	UFunction::FindFunction("dummy to trigger function cache");
}

// clang-format off
void StadiumDrip::initCvars()
{

}

void StadiumDrip::initCommands()
{
	RegisterCommand(Commands::show_ball_trail, [this](...)
	{
		auto fxBalls = Instances.GetAllInstancesOf<AFXActor_Ball_TA>();
		LOG("num found AFXActor_Ball_TA: {}", fxBalls.size());

		for (auto fxBall : fxBalls)
		{
			if (!validUObject(fxBall))
				continue;

			fxBall->SuperSonicSpeed = 0;
			//fxBall->MaxTrailDilation = 10;
			//fxBall->CurrentTrailDilation = 10;
		}
	});

	RegisterCommand(Commands::exit_to_main_menu, [this](...)
	{
		auto shell = Instances.GetInstanceOf<UGFxShell_X>();
		if (!validUObject(shell))
			return;

		shell->ExitToMainMenu();
	});

	RegisterCommand(Commands::forfeit, [this](...)
	{
		auto shell = Instances.GetInstanceOf<UGFxShell_TA>();
		if (!validUObject(shell))
			return;

		shell->VoteToForfeit();

		LOG("voted to forfeit...");
	});

	// testing
	RegisterCommand(Commands::test_1, [this](std::vector<std::string> args)
	{
		LOG("gameWrapper->IsInFreeplay(): {}", gameWrapper->IsInFreeplay());
	});

	RegisterCommand(Commands::test_2, [this](std::vector<std::string> args)
	{
		Teams.clearOgNames();
	});
	
	RegisterCommand(Commands::test_3, [this](std::vector<std::string> args)
	{
		auto world_info = AWorldInfo::GetWorldInfo();
		if (!world_info)
			return;

		std::string map = world_info->GetMapName(true).ToString();
		LOG("GetMapName output: {}", map);
	});
}

// hooks used by multiple components... they go here bc i dont think BM supports hooking a function multiple times w different callbacks
void StadiumDrip::initHooks()
{
	HookWithCallerPost(Events::EngineShare_X_EventPreLoadMap, [this](ActorWrapper Caller, void* Params, ...)
	{
		auto* params = reinterpret_cast<UEngineShare_X_execEventPreLoadMap_Params*>(Params);
		if (!params)
			return;

		Textures.handleLoadingScreenStart(params);
		Mainmenu.setMapHasBeenLoaded(true);
		Teams.clearOgNames();
	});

	HookEventPost(Events::LoadingScreen_TA_HandlePostLoadMap, [this](...)
	{
		Textures.handleLoadingScreenEnd();

		RunCommandInterval(Commands::apply_ad_texture, 3, 0.3f, true);
		DELAY(2.0f,
			Replays.storeCurrentMapName();
		);
	});

	HookEventPost(Events::GFxData_StartMenu_TA_ProgressToMainMenu,[this](...)
	{
		DELAY(0.2f,
			Mainmenu.applyCustomCamSettings();
			Teams.applyColors();
		);
	});
}
// clang-format on
