#include "GroupPermPage2.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/GroupPermission.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include <string_view>

namespace coral_fans::cfsp::gui {

namespace {
struct PermItem {
    std::string_view       key;
    std::string_view       i18nKey;
    group::GroupPermission perm;
};

constexpr PermItem GroupPermItems[] = {
    {"addSp",     "gui.perm.addSp",     group::GroupPermission::AddSp    },
    {"rmSp",      "gui.perm.rmSp",      group::GroupPermission::RmSp     },
    {"delete",    "gui.perm.delete",    group::GroupPermission::Delete   },
    {"spawn",     "gui.perm.spawn",     group::GroupPermission::Spawn    },
    {"despawn",   "gui.perm.despawn",   group::GroupPermission::Despawn  },
    {"respawn",   "gui.perm.respawn",   group::GroupPermission::Respawn  },
    {"deleteSp",  "gui.perm.deleteSp",  group::GroupPermission::DeleteSp },
    {"stop",      "gui.perm.stop",      group::GroupPermission::Stop     },
    {"drop",      "gui.perm.drop",      group::GroupPermission::Drop     },
    {"dropInv",   "gui.perm.dropInv",   group::GroupPermission::DropInv  },
    {"sneaking",  "gui.perm.sneaking",  group::GroupPermission::Sneaking },
    {"dwimming",  "gui.perm.swimming",  group::GroupPermission::Swimming },
    {"flying",    "gui.perm.flying",    group::GroupPermission::Flying   },
    {"sprinting", "gui.perm.sprinting", group::GroupPermission::Sprinting},
    {"attack",    "gui.perm.attack",    group::GroupPermission::Attack   },
    {"build",     "gui.perm.build",     group::GroupPermission::Build    },
    {"interact",  "gui.perm.interact",  group::GroupPermission::Interact },
    {"jump",      "gui.perm.jump",      group::GroupPermission::Jump     },
    {"use",       "gui.perm.use",       group::GroupPermission::Use      },
    {"destroy",   "gui.perm.destroy",   group::GroupPermission::Destroy  },
    {"chat",      "gui.perm.chat",      group::GroupPermission::Chat     },
    {"runCmd",    "gui.perm.runCmd",    group::GroupPermission::RunCmd   },
    {"lookAt",    "gui.perm.lookAt",    group::GroupPermission::LookAt   },
    {"moveTo",    "gui.perm.moveTo",    group::GroupPermission::MoveTo   },
    {"navTo",     "gui.perm.navTo",     group::GroupPermission::NavTo    },
    {"tp",        "gui.perm.tp",        group::GroupPermission::Tp       },
    {"select",    "gui.perm.select",    group::GroupPermission::Select   },
};

std::string tr(std::string_view key) { return std::string(ll::i18n::getInstance().get(key, {})); }
} // namespace

void GroupPermPage2::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm(
        "gui.perm.groupTitle2"_tr(
            mGroup->mData.name,
            mTargetPlayerName != "" ? mTargetPlayerName : "gui.perm.publicGroup"_tr()
        )
    );

    uint perm = 0;
    if (mTargetPlayerName.empty()) perm = mGroup->mData.publicPermission;
    else {
        auto it = mGroup->mData.permission.find(mTargetPlayerUuid);
        if (it != mGroup->mData.permission.end()) perm = it->second;
    }

    for (auto const& item : GroupPermItems)
        form.appendToggle(std::string(item.key), tr(item.i18nKey), perm & (uint)item.perm);

    form.sendTo(
        player,
        [this, self = shared_from_this()](
            Player&                           player,
            ll::form::CustomFormResult const& elements,
            ll::form::FormCancelReason        cancelReason
        ) {
            if (this->backOnCancel(player, cancelReason)) return;
            if (!elements.has_value()) return base::OperateResult::error("gui.para.paraError"_tr()).sendTo(player);

            uint newPerm = 0;
            for (auto const& item : GroupPermItems)
                if (auto value = utils::getToggleElement(elements, std::string(item.key));
                    value.has_value() && value.value())
                    newPerm |= (uint)item.perm;

            if (mTargetPlayerUuid.empty()) mGroup->mData.publicPermission = newPerm;
            else if (newPerm) mGroup->mData.permission[mTargetPlayerUuid] = newPerm;
            else if (auto it = mGroup->mData.permission.find(mTargetPlayerUuid); it != mGroup->mData.permission.end())
                mGroup->mData.permission.erase(it);
            mGroup->save();

            base::OperateResult::success("manager.success.operate"_tr()).sendTo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui
