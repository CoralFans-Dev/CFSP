#pragma once

#include "mc/network/packet/TextPacket.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/world/actor/player/Player.h"
#include <string>


namespace coral_fans::cfsp::base {
class OperateResult {
public:
    enum class Type : int {
        Swing   = 0,
        Success = 1,
        Error   = 2,
    };

public:
    Type        mType = Type::Swing;
    std::string mInfo = "";

private:
    OperateResult(Type type, std::string info) : mType(type), mInfo(info) {}


public:
    OperateResult() {}
    static OperateResult success(std::string info = "") { return OperateResult(Type::Success, info); }
    static OperateResult error(std::string info = "") { return OperateResult(Type::Error, info); }
    static OperateResult swing(std::string info = "") { return OperateResult(Type::Swing, info); }
    operator bool() const { return mType != Type::Error; }
    OperateResult(const OperateResult& other) : mType(other.mType), mInfo(other.mInfo) {}
    OperateResult(OperateResult&& other) : mType(std::move(other.mType)), mInfo(std::move(other.mInfo)) {}
    OperateResult& operator=(OperateResult&& other) noexcept {
        if (this != &other) {
            mType = std::move(other.mType);
            mInfo = std::move(other.mInfo);
        }
        return *this;
    }

public:
    void sendTo(Player& player) {
        if (mType == Type::Success) return TextPacket::createRawMessage(mInfo).sendTo(player);
        return TextPacket::createRawMessage("§c" + mInfo).sendTo(player);
    }

    void output(CommandOutput& output) {
        if (mType == Type::Success) return output.success(mInfo);
        output.error(mInfo);
    }
};
} // namespace coral_fans::cfsp::base