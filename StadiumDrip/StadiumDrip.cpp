#include "pch.h"
#include "StadiumDrip.h"


BAKKESMOD_PLUGIN(StadiumDrip, "Stadium Drip", plugin_version, PLUGINTYPE_FREEPLAY)
std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void StadiumDrip::onLoad()
{
	_globalCvarManager = cvarManager;

	Instances.InitGlobals(); // initialize RLSDK globals
	if (!Instances.CheckGlobals())
		return;

	pluginInit();

	LOG("Stadium Drip loaded :)");
}

void StadiumDrip::onUnload()
{
	Teams.restoreOgColorsInUnload();
}