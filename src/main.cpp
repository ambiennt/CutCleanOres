#include "main.h"
#include <dllentry.h>

void dllenter() {}
void dllexit() {}

DEFAULT_SETTINGS(settings);

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> generateFloat(0.f, 1.f);
static std::uniform_int_distribution<int32_t> generateInt(0, 2); // experience drop range

namespace CutCleanOres {

// https://www.spigotmc.org/threads/fortune.52262/#post-582571
int32_t getFortuneDropCount(int32_t bonusLootLevel) {

	if (bonusLootLevel <= 0) return 1;

	float myRand  = generateFloat(gen);
	int32_t bonus = (int32_t)(myRand * (bonusLootLevel + 2)) - 1;
	return (int32_t)std::max(0, bonus + 1);
}

int32_t getExperienceDropCount() { return generateInt(gen); }

}

/*TInstanceHook(ItemActor*, "?popResource@BlockLegacy@@QEBAPEAVItemActor@@AEAVBlockSource@@AEBVBlockPos@@AEBVItemInstance@@@Z",
	BlockLegacy, void* region, void* pos, ItemInstance const &item) {

	bool isSilkTouch = ((int64_t)_ReturnAddress() - (int64_t)GetModuleHandle(0) == 0x8DCD29); // hack
	if (!isSilkTouch) {
		switch (item.getIdAsEnum()) {
			case ItemRuntimeID::GOLD_ORE:
				return original(this, region, pos, *VanillaItems::mGoldIngot);

			case ItemRuntimeID::IRON_ORE:
				return original(this, region, pos, *VanillaItems::mIronIngot);

			case ItemRuntimeID::ANCIENT_DEBRIS:
				return original(this, region, pos, *VanillaItems::mNetheriteScrap);

			default: break;
		}
	}
	return original(this, region, pos, item);
}*/

TClasslessInstanceHook(ItemInstance, "?getResourceItem@OreBlock@@UEBA?AVItemInstance@@AEAVRandom@@AEBVBlock@@H@Z",
	void* random, Block const& block, int32_t bonusLootLevel) {

	switch (block.mLegacyBlock->getIdAsEnum()) {
		case LegacyBlockID::GOLD_ORE:
			return ItemInstance(*VanillaItems::mGoldIngot);

		case LegacyBlockID::IRON_ORE:
			return ItemInstance(*VanillaItems::mIronIngot);

		case LegacyBlockID::ANCIENT_DEBRIS:
			return ItemInstance(*VanillaItems::mNetheriteScrap);
			
		default: break;
	}
	
	return original(this, random, block, bonusLootLevel);
}

// use RedstoneOreBlock::getResourceCount for redstone
TInstanceHook(int32_t, "?getResourceCount@OreBlock@@UEBAHAEAVRandom@@AEBVBlock@@H_N@Z",
	OreBlock, void* random, void* block, int32_t bonusLootLevel, bool allowRandomness) {

	if (bonusLootLevel > 0) { // if mined with a fortune pickaxe

		auto id = this->getIdAsEnum();
		if ((id == LegacyBlockID::GOLD_ORE) ||
			(id == LegacyBlockID::IRON_ORE) ||
			(id == LegacyBlockID::ANCIENT_DEBRIS)) {
			return allowRandomness ? CutCleanOres::getFortuneDropCount(bonusLootLevel) : 1;
		}
	}
	return original(this, random, block, bonusLootLevel, allowRandomness);
}

TInstanceHook(int32_t, "?getExperienceDrop@OreBlock@@UEBAHAEAVRandom@@@Z", OreBlock, void* random) {

	if (settings.dropExperience) {

		auto id = this->getIdAsEnum();
		if ((id == LegacyBlockID::GOLD_ORE) ||
			(id == LegacyBlockID::IRON_ORE) ||
			(id == LegacyBlockID::ANCIENT_DEBRIS)) {
			return CutCleanOres::getExperienceDropCount();
		}
	}
	return original(this, random);
}