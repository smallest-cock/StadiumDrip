#include "pch.h"
#include "StadiumDrip.h"



void StadiumDrip::cmd_applyAdTexture(std::vector<std::string> args)
{
	auto use_custom_ads_cvar =			GetCvar(Cvars::use_custom_ads);
	auto use_single_ad_image_cvar =		GetCvar(Cvars::use_single_ad_image);
	if (!use_custom_ads_cvar || !use_custom_ads_cvar.getBoolValue()) return;

	if (use_single_ad_image_cvar.getBoolValue())
		Textures.apply_selected_ad_tex_to_all_ads();
	else
		Textures.apply_ads_to_specific_locations();
}


void StadiumDrip::cmd_change_mainmenu_bg(std::vector<std::string> args)
{
	if (args.size() < 2)
	{
		LOG("Usage:\tchange_mainmenu_bg <background_id_num>");
		return;
	}

	int int_id = std::stoi(args[1]);
	if (int_id < 0 || int_id > 255) return;

	EMainMenuBackground bg_id = static_cast<EMainMenuBackground>(int_id);
	Mainmenu.SetBackground(bg_id, true);

	DELAY(1.0f,
		RunCommandInterval(Commands::apply_ad_texture, 3, 1.0f, true);
	);
}


void StadiumDrip::cmd_apply_motd(std::vector<std::string> args)
{
	auto enable_motd_cvar = GetCvar(Cvars::enable_motd);
	if (!enable_motd_cvar || !enable_motd_cvar.getBoolValue())
		return;

	Messages.apply_custom_motd();
}


void StadiumDrip::cmd_applyTeamColors(std::vector<std::string> args)
{
	ApplyCustomMatchColors();
}


void StadiumDrip::cmd_applyTeamNames(std::vector<std::string> args)
{
	Teams.ChangeNamesFromGameEvent();
}


void StadiumDrip::cmd_showBallTrail(std::vector<std::string> args)
{
	std::vector<AFXActor_Ball_TA*> fxBalls = Instances.GetAllInstancesOf<AFXActor_Ball_TA>();
	LOG("num found AFXActor_Ball_TA: {}", fxBalls.size());

	for (auto fxBall : fxBalls)
	{
		if (!fxBall) continue;

		fxBall->SuperSonicSpeed = 0;
		//fxBall->MaxTrailDilation = 10;
		//fxBall->CurrentTrailDilation = 10;
	}
}


void StadiumDrip::cmd_checkGameState(std::vector<std::string> args)
{
	int state = GetGameState();

	LOG("result of GetGameState(): {}", state);
}


void StadiumDrip::cmd_exitToMainMenu(std::vector<std::string> args)
{
	auto shell = Instances.GetInstanceOf<UGFxShell_X>();
	if (!shell) return;

	shell->ExitToMainMenu();
}


void StadiumDrip::cmd_forfeit(std::vector<std::string> args)
{
	auto shell = Instances.GetInstanceOf<UGFxShell_TA>();
	if (!shell) return;

	shell->VoteToForfeit();

	LOG("voted to forfeit...");
}


void StadiumDrip::cmd_changeReplayMap(std::vector<std::string> args)
{
	if (args.size() < 2) return;

	Replays.ChangeMap(args[1]);
}



// ================================ testing ================================

void StadiumDrip::cmd_test(std::vector<std::string> args)
{
	if (args.size() < 2) return;

	int mapID = std::stoi(args[1]);
	EMainMenuBackground bgID = static_cast<EMainMenuBackground>(mapID);

	Mainmenu.SetBackground(bgID, true);
}


void StadiumDrip::cmd_test2(std::vector<std::string> args)
{
	// ...
}


void StadiumDrip::cmd_test3(std::vector<std::string> args)
{
	auto world_info = AWorldInfo::GetWorldInfo();
	if (!world_info)
		return;

	std::string map = world_info->GetMapName(true).ToString();
	LOG("GetMapName output: {}", map);
}

