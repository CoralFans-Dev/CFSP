#include "ComandManager.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/GuiManager.h"
#include "ll/api/command/runtime/RuntimeCommand.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/i18n/I18n.h"
#include "mc/server/commands/CommandOutput.h"


namespace coral_fans::cfsp::command {
void ComandManager::registerGuiCommand() {
    using ll::i18n_literals::operator""_tr;
    if (!manager::CFSPManager::getInstance().getPermissionConfig().gui.enabled) return;

    // sp
    this->command->overload().execute([this](CommandOrigin const& origin, CommandOutput& output) {
        auto player = this->tryGetPlayer(origin);
        if (!player.has_value() || !player.value()) return output.error("command.fail.onlyplayer"_tr());
        if (auto res = manager::CFSPManager::getInstance()
                           .baseCheck(player.value(), manager::CFSPManager::getInstance().getPermissionConfig().gui);
            !res)
            return res.output(output);
        if (manager::CFSPManager::getInstance().isManager(player.value()))
            gui::GuiManager::getInstance().sendManagerMainMenu(*player.value());
        else gui::GuiManager::getInstance().sendMainMenu(*player.value());
    });

    // sp p <name: string>
    this->command->runtimeOverload()
        .text("p")
        .optional("name", ll::command::ParamKind::SoftEnum, "cfspSplist")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value() || !player.value()) return output.error("command.fail.onlyplayer"_tr());
            if (auto res = manager::CFSPManager::getInstance().baseCheck(
                    player.value(),
                    manager::CFSPManager::getInstance().getPermissionConfig().gui
                );
                !res)
                return res.output(output);
            if (!self["name"].has_value()) return gui::GuiManager::getInstance().sendSplist(*player.value());
            auto cfsp =
                manager::CFSPManager::getInstance().tryGetCFSP(self["name"].get<ll::command::ParamKind::SoftEnum>());
            if (!cfsp.has_value()) return output.error("command.fail.spNoFind"_tr());
            if (!cfsp.value()->hasPermission(player.value(), simulated_player::SimPlayerPermission::None))
                return output.error("command.error.permissionDenied"_tr());
            gui::GuiManager::getInstance().sendOperatorSpPage(*player.value(), cfsp.value());
        });

    // sp g <name: string>
    this->command->runtimeOverload()
        .text("g")
        .optional("name", ll::command::ParamKind::SoftEnum, "cfspGroup")
        .execute([this](CommandOrigin const& origin, CommandOutput& output, ll::command::RuntimeCommand const& self) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value() || !player.value()) return output.error("command.fail.onlyplayer"_tr());
            if (auto res = manager::CFSPManager::getInstance().baseCheck(
                    player.value(),
                    manager::CFSPManager::getInstance().getPermissionConfig().gui
                );
                !res)
                return res.output(output);
            if (!self["name"].has_value()) return gui::GuiManager::getInstance().sendGroupList(*player.value());
            auto group =
                manager::CFSPManager::getInstance().tryGetCFSPGroup(self["name"].get<ll::command::ParamKind::SoftEnum>()
                );
            if (!group.has_value()) return output.error("command.fail.groupNoFind"_tr());
            if (!group.value()->hasPermission(player.value(), group::GroupPermission::None))
                return output.error("command.error.permissionDenied"_tr());
            gui::GuiManager::getInstance().sendOperatorGroupPage(*player.value(), group.value());
        });
}
} // namespace coral_fans::cfsp::command