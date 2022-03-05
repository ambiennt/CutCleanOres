#include "main.h"
#include <dllentry.h>

void dllenter() {}
void dllexit() {}

DEFAULT_SETTINGS(settings);

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> generateFloat(0.f, 1.f);
static std::uniform_int_distribution<int32_t> generateInt(0, 2); // experience drop range

// https://www.spigotmc.org/threads/fortune.52262/#post-582571
inline int32_t getFortuneDropCount(int32_t bonusLootLevel) {

	if (bonusLootLevel <= 0) return 1;

	float myRand = generateFloat(gen);
	int32_t bonus = (int32_t)(myRand * (bonusLootLevel + 2)) - 1;
	if (bonus <= 0) bonus = 0;

	return 1 + bonus;
}

TInstanceHook(ItemActor*, "?popResource@BlockLegacy@@QEBAPEAVItemActor@@AEAVBlockSource@@AEBVBlockPos@@AEBVItemInstance@@@Z",
	BlockLegacy, void* region, void* pos, ItemInstance const &item) {

	bool isSilkTouch = ((int64_t)_ReturnAddress() - (int64_t)GetModuleHandle(0) == 0x8DCD29); // hack
	if (!isSilkTouch) {
		switch (item.getId()) {
			case 14:
				return original(this, region, pos, *VanillaItems::mGoldIngot);

			case 15:
				return original(this, region, pos, *VanillaItems::mIronIngot);

			case -271: // 255 + abs(-271) = 526
				return original(this, region, pos, *VanillaItems::mNetheriteScrap);

			default: break;
		}
	}
	return original(this, region, pos, item);
}

// use RedstoneOreBlock::getResourceCount for redstone
TInstanceHook(int32_t, "?getResourceCount@OreBlock@@UEBAHAEAVRandom@@AEBVBlock@@H_N@Z",
	OreBlock, void* random, void* block, int32_t bonusLootLevel, bool allowRandomness) {

	if (bonusLootLevel > 0) { // if mined with a fortune pickaxe

		switch (this->mBlockID) {
			case 14:  // gold ore
			case 15:  // iron ore
			case 526: // ancient debris
				return allowRandomness ? getFortuneDropCount(bonusLootLevel) : 1;

			default: break;
		}
	}
	return original(this, random, block, bonusLootLevel, allowRandomness);
}

TInstanceHook(int32_t, "?getExperienceDrop@OreBlock@@UEBAHAEAVRandom@@@Z", OreBlock, void* random) {

	if (settings.dropExperience) {

		switch (this->mBlockID) {
			case 14:
			case 15:
			case 526:
				return generateInt(gen);

			default: break;
		}
	}
	return original(this, random);
}


/*THook(ItemInstance, "?getResourceItem@OreBlock@@UEBA?AVItemInstance@@AEAVRandom@@AEBVBlock@@H@Z",
	OreBlock* self, ItemInstance *instance, class Random &random, Block const &block, int bonusLootLevel) {

	uint16_t blockId = block.LegacyBlock.mBlockID;
	std::cout << "blockId: " << blockId << std::endl;

	switch (blockId) {
		case 15: {
			std::cout << "mined iron ore\n";
			ItemInstance newItem(*VanillaItems::mIronIngot);
			instance = &newItem;
			return *instance;
		}

		default: break;
	}
	return original(self, instance, random, block, bonusLootLevel);
}*/