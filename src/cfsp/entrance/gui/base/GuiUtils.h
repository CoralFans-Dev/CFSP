#pragma once

#include "cfsp/base/OperateResult.h"
#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "ll/api/form/CustomForm.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace coral_fans::cfsp::gui::utils {

std::optional<Vec3>  tryGetVec3(std::string& str);
std::optional<int>   tryGetInt(std::string& str);
std::optional<float> tryGetFloat(std::string& str);

std::optional<std::string> getStringElement(ll::form::CustomFormResult const& elements, std::string const& key);
std::optional<bool>        getToggleElement(ll::form::CustomFormResult const& elements, std::string const& key);

Vec3        getLookOrFeetPos(Player& player);
std::string posToString(Vec3 const& pos);

void                appendDimDropdown(ll::form::CustomForm& form, int defDim = 0);
std::optional<int>  getDimElement(ll::form::CustomFormResult const& elements);
std::optional<bool> getYesNoElement(ll::form::CustomFormResult const& elements, std::string const& key);

int findOpIndex(std::vector<std::string> const& ops, std::string const& op);

void sendResults(Player& player, std::vector<base::OperateResult> const& results);

uint calcSpPerm(Player& player, std::shared_ptr<simulated_player::SimPlayer> const& cfsp);
uint calcGroupPerm(Player& player, std::shared_ptr<group::CFSPGroup> const& group);

void showConfirmDialog(
    Player&                      player,
    std::string const&           title,
    std::string const&           content,
    std::function<void(Player&)> onYes    = {},
    std::function<void(Player&)> onNo     = {},
    std::function<void(Player&)> onCancel = {}
);

} // namespace coral_fans::cfsp::gui::utils
