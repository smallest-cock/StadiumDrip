#include "pch.h"
#include "StadiumDrip.h"
#include "components/Instances.hpp"
#include "components/Teams.hpp"


BAKKESMOD_PLUGIN(StadiumDrip, "Stadium Drip", plugin_version, PLUGINTYPE_FREEPLAY)
std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void StadiumDrip::onLoad()
{
	_globalCvarManager = cvarManager;

	if (!Instances.InitGlobals())
		return;

	pluginInit();

	LOG("Stadium Drip loaded :)");
}

void StadiumDrip::onUnload()
{
	Teams.restoreOgColorsInUnload();
}