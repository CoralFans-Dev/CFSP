#include "ComandManager.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/command/runtime/ParamKind.h"
#include "ll/api/command/runtime/RuntimeCommand.h"
#include "ll/api/command/runtime/RuntimeOverload.h"
#include "ll/api/i18n/I18n.h"

namespace coral_fans::cfsp::command {
void ComandManager::registerGroupComand() {
    using ll::i18n_literals::operator""_tr;
    // sp list g
    this->command->overload().text("list").text("g").execute(
        [this](CommandOrigin const& origin, CommandOutput& output) {
            auto player = this->tryGetPlayer(origin);
            if (!player.has_value()) return output.error("command.fail.illegalOrigin"_tr());
            output.success(manager::CFSPManager::getInstance().listGroup(player.value()));
        }
    );
}
} // namespace coral_fans::cfsp::command