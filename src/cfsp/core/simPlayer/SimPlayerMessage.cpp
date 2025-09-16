#include "SimPlayer.h"
#include "cfsp/base/OperateResult.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/deps/core/utility/MCRESULT.h"
#include "mc/server/commands/CommandContext.h"
#include "mc/server/commands/CommandVersion.h"
#include "mc/server/commands/MinecraftCommands.h"
#include "mc/server/commands/PlayerCommandOrigin.h"
#include "mc/world/Minecraft.h"

namespace coral_fans::cfsp::simulated_player {
base::OperateResult SimPlayer::chat(std::string const& messasge) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    this->mSimPlayer->simulateChat(messasge);
    return base::OperateResult::success("manager.success.operate"_tr());
}

base::OperateResult SimPlayer::runcmd(std::string const& cmd) {
    using ll::i18n_literals::operator""_tr;
    if (!this->mSimPlayer) [[unlikely]]
        return base::OperateResult::error("manager.error.loseSimplayer"_tr());
    CommandContext ctx(
        cmd,
        std::make_unique<PlayerCommandOrigin>(PlayerCommandOrigin(*this->mSimPlayer)),
        CommandVersion::CurrentVersion()
    );
    auto mc = ll::service::getMinecraft();
    if (!mc) [[unlikely]]
        return base::OperateResult::error("manager.error.runcmd"_tr());
    auto rst = mc->mCommands->executeCommand(ctx, false);
    if (!rst.mSuccess) return base::OperateResult::error("manager.fail.runcmd"_tr());
    return base::OperateResult::success("manager.success.operate"_tr());
}
} // namespace coral_fans::cfsp::simulated_player