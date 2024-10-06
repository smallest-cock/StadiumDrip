#include "pch.h"
#include "StadiumDrip.h"



void StadiumDrip::cmd_applyAdTexture(std::vector<std::string> args)
{
	auto useCustomAds_cvar = GetCvar(Cvars::useCustomAds);
	if (!useCustomAds_cvar || !useCustomAds_cvar.getBoolValue()) return;

	Textures.ApplySelectedAdTexture();
}


void StadiumDrip::cmd_changeMessageOfTheDay(std::vector<std::string> args)
{
	auto enableMotD_cvar = GetCvar(Cvars::enableMotD);
	if (!enableMotD_cvar || !enableMotD_cvar.getBoolValue()) return;

	auto motd_cvar = GetCvar(Cvars::motd);
	if (!motd_cvar) return;

	Messages.ChangeMOTD(motd_cvar.getStringValue(), true);
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
	// ...
}


void StadiumDrip::cmd_test2(std::vector<std::string> args)
{
	// ...
}


void StadiumDrip::cmd_test3(std::vector<std::string> args)
{
	// ...
}

