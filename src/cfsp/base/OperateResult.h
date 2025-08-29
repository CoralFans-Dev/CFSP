#pragma once

#include "mc/network/packet/TextPacket.h"
#include "mc/world/actor/player/Player.h"
#include <string>

namespace coral_fans::cfsp::base {
class OperateResult {
public:
    enum class Type : int {
        none    = 0,
        success = 1,
        warn    = 2,
        error   = 3,
    };

public:
    Type        mType = Type::none;
    std::string mInfo = "";

private:
    OperateResult(Type type, std::string info) : mType(type), mInfo(info) {}

public:
    OperateResult() {}
    static OperateResult success(std::string info = "") { return OperateResult(Type::success, info); }
    static OperateResult warn(std::string info = "") { return OperateResult(Type::warn, "§g" + info); }
    static OperateResult error(std::string info = "") { return OperateResult(Type::error, "§4" + info); }

public:
    void sendTo(Player& player) { TextPacket::createRawMessage(mInfo).sendTo(player); }
};
} // namespace coral_fans::cfsp::base