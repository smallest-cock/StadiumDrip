#pragma once
#include "Component.hpp"

struct MapName
{
	std::string internalName;
	std::string prettyName;

	MapName(const std::string& rawName, const std::string& prettifiedName) : internalName(rawName), prettyName(prettifiedName) {}
};

class ReplaysComponent : Component<ReplaysComponent>
{
public:
	ReplaysComponent() {}
	~ReplaysComponent() {}

	static constexpr std::string_view componentName = "Replays";
	void                              init(const std::shared_ptr<GameWrapper>& gw);

private:
	void initCvars();
	void initCommands();

private:
	// cvar values
	std::shared_ptr<bool> m_useAltMapSwitchMethod = std::make_shared<bool>(false);

	// values
	std::vector<MapName> m_mapNames;
	std::string          m_currentMapName;
	int                  m_dropdownPreviewIndex = 0;

private:
	void findMapNames();
	void changeMap(const std::string& mapName);

public:
	void storeCurrentMapName();

private:
	// gui
	void display_mapsDropdown();

public:
	// gui
	void display();
};

extern class ReplaysComponent Replays;