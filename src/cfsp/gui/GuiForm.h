
#include "cfsp/simplayer/CFSP.h"
#include "mc/world/actor/player/Player.h"


namespace coral_fans::cfsp::gui {
void sendManagerMainMenu(Player*);
void sendMainMenu(Player*);
void sendSplist(Player*);
void sendGroupList(Player*);
void sendSpOperatorList(Player*, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::SimPlayerInfo>);
void sendGroupOperatorList(Player*, boost::shared_ptr<coral_fans::cfsp::SimPlayerManager::GroupInfo>);
} // namespace coral_fans::cfsp::gui