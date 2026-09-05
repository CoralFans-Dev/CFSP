#include "SpPermPage.h"
#include "SpPermPage2.h"
#include "cfsp/base/Utils.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/Level.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace coral_fans::cfsp::gui {

void SpPermPage::sendTo(Player& player) {
    using ll::i18n_literals::operator""_tr;
    std::vector<std::pair<std::string, std::string>> splist;
    auto                                             level = ll::service::getLevel();
    if (level.has_value())
        level->forEachPlayer([&splist, pname = player.mName.get()](Player& player) {
            if (!player.isSimulatedPlayer() && pname != player.mName.get())
                splist.emplace_back(std::make_pair(player.mName, player.getUuid().asString()));
            return true;
        });
    for (auto i : mCfsp->mSaveData.permission)
        splist.emplace_back(std::make_pair(base::utils::tryGetPlayerName(i.first), i.first));
    std::sort(splist.begin(), splist.end());

    auto form = ll::form::SimpleForm("gui.perm.spTitle"_tr());
    auto self = shared_from_this();
    int  size = (int)splist.size();
    if (size) {
        form.appendButton(splist[0].first, [this, self, targetPlayer = splist[0]](Player& player) {
            std::make_shared<SpPermPage2>(mCfsp, self, targetPlayer.first, targetPlayer.second)->sendTo(player);
        });
        for (int i = 1; i < size; i++)
            if (splist[i] != splist[i - 1])
                form.appendButton(splist[i].first, [this, self, targetPlayer = splist[i]](Player& player) {
                    std::make_shared<SpPermPage2>(mCfsp, self, targetPlayer.first, targetPlayer.second)->sendTo(player);
                });
    }
    form.appendButton("gui.perm.publicSp"_tr(), [this, self](Player& player) {
        std::make_shared<SpPermPage2>(mCfsp, self)->sendTo(player);
    });
    form.sendTo(player, [this, self](Player& player, int, ll::form::FormCancelReason cancelReason) {
        this->backOnCancel(player, cancelReason);
    });
}

} // namespace coral_fans::cfsp::gui
