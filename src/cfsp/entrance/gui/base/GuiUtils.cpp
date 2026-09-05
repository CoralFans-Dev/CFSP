#include "GuiUtils.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include "mc/world/phys/HitResult.h"
#include <sstream>
#include <utility>

namespace coral_fans::cfsp::gui::utils {

std::optional<Vec3> tryGetVec3(std::string& str) {
    std::istringstream iss(str);
    float              x, y, z;
    char               test;
    if (!(iss >> x >> y >> z) || iss >> test) return std::nullopt;
    return Vec3(x, y, z);
}

std::optional<int> tryGetInt(std::string& str) {
    std::istringstream iss(str);
    int                i;
    char               test;
    if (!(iss >> i) || iss >> test) return std::nullopt;
    return i;
}

std::optional<float> tryGetFloat(std::string& str) {
    std::istringstream iss(str);
    float              i;
    char               test;
    if (!(iss >> i) || iss >> test) return std::nullopt;
    return i;
}

std::optional<std::string> getStringElement(ll::form::CustomFormResult const& elements, std::string const& key) {
    if (!elements.has_value()) return std::nullopt;
    auto it = elements.value().find(key);
    if (it == elements.value().end() || !std::holds_alternative<std::string>(it->second)) return std::nullopt;
    return std::get<std::string>(it->second);
}

std::optional<bool> getToggleElement(ll::form::CustomFormResult const& elements, std::string const& key) {
    if (!elements.has_value()) return std::nullopt;
    auto it = elements.value().find(key);
    if (it == elements.value().end() || !std::holds_alternative<uint64>(it->second)) return std::nullopt;
    return (bool)std::get<uint64>(it->second);
}

Vec3 getLookOrFeetPos(Player& player) {
    const auto& hit = player.traceRay(5.25f, false, true);
    if (hit) return hit.mPos;
    return player.getFeetPos();
}

std::string posToString(Vec3 const& pos) {
    return std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z);
}

void appendDimDropdown(ll::form::CustomForm& form, int defDim) {
    using ll::i18n_literals::operator""_tr;
    form.appendDropdown(
        "dim",
        "gui.para.targetDim"_tr(),
        std::vector<std::string>{
            "base.dimension.overworld"_tr(),
            "base.dimension.nether"_tr(),
            "base.dimension.theend"_tr()
        },
        defDim
    );
}

std::optional<int> getDimElement(ll::form::CustomFormResult const& elements) {
    using ll::i18n_literals::operator""_tr;
    auto ele = getStringElement(elements, "dim");
    if (!ele.has_value()) return std::nullopt;
    if (ele.value() == "base.dimension.overworld"_tr()) return 0;
    if (ele.value() == "base.dimension.nether"_tr()) return 1;
    if (ele.value() == "base.dimension.theend"_tr()) return 2;
    return std::nullopt;
}

std::optional<bool> getYesNoElement(ll::form::CustomFormResult const& elements, std::string const& key) {
    using ll::i18n_literals::operator""_tr;
    auto ele = getStringElement(elements, key);
    if (!ele.has_value()) return std::nullopt;
    if (ele.value() == "base.yesOrNo.yes"_tr()) return true;
    if (ele.value() == "base.yesOrNo.no"_tr()) return false;
    return std::nullopt;
}

int findOpIndex(std::vector<std::string> const& ops, std::string const& op) {
    int size = (int)ops.size();
    for (int i = 0; i < size; i++)
        if (op == ops[i]) return i;
    return -1;
}

void sendResults(Player& player, std::vector<base::OperateResult> const& results) {
    for (auto res : results) res.sendTo(player);
}

uint calcSpPerm(Player& player, std::shared_ptr<simulated_player::SimPlayer> const& cfsp) {
    if (manager::CFSPManager::getInstance().isManager(&player))
        return manager::CFSPManager::getInstance().getSpPermissionMask();
    return (uint)cfsp->getPermission(&player)
         & manager::CFSPManager::getInstance().getSpPermissionMask(
             std::optional<CommandPermissionLevel>(player.getCommandPermissionLevel())
         );
}

uint calcGroupPerm(Player& player, std::shared_ptr<group::CFSPGroup> const& group) {
    if (manager::CFSPManager::getInstance().isManager(&player))
        return manager::CFSPManager::getInstance().getGroupPermissionMask();
    return (uint)group->getPermission(&player)
         & manager::CFSPManager::getInstance().getGroupPermissionMask(
             std::optional<CommandPermissionLevel>(player.getCommandPermissionLevel())
         );
}

void showConfirmDialog(
    Player&                      player,
    std::string const&           title,
    std::string const&           content,
    std::function<void(Player&)> onYes,
    std::function<void(Player&)> onNo,
    std::function<void(Player&)> onCancel
) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::ModalForm(title, content, "base.yesOrNo.yes"_tr(), "base.yesOrNo.no"_tr());
    form.sendTo(
        player,
        [onYes = std::move(onYes), onNo = std::move(onNo), onCancel = std::move(onCancel)](
            Player&                    player,
            ll::form::ModalFormResult  result,
            ll::form::FormCancelReason cancelReason
        ) {
            if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed) {
                if (onCancel) onCancel(player);
                return;
            }
            if (!result.has_value()) return;
            if (result.value() == ll::form::ModalFormSelectedButton::Upper) {
                if (onYes) onYes(player);
            } else if (onNo) onNo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui::utils
