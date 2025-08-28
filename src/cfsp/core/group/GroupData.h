#include <string>
#include <unordered_set>

namespace coral_fans::cfsp::group {
struct GroupData {
    int                             version = 1;
    std::string                     name;
    std::unordered_set<std::string> splist;    // 假人名称
    std::string                     ownerUuid; // 所有者uuid
    std::unordered_set<std::string> adminUuid; // 管理员uuid
};
} // namespace coral_fans::cfsp::group
