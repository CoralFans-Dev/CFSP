#include "GuiManager.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"


namespace coral_fans::cfsp::gui {
void GuiManager::sendNewSpPage(Player& player, int defDim, std::string defPos, std::string defName) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm("gui.createSp.title"_tr());
    form.appendInput("name", "gui.createSp.name"_tr(), "", defName);
    form.appendInput("targetpos", "gui.createSp.targetPos"_tr(), "0 0 0", defPos);
    form.appendDropdown(
        "dim",
        "gui.createSp.targetDim"_tr(),
        std::vector<std::string>{
            "base.dimension.overworld"_tr(),
            "base.dimension.nether"_tr(),
            "base.dimension.theend"_tr()
        },
        defDim
    );
    form.sendTo(
        player,
        [this](Player& player, ll::form::CustomFormResult const& elements, ll::form::FormCancelReason cancelReason) {
            if (cancelReason.has_value()) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);

            auto it = elements.value().find("name");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second)) {
                return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);
            }
            std::string name = std::get<std::string>(it->second);

            it = elements.value().find("targetpos");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);
            auto elePos    = std::get<std::string>(it->second);
            auto targetPos = this->tryGetVec3(elePos);

            it = elements.value().find("dim");
            if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second))
                return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);
            auto eleDim = std::get<std::string>(it->second);
            int  dim;
            if (eleDim == "base.dimension.overworld"_tr()) dim = 0;
            else if (eleDim == "base.dimension.nether"_tr()) dim = 1;
            else if (eleDim == "base.dimension.theend"_tr()) dim = 2;
            else return base::OperateResult::error("gui.createSp.paraError"_tr()).sendTo(player);

            if (!targetPos.has_value()) {
                base::OperateResult::error("gui.createSp.nameError"_tr()).sendTo(player);
                return this->sendNewSpPage(player, dim, elePos, name);
            }
            auto res = manager::CFSPManager::getInstance().createSp(&player, name, targetPos.value(), dim);
            res.sendTo(player);
            if (!res) this->sendNewSpPage(player, dim, elePos, name);
        }
    );
}

void GuiManager::sendOperatorSpPage(Player&, std::shared_ptr<simulated_player::SimPlayer>) {}
} // namespace coral_fans::cfsp::gui