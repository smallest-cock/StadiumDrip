#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "bakkesmod/plugin/bakkesmodplugin.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_stdlib.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_rangeslider.h"

#include <limits.h>
#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <fstream>
#include <shellapi.h>

#pragma comment(lib, "Shlwapi.lib")

#include "nlohmann.hpp"
#include "SDK/SdkHeaders.hpp"

#include "logging.h"

namespace fs = std::filesystem;
using json = nlohmann::json;