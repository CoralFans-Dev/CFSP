#include "cfsp/core/group/CFSPGroup.h"
#include "cfsp/core/simPlayer/SimPlayer.h"
#include "mc/world/actor/player/Player.h"


namespace coral_fans::cfsp::gui {
class GuiManager {
public:
    static GuiManager& getInstance();

private:
    void sendPublicSplist(Player&);
    void sendAllSplist(Player&);
    void sendAllGroupList(Player&);
    void sendManagePage(Player&);
    void sendNewSpPage(Player& pl, int defDim = 0, std::string defPos = "", std::string defName = "");

public:
    void sendMainMenu(Player&);
    void sendManagerMainMenu(Player&);
    void sendSplist(Player&);
    void sendGroupList(Player&);
    void sendSpOperatorList(Player&, std::shared_ptr<simulated_player::SimPlayer>);
    void sendGroupOperatorList(Player& pl, std::shared_ptr<group::CFSPGroup> groupInfo);
};
} // namespace coral_fans::cfsp::gui