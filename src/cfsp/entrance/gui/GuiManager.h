#pragma once

#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "mc/world/actor/player/Player.h"
#include <optional>
#include <unordered_set>

namespace coral_fans::cfsp::gui {
class GuiManager {
public:
    static GuiManager& getInstance();

private:
    std::optional<Vec3>  tryGetVec3(std::string&);
    std::optional<int>   tryGetInt(std::string&);
    std::optional<float> tryGetFloat(std::string&);

private:
    void sendManagePage(Player&);

private:
    void sendOperateSpPage(Player&, std::shared_ptr<simulated_player::SimPlayer>);
    void sendPublicSplist(Player&);
    void sendAllSplist(Player&);
    void sendCreateSpPage(
        Player&     player,
        int         defDim          = 0,
        std::string defPos          = "",
        std::string defName         = "",
        int         defLockUniqueId = 0
    );
    void sendSpInfoPage(Player&, std::shared_ptr<simulated_player::SimPlayer>);
    void sendSpInvOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);
    void sendSpTpOperatorPage(
        Player&                                      player,
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        int                                          defDim = 0,
        std::string                                  defPos = ""
    );
    void sendSpMessageOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);
    void sendSpMoveOperatorPage(
        Player&                                      player,
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        uint                                         perm,
        std::string                                  defPos   = "",
        std::string                                  defSpeed = "4.3",
        int                                          defOp    = 0
    );
    void
    sendSpLookOperatorPage(Player& player, std::shared_ptr<simulated_player::SimPlayer> cfsp, std::string defPos = "");
    void sendSpActionOperatorPage(
        Player&                                      player,
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        uint                                         perm,
        std::string                                  defTimes    = "1",
        std::string                                  defInterval = "1",
        int                                          defOp       = 0
    );
    void sendSpLongActionOperatorPage(
        Player&                                      player,
        std::shared_ptr<simulated_player::SimPlayer> cfsp,
        uint                                         perm,
        std::string                                  defLong     = "10",
        std::string                                  defTimes    = "1",
        std::string                                  defInterval = "1",
        int                                          defOp       = 0
    );
    void sendSpStatusOperatorPage(Player&, std::shared_ptr<simulated_player::SimPlayer>, uint);

private:
    void sendAllGroupList(Player&);
    void sendCreateGroupPage(Player&);
    void sendManageSpInGroupPage(Player&, std::shared_ptr<group::CFSPGroup>, uint);
    void sendOperateGroupPage(Player&, std::shared_ptr<group::CFSPGroup>);
    void sendGroupInfoPage(Player&, std::shared_ptr<group::CFSPGroup>);
    void sendGroupTpOperatorPage(
        Player&                           player,
        std::shared_ptr<group::CFSPGroup> group,
        int                               defDim = 0,
        std::string                       defPos = ""
    );
    void sendGroupLookOperatorPage(Player& player, std::shared_ptr<group::CFSPGroup> group, std::string defPos = "");
    void sendGroupMoveOperatorPage(
        Player&                           player,
        std::shared_ptr<group::CFSPGroup> group,
        uint                              perm,
        std::string                       defPos   = "",
        std::string                       defSpeed = "4.3",
        int                               defOp    = 0
    );
    void sendGroupInvOperatorPage(Player&, std::shared_ptr<group::CFSPGroup>, uint);
    void sendGroupMessageOperatorPage(Player&, std::shared_ptr<group::CFSPGroup>, uint);
    void sendGroupActionOperatorPage(
        Player&                           player,
        std::shared_ptr<group::CFSPGroup> group,
        uint                              perm,
        std::string                       defTimes    = "1",
        std::string                       defInterval = "1",
        int                               defOp       = 0
    );
    void sendGroupLongActionOperatorPage(
        Player&                           player,
        std::shared_ptr<group::CFSPGroup> group,
        uint                              perm,
        std::string                       defLong     = "10",
        std::string                       defTimes    = "1",
        std::string                       defInterval = "1",
        int                               defOp       = 0
    );
    void sendGroupStatusOperatorPage(Player&, std::shared_ptr<group::CFSPGroup>, uint);
    void sendGroupDeleteSpConfrim(Player&, std::unordered_set<std::string>&);

public:
    void sendMainMenu(Player&);
    void sendManagerMainMenu(Player&);
    void sendSplist(Player&);
    void sendGroupList(Player&);
};
} // namespace coral_fans::cfsp::gui