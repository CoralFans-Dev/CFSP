#include "SpPermPage2.h"
#include "cfsp/base/OperateResult.h"
#include "cfsp/core/simPlayer/SimPlayerPermission.h"
#include "cfsp/entrance/gui/base/GuiUtils.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/i18n/I18n.h"
#include <string_view>

namespace coral_fans::cfsp::gui {

namespace {
struct PermItem {
    std::string_view                      key;
    std::string_view                      i18nKey;
    simulated_player::SimPlayerPermission perm;
};

constexpr PermItem SpPermItems[] = {
    {"spawn",          "gui.perm.spawn",          simulated_player::SimPlayerPermission::Spawn         },
    {"despawn",        "gui.perm.despawn",        simulated_player::SimPlayerPermission::Despawn       },
    {"respawn",        "gui.perm.respawn",        simulated_player::SimPlayerPermission::Respawn       },
    {"delete",         "gui.perm.delete",         simulated_player::SimPlayerPermission::Delete        },
    {"stop",           "gui.perm.stop",           simulated_player::SimPlayerPermission::Stop          },
    {"drop",           "gui.perm.drop",           simulated_player::SimPlayerPermission::Drop          },
    {"dropInv",        "gui.perm.dropInv",        simulated_player::SimPlayerPermission::DropInv       },
    {"swap",           "gui.perm.swap",           simulated_player::SimPlayerPermission::Swap          },
    {"sneaking",       "gui.perm.sneaking",       simulated_player::SimPlayerPermission::Sneaking      },
    {"swimming",       "gui.perm.swimming",       simulated_player::SimPlayerPermission::Swimming      },
    {"flying",         "gui.perm.flying",         simulated_player::SimPlayerPermission::Flying        },
    {"sprinting",      "gui.perm.sprinting",      simulated_player::SimPlayerPermission::Sprinting     },
    {"attack",         "gui.perm.attack",         simulated_player::SimPlayerPermission::Attack        },
    {"build",          "gui.perm.build",          simulated_player::SimPlayerPermission::Build         },
    {"interact",       "gui.perm.interact",       simulated_player::SimPlayerPermission::Interact      },
    {"jump",           "gui.perm.jump",           simulated_player::SimPlayerPermission::Jump          },
    {"use",            "gui.perm.use",            simulated_player::SimPlayerPermission::Use           },
    {"destroy",        "gui.perm.destroy",        simulated_player::SimPlayerPermission::Destroy       },
    {"chat",           "gui.perm.chat",           simulated_player::SimPlayerPermission::Chat          },
    {"runCmd",         "gui.perm.runCmd",         simulated_player::SimPlayerPermission::RunCmd        },
    {"select",         "gui.perm.select",         simulated_player::SimPlayerPermission::Select        },
    {"lookAt",         "gui.perm.lookAt",         simulated_player::SimPlayerPermission::LookAt        },
    {"moveTo",         "gui.perm.moveTo",         simulated_player::SimPlayerPermission::MoveTo        },
    {"navTo",          "gui.perm.navTo",          simulated_player::SimPlayerPermission::NavTo         },
    {"tp",             "gui.perm.tp",             simulated_player::SimPlayerPermission::Tp            },
    {"beAddedToGroup", "gui.perm.beAddedToGroup", simulated_player::SimPlayerPermission::BeAddedToGroup},
};

std::string tr(std::string_view key) { return std::string(ll::i18n::getInstance().get(key, {})); }
} // namespace

void SpPermPage2::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    auto form = ll::form::CustomForm(
        "gui.perm.spTitle2"_tr(
            mCfsp->mSaveData.name,
            mTargetPlayerName != "" ? mTargetPlayerName : "gui.perm.publicSp"_tr()
        )
    );

    uint perm = 0;
    if (mTargetPlayerName.empty()) perm = mCfsp->mSaveData.publicPermission;
    else {
        auto it = mCfsp->mSaveData.permission.find(mTargetPlayerUuid);
        if (it != mCfsp->mSaveData.permission.end()) perm = it->second;
    }

    for (auto const& item : SpPermItems)
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
            for (auto const& item : SpPermItems)
                if (auto value = utils::getToggleElement(elements, std::string(item.key));
                    value.has_value() && value.value())
                    newPerm |= (uint)item.perm;

            if (mTargetPlayerUuid.empty()) mCfsp->mSaveData.publicPermission = newPerm;
            else if (newPerm) mCfsp->mSaveData.permission[mTargetPlayerUuid] = newPerm;
            else if (
                auto it = mCfsp->mSaveData.permission.find(mTargetPlayerUuid); it != mCfsp->mSaveData.permission.end()
            )
                mCfsp->mSaveData.permission.erase(it);

            mCfsp->mShouldSave = true;
            if (!mCfsp->mSimPlayer) mCfsp->save();

            base::OperateResult::success("manager.success.operate"_tr()).sendTo(player);
        }
    );
}

} // namespace coral_fans::cfsp::gui
