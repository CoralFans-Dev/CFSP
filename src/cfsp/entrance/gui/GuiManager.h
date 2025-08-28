#include "cfsp/core/simPlayer/SimPlayer.h"
#include "mc/world/actor/player/Player.h"


namespace coral_fans::cfsp::gui {
class GuiManager {
public:
    static GuiManager& getInstance();

private:
public:
    void sendMainMenu(Player*);
    void sendManagerMainMenu(Player*);
    void sendSplist(Player*);
    void sendGroupList(Player*);
    void sendSpOperatorList(Player*, std::shared_ptr<simulated_player::SimPlayer>);
};
} // namespace coral_fans::cfsp::gui