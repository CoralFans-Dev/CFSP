#pragma once

#include "mc/network/packet/TextPacket.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/world/actor/player/Player.h"
#include <string>


namespace coral_fans::cfsp::base {
class OperateResult {
public:
    enum class Type : int {
        none    = 0,
        success = 1,
        error   = 2,
    };

public:
    Type        mType = Type::none;
    std::string mInfo = "";

private:
    OperateResult(Type type, std::string info) : mType(type), mInfo(info) {}

public:
    OperateResult() {}
    static OperateResult success(std::string info = "") { return OperateResult(Type::success, info); }
    static OperateResult error(std::string info = "") { return OperateResult(Type::error, info); }
    explicit             operator bool() const { return mType != Type::error; }

public:
    void sendTo(Player& player) {
        if (mType == Type::success) return TextPacket::createRawMessage(mInfo).sendTo(player);
        if (this->mType == Type::error) return TextPacket::createRawMessage("§c" + mInfo).sendTo(player);
    }

    void output(CommandOutput& output) {
        if (mType == Type::success) return output.success(mInfo);
        if (mType == Type::error) return output.error(mInfo);
    }
};
} // namespace coral_fans::cfsp::base