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

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> disFloat(0.0, 1.0);
std::uniform_int_distribution<> disInt(0, 1); // experience drop range

DEF_LOGGER("CutCleanOres");