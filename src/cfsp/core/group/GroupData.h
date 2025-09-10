#include "GroupPermission.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace coral_fans::cfsp::group {
struct GroupData {
    int                                              version = 1;
    std::string                                      name;
    std::unordered_set<std::string>                  splist;    // 假人名称
    std::string                                      ownerUuid; // 所有者uuid
    GroupPermission                                  publicPermission;
    std::unordered_map<std::string, GroupPermission> permission; // <玩家uuid, permission>
};
} // namespace coral_fans::cfsp::group
