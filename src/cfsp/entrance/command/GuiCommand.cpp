#include "ComandManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/GuiManager.h"
#include "ll/api/command/runtime/RuntimeCommand.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/i18n/I18n.h"
#include "mc/server/commands/CommandOutput.h"


namespace coral_fans::cfsp::command {
void ComandManager::registerGuiCommand(ll::command::CommandHandle& command) {
    using ll::i18n_literals::operator""_tr;
    if (!manager::CFSPManager::getInstance().getPermissionConfig().gui.enabled) return;

    // sp
    command.overload().execute([this](CommandOrigin const& origin, CommandOutput& output) {
        auto player = this->tryGetPlayer(origin);
        if (!player.has_value()) return output.error("command.fail.onlyplayer"_tr());
        if (!manager::CFSPManager::getInstance().isAllowed(player.value())
            || player.value()->getCommandPermissionLevel()
                   < manager::CFSPManager::getInstance().getPermissionConfig().gui.permission)
            return output.error("manager.fail.permissionDenied");
        if (!manager::CFSPManager::getInstance().isManager(player.value()))
            gui::GuiManager::getInstance().sendManagerMainMenu(player.value());
        else gui::GuiManager::getInstance().sendMainMenu(player.value());
    });

    // sp p <name: string>
    command.runtimeOverload()
        .text("p")
        .optional("name", ll::command::ParamKind::SoftEnum, "cfspname")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.onlyplayer"_tr());
            if (!self["name"].has_value()) return gui::GuiManager::getInstance().sendSplist(player.value());
            auto cfsp =
                manager::CFSPManager::getInstance().tryGetCFSP(self["name"].get<ll::command::ParamKind::SoftEnum>());
            if (!cfsp.has_value()) return output.error("command.sp.error.spNoFind"_tr());
            if (!manager::CFSPManager::getInstance().isManager(player.value())
                && !cfsp.value()->hasPermission(player.value()))
                return output.error("command.sp.error.permissionDenied"_tr());
            gui::GuiManager::getInstance().sendSpOperatorList(player.value(), cfsp.value());
        });

    // sp g <name: string>
    command.runtimeOverload()
        .text("g")
        .optional("name", ll::command::ParamKind::SoftEnum, "gname")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.onlyplayer"_tr());
            if (!self["name"].has_value()) return gui::GuiManager::getInstance().sendGroupList(player.value());
            auto group =
                manager::CFSPManager::getInstance().tryGetCFSPGroup(self["name"].get<ll::command::ParamKind::SoftEnum>()
                );
            if (!group.has_value()) return output.error("command.sp.error.groupNoFind"_tr());
        });
}
} // namespace coral_fans::cfsp::command