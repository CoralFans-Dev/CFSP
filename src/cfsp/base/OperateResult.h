#include <string>
namespace coral_fans::cfsp::base {
class OperateResult {
public:
    enum class Type : int {
        none    = 0,
        success = 1,
        warn    = 2,
        error   = 3,
    };

public:
    Type        mType;
    std::string info;

public:
    OperateResult(Type mType, std::string info) : mType(mType), info(info) {}
    OperateResult() { mType = Type::none; }
};
} // namespace coral_fans::cfsp::base