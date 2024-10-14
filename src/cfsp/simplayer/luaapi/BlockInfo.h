#pragma once

#include "cfsp/base/Utils.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/CompoundTagVariant.h"
#include "mc/nbt/Tag.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include <memory>
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace coral_fans::cfsp {

namespace sputils::lua_api {

// userdata blockinfo begin

class BlockInfo {
public:
    std::string                  name;
    std::string                  type;
    int                          id;
    BlockPos                     pos;
    int                          dim;
    int                          variant;
    float                        translucency;
    float                        thickness;
    bool                         isAir;
    bool                         isBounceBlock;
    bool                         isButtonBlock;
    bool                         isCropBlock;
    bool                         isDoorBlock;
    bool                         isFallingBlock;
    bool                         isFenceBlock;
    bool                         isFenceGateBlock;
    bool                         isSlabBlock;
    bool                         isStemBlock;
    bool                         isThinFenceBlock;
    bool                         isUnbreakable;
    bool                         isWaterBlockingBlock;
    std::unique_ptr<CompoundTag> tag;

public:
    BlockInfo(BlockPos const bp, int d, Block const& bl)
    : name(bl.buildDescriptionName()),
      type(utils::removeMinecraftPrefix(bl.getTypeName())),
      id(bl.getBlockItemId()),
      pos(bp),
      dim(d),
      variant(bl.getVariant()),
      translucency(bl.getTranslucency()),
      thickness(bl.getThickness()),
      isAir(bl.isAir()),
      isBounceBlock(bl.isBounceBlock()),
      isButtonBlock(bl.isButtonBlock()),
      isCropBlock(bl.isCropBlock()),
      isDoorBlock(bl.isDoorBlock()),
      isFallingBlock(bl.isFallingBlock()),
      isFenceBlock(bl.isFenceBlock()),
      isFenceGateBlock(bl.isFenceGateBlock()),
      isSlabBlock(bl.isSlabBlock()),
      isStemBlock(bl.isStemBlock()),
      isThinFenceBlock(bl.isThinFenceBlock()),
      isUnbreakable(bl.isUnbreakable()),
      isWaterBlockingBlock(bl.isWaterBlocking()),
      tag(std::make_unique<CompoundTag>(bl.getSerializationId())) {}

    bool operator==(const BlockInfo& bi) const {
        return name == bi.name && type == bi.type && id == bi.id && pos == bi.pos && dim == bi.dim
            && variant == bi.variant && translucency == bi.translucency && thickness == bi.thickness
            && isAir == bi.isAir && isBounceBlock == bi.isBounceBlock && isButtonBlock == bi.isButtonBlock
            && isCropBlock == bi.isCropBlock && isDoorBlock == bi.isDoorBlock && isFallingBlock == bi.isFallingBlock
            && isFenceBlock == bi.isFenceBlock && isFenceGateBlock == bi.isFenceGateBlock
            && isSlabBlock == bi.isSlabBlock && isStemBlock == bi.isStemBlock && isThinFenceBlock == bi.isThinFenceBlock
            && isUnbreakable == bi.isUnbreakable && isWaterBlockingBlock == bi.isWaterBlockingBlock
            && *tag == (*bi.tag);
    }
};

int lua_api_blockinfo_getName(lua_State*);

int lua_api_blockinfo_getType(lua_State*);

int lua_api_blockinfo_getId(lua_State*);

int lua_api_blockinfo_getPos(lua_State*);

int lua_api_blockinfo_getDim(lua_State*);

int lua_api_blockinfo_getVariant(lua_State*);

int lua_api_blockinfo_getTranslucency(lua_State*);

int lua_api_blockinfo_getThickness(lua_State*);

int lua_api_blockinfo_isAir(lua_State*);

int lua_api_blockinfo_isBounceBlock(lua_State*);

int lua_api_blockinfo_isButtonBlock(lua_State*);

int lua_api_blockinfo_isCropBlock(lua_State*);

int lua_api_blockinfo_isDoorBlock(lua_State*);

int lua_api_blockinfo_isFallingBlock(lua_State*);

int lua_api_blockinfo_isFenceBlock(lua_State*);

int lua_api_blockinfo_isFenceGateBlock(lua_State*);

int lua_api_blockinfo_isSlabBlock(lua_State*);

int lua_api_blockinfo_isStemBlock(lua_State*);

int lua_api_blockinfo_isThinFenceBlock(lua_State*);

int lua_api_blockinfo_isUnbreakable(lua_State*);

int lua_api_blockinfo_isWaterBlockingBlock(lua_State*);

int lua_api_blockinfo_getNbtTable(lua_State*);

int lua_api_blockinfo_getSnbt(lua_State*);

int lua_api_blockinfo_getSnbtWithPath(lua_State*);

int lua_api_blockinfo_meta_eq(lua_State*);

int lua_api_blockinfo_meta_gc(lua_State*);

extern const luaL_Reg lua_reg_blockinfo_m[27];

int lua_api_open_blockinfo(lua_State*);

// userdata blockinfo end

} // namespace sputils::lua_api

} // namespace coral_fans::cfsp