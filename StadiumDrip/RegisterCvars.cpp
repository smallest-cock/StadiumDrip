#include "pch.h"
#include "StadiumDrip.h"


// cvars
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


// commands
void StadiumDrip::RunCommand(const CvarData& command, float delaySeconds)
{
	if (delaySeconds == 0)
	{
		cvarManager->executeCommand(command.name);
	}
	else if (delaySeconds > 0)
	{
		gameWrapper->SetTimeout([this, command](GameWrapper* gw) { cvarManager->executeCommand(command.name); }, delaySeconds);
	}
}

void StadiumDrip::RunCommandInterval(const CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand)
{
	if (!delayFirstCommand)
	{
		RunCommand(command);
		numIntervals--;
	}

	for (int i = 1; i <= numIntervals; i++)
	{
		RunCommand(command, delaySeconds * i);
	}
}

void StadiumDrip::AutoRunCommand(const CvarData& autoRunBool, const CvarData& command, float delaySeconds)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue())
		return;

	RunCommand(command, delaySeconds);
}

void StadiumDrip::AutoRunCommandInterval(
    const CvarData& autoRunBool, const CvarData& command, int numIntervals, float delaySeconds, bool delayFirstCommand)
{
	auto autoRunBool_cvar = GetCvar(autoRunBool);
	if (!autoRunBool_cvar || !autoRunBool_cvar.getBoolValue())
		return;

	RunCommandInterval(command, numIntervals, delaySeconds, delayFirstCommand);
}


// hooks
void StadiumDrip::HookEvent(const char* funcName, std::function<void(std::string)> callback)
{
	gameWrapper->HookEvent(funcName, callback);
	LOG("Hooked function pre: \"{}\"", funcName);
}

void StadiumDrip::HookEventPost(const char* funcName, std::function<void(std::string)> callback)
{
	gameWrapper->HookEventPost(funcName, callback);
	LOG("Hooked function post: \"{}\"", funcName);
}

void StadiumDrip::HookWithCaller(const char* funcName, std::function<void(ActorWrapper, void*, std::string)> callback)
{
	gameWrapper->HookEventWithCaller<ActorWrapper>(funcName, callback);
	LOG("Hooked function pre: \"{}\"", funcName);
}

void StadiumDrip::HookWithCallerPost(const char* funcName, std::function<void(ActorWrapper, void*, std::string)> callback)
{
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(funcName, callback);
	LOG("Hooked function post: \"{}\"", funcName);
}