#pragma once

#include "ll/api/base/StdInt.h"
#include "ll/api/form/FormBase.h"
#include "mc/world/actor/player/Player.h"
#include <memory>
#include <utility>

namespace coral_fans::cfsp::gui {

class GuiPage : public std::enable_shared_from_this<GuiPage> {
public:
    explicit GuiPage(std::shared_ptr<GuiPage> parent = nullptr) : mParent(std::move(parent)) {}
    virtual ~GuiPage() = default;

public:
    virtual void sendTo(Player& player) = 0;

public:
    void back(Player& player) {
        if (mParent) mParent->sendTo(player);
    }

    bool backOnCancel(Player& player, ll::form::FormCancelReason const& cancelReason) {
        if (cancelReason.has_value() && cancelReason == ModalFormCancelReason::UserClosed) {
            back(player);
            return true;
        }
        return false;
    }

protected:
    std::shared_ptr<GuiPage> mParent;
};

} // namespace coral_fans::cfsp::gui
