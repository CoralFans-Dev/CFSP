#include "cfsp/simplayer/luaapi/Utils.h"
#include "cfsp/base/Macros.h"
#include "mc/nbt/ByteArrayTag.h"
#include "mc/nbt/ByteTag.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/nbt/FloatTag.h"
#include "mc/nbt/IntArrayTag.h"
#include "mc/nbt/IntTag.h"
#include "mc/nbt/ListTag.h"
#include "mc/nbt/ShortTag.h"
#include "mc/nbt/StringTag.h"
#include "mc/nbt/Tag.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include <string>
#include <utility>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

const luaL_Reg lua_reg_null_c[1] = {
    {NULL, NULL}
};

void compoundTagToLuaTable(lua_State* L, CompoundTagVariant tag) {
    if (tag.hold(Tag::Compound)) {
        lua_newtable(L);
        for (const auto& item : tag.get<CompoundTag>()) {
            lua_pushstring(L, item.first.c_str());
            compoundTagToLuaTable(L, item.second);
            lua_settable(L, -3);
        }
        return;
    }
    if (tag.hold(Tag::ByteArray)) {
        int idx = 1;
        lua_newtable(L);
        for (const auto& item : tag.get<ByteArrayTag>()) {
            lua_pushinteger(L, item);
            lua_seti(L, -2, idx);
            ++idx;
        }
        return;
    }
    if (tag.hold(Tag::IntArray)) {
        int idx = 1;
        lua_newtable(L);
        for (const auto& item : tag.get<IntArrayTag>()) {
            lua_pushinteger(L, item);
            lua_seti(L, -2, idx);
            ++idx;
        }
        return;
    }
    if (tag.hold(Tag::List)) {
        int idx = 1;
        lua_newtable(L);
        for (const auto& item : tag.get<ListTag>()) {
            compoundTagToLuaTable(L, item);
            lua_seti(L, -2, idx);
            ++idx;
        }
        return;
    }
    if (tag.hold(Tag::Byte)) {
        lua_pushinteger(L, tag.get<ByteTag>());
        return;
    }
    if (tag.hold(Tag::Double)) {
        lua_pushnumber(L, tag.get<DoubleTag>());
        return;
    }
    if (tag.hold(Tag::Float)) {
        lua_pushnumber(L, tag.get<FloatTag>());
        return;
    }
    if (tag.hold(Tag::Int)) {
        lua_pushinteger(L, tag.get<IntTag>());
        return;
    }
    if (tag.hold(Tag::Int64)) {
        lua_pushinteger(L, tag.get<Int64Tag>());
        return;
    }
    if (tag.hold(Tag::Short)) {
        lua_pushinteger(L, tag.get<ShortTag>());
        return;
    }
    if (tag.hold(Tag::String)) {
        lua_pushstring(L, tag.get<StringTag>().c_str());
        return;
    }
}

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp