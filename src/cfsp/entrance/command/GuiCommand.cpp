#include "ComandManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "cfsp/entrance/gui/GuiManager.h"
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

    // sp p
    this->command->overload().text("p").execute([this](CommandOrigin const& origin, CommandOutput& output) {
        auto player = this->tryGetPlayer(origin);
        if (!player.has_value() || !player.value()) return output.error("command.fail.onlyplayer"_tr());
        if (auto res = manager::CFSPManager::getInstance()
                           .baseCheck(player.value(), manager::CFSPManager::getInstance().getPermissionConfig().gui);
            !res)
            return res.output(output);
        return gui::GuiManager::getInstance().sendSplist(*player.value());
    });

    // sp g
    this->command->overload().text("g").execute([this](CommandOrigin const& origin, CommandOutput& output) {
        auto player = this->tryGetPlayer(origin);
        if (!player.has_value() || !player.value()) return output.error("command.fail.onlyplayer"_tr());
        if (auto res = manager::CFSPManager::getInstance()
                           .baseCheck(player.value(), manager::CFSPManager::getInstance().getPermissionConfig().gui);
            !res)
            return res.output(output);
        return gui::GuiManager::getInstance().sendGroupList(*player.value());
    });
}
} // namespace coral_fans::cfsp::command