#pragma once
#include "Component.hpp"

class ItemsComponent : Component<ItemsComponent>
{
public:
	ItemsComponent() {}
	~ItemsComponent() {}

	static constexpr std::string_view componentName = "Items";
	void                              init(const std::shared_ptr<GameWrapper>& gw);

private:
	void initFilepaths();

private:
	// values
	int      m_generatedPIIDs = 0;
	fs::path m_pluginFolder;

	// for ImGui
	int                      m_currentCarNameIndex = 0;
	std::vector<std::string> m_carNames;

	std::map<std::string, int> m_carIDs;

private:
	// functions
	UOnlineProduct_TA* spawnProduct(int item,
	    TArray<FOnlineProductAttribute> attributes   = {},
	    int                             seriesid     = 0,
	    int                             tradehold    = 0,
	    bool                            log          = false,
	    const std::string&              spawnMessage = "");

	bool               spawnProductData(FOnlineProductData productData, const std::string& spawnMessage);
	FProductInstanceID generatePIID(int64_t Product = -1);
	FProductInstanceID intToProductInstanceID(int64_t Value);
	uint64_t           getTimestampLong();

	void dumpItems();
	void findAndStoreCarInfo();

	void spawnBallInFreeplay();

	template <typename T> void spawnInFreeplay(const FVector& location)
	{
		if (!std::is_base_of<AActor, T>::value)
			return;

		T* thing = Instances.GetInstanceOf<T>();
		if (!validUObject(thing))
			return;

		T* newThing = static_cast<T*>(thing->SpawnInstance(thing, L"", location, thing->Rotation, true));
		// Instances.MarkInvincible(newThing);
	}

public:
	// gui
	void display();
};

extern class ItemsComponent Items;