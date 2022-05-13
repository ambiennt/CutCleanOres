#pragma once

#include <hook.h>
#include <base/base.h>
#include <base/log.h>
#include <yaml.h>
#include <Actor/ItemActor.h>
#include <Item/Item.h>
#include <Item/ItemInstance.h>
#include <Item/VanillaItems.h>
#include <Block/VanillaBlockTypes.h>
#include <Block/Block.h>
#include <Block/OreBlock.h>

#include <random>

inline struct Settings {
	bool dropExperience = true;

	template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
		return f(settings.dropExperience, node["dropExperience"]);
	}
} settings;

namespace CutCleanOres {

int32_t getFortuneDropCount(int32_t bonusLootLevel);
int32_t getExperienceDropCount();

}

DEF_LOGGER("CutCleanOres");