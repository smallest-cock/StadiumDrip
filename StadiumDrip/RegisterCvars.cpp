#include "pch.h"
#include "StadiumDrip.h"



CVarWrapper StadiumDrip::RegisterCvar_Bool(const CvarData& cvar, bool startingValue)
{
	std::string value = startingValue ? "1" : "0";

	return cvarManager->registerCvar(cvar.name, value, cvar.description, true, true, 0, true, 1);
}


CVarWrapper StadiumDrip::RegisterCvar_String(const CvarData& cvar, const std::string& startingValue)
{
	return cvarManager->registerCvar(cvar.name, startingValue, cvar.description);
}


CVarWrapper StadiumDrip::RegisterCvar_Number(const CvarData& cvar, float startingValue, bool hasMinMax, float min, float max)
{
	std::string numberStr = std::to_string(startingValue);

	if (hasMinMax)
	{
		return cvarManager->registerCvar(cvar.name, numberStr, cvar.description, true, true, min, true, max);
	}
	else
	{
		return cvarManager->registerCvar(cvar.name, numberStr, cvar.description);
	}
}


CVarWrapper StadiumDrip::RegisterCvar_Color(const CvarData& cvar, const std::string& startingValue)
{
	return cvarManager->registerCvar(cvar.name, startingValue, cvar.description);
}


void StadiumDrip::RegisterCommand(const CvarData& cvar, std::function<void(std::vector<std::string>)> callback)
{
	cvarManager->registerNotifier(cvar.name, callback, cvar.description, PERMISSION_ALL);
}


CVarWrapper StadiumDrip::GetCvar(const CvarData& cvar)
{
	return cvarManager->getCvar(cvar.name);
}

