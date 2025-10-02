#include "pch.h"
#include "Items.hpp"
#include "Messages.hpp"

// ##############################################################################################################
// ###############################################    INIT    ###################################################
// ##############################################################################################################

void ItemsComponent::init(const std::shared_ptr<GameWrapper>& gw)
{
	gameWrapper = gw;

	initFilepaths();
	findAndStoreCarInfo(); // store car body names and their product IDs
}

void ItemsComponent::initFilepaths()
{
	m_pluginFolder = gameWrapper->GetDataFolder() / "StadiumDrip";
	if (!fs::exists(m_pluginFolder))
	{
		fs::create_directory(m_pluginFolder);
		LOG("'{}' didn't exist... so I created it.", m_pluginFolder.string());
	}
}

// ##############################################################################################################
// ###############################################    FUNCTIONS    ##############################################
// ##############################################################################################################

void ItemsComponent::findAndStoreCarInfo()
{
	m_carNames.clear();
	m_carIDs.clear();

	auto* productDb = Instances.GetInstanceOf<UProductDatabase_TA>();
	if (!productDb)
	{
		LOGERROR("UProductDatabase_TA* is invalid");
		return;
	}

	const auto& products = productDb->Products_Pristine;
	for (UProduct_TA* prod : products)
	{
		if (!validUObject(prod) || !validUObject(prod->Slot))
			continue;

		if (prod->Slot->Label != L"Body") // only save car bodies
			continue;

		// car info
		std::string prodLabel = prod->LongLabel.ToString();

		m_carNames.push_back(prodLabel);
		m_carIDs[prodLabel] = prod->GetID();
	}
}

void ItemsComponent::dumpItems()
{
	auto* productDb = Instances.GetInstanceOf<UProductDatabase_TA>();
	if (!productDb)
	{
		LOGERROR("UProductDatabase_TA* is invalid");
		return;
	}

	std::ofstream itemsFile(m_pluginFolder / "item_dump.txt");

	const auto& products = productDb->Products_Pristine;

	// 1st pass: find max label length
	int maxLen = 30;
	for (UProduct_TA* prod : products)
	{
		if (!validUObject(prod) || !validUObject(prod->Slot))
			continue;

		// item info
		const auto& prodID = prod->GetID();

		if (prodID == 0 || prod->Slot->Label == L"null" || prod->LongLabel == L"null")
			continue;

		int labelLen = prod->Slot->Label.size();

		if (labelLen > maxLen)
			maxLen = labelLen;
	}

	// 2nd pass: dump product info
	for (UProduct_TA* prod : products)
	{
		if (!validUObject(prod) || !validUObject(prod->Slot))
			continue;

		// item info
		const auto& prodID = prod->GetID();

		if (prodID == 0 || prod->Slot->Label == L"null" || prod->LongLabel == L"null")
			continue;

		itemsFile << std::left << std::setw(7) << ("[" + std::to_string(prodID) + "]") << " " << std::setw(maxLen)
		          << prod->LongLabel.ToString() << " (" << prod->Slot->Label.ToString() << ")" << "\n";
	}

	itemsFile << std::endl;
	itemsFile.close();

	Messages.spawnNotification("Stadium Drip", "Dumped items to 'item_dump.txt'", 3);
}

UOnlineProduct_TA* ItemsComponent::spawnProduct(
    int item, TArray<FOnlineProductAttribute> attributes, int seriesid, int tradehold, bool log, const std::string& spawnMessage)
{
	auto* utils = reinterpret_cast<UProductUtil_TA*>(UProductUtil_TA::StaticClass());
	if (!validUObject(utils))
		return nullptr;

	FOnlineProductData productData;
	productData.ProductID      = item;
	productData.SeriesID       = seriesid;
	productData.InstanceID     = generatePIID(item);
	productData.TradeHold      = tradehold;
	productData.AddedTimestamp = getTimestampLong();
	productData.Attributes     = attributes;

	UOnlineProduct_TA* product = utils->CreateOnlineProduct(productData);
	if (!validUObject(product))
		return nullptr;

	if (spawnProductData(product->InstanceOnlineProductData(), spawnMessage))
		return product;
	else
		return nullptr;
}

bool ItemsComponent::spawnProductData(FOnlineProductData productData, const std::string& spawnMessage)
{
	auto* containerDrops = Instances.GetInstanceOf<UGFxData_ContainerDrops_TA>();
	if (!containerDrops)
	{
		LOGERROR("UGFxData_ContainerDrops_TA is invalid");
		return false;
	}

	UOnlineProduct_TA* onlineProduct = containerDrops->CreateTempOnlineProduct(productData);
	if (!validUObject(onlineProduct))
	{
		LOGERROR("UOnlineProduct_TA is invalid");
		return false;
	}

	auto* saveData = Instances.GetInstanceOf<USaveData_TA>();
	if (!saveData)
	{
		LOGERROR("USaveData_TA* is invalid");
		return false;
	}

	FString Message = FString::create(spawnMessage);

	saveData->eventGiveOnlineProduct(onlineProduct, Message, 0.0f);
	saveData->GiveOnlineProductHelper(onlineProduct);
	saveData->OnNewOnlineProduct(onlineProduct, Message);
	saveData->EventNewOnlineProduct(saveData, onlineProduct, Message);

	if (validUObject(saveData->OnlineProductSet))
	{
		saveData->OnlineProductSet->Add(onlineProduct);
		auto ProductData = onlineProduct->InstanceOnlineProductData();
	}

	LOG("Successfully spawned product: {}", Format::EscapeBraces(onlineProduct->ToJson().ToString()));
	return true;
}

FProductInstanceID ItemsComponent::generatePIID(int64_t Product)
{
	m_generatedPIIDs++;
	return intToProductInstanceID(getTimestampLong() * Product + m_generatedPIIDs);
}

FProductInstanceID ItemsComponent::intToProductInstanceID(int64_t Value)
{
	FProductInstanceID id{};
	id.UpperBits = static_cast<uint64_t>(Value >> 32);
	id.LowerBits = static_cast<uint64_t>(Value & 0xffffffff);
	return id;
}

uint64_t ItemsComponent::getTimestampLong()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void ItemsComponent::spawnBallInFreeplay() { spawnInFreeplay<ABall_TA>({500, 500, 500}); }

class ItemsComponent Items{};