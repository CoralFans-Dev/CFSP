# Details

Date : 2025-09-16 17:49:59

Directory f:\\Project\\levilamina\\1.4.x\\CFSP

Total : 61 files,  6495 codes, 91 comments, 664 blanks, all 7250 lines

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [.github/ISSUE\_TEMPLATE/bug\_report.yml](/.github/ISSUE_TEMPLATE/bug_report.yml) | YAML | 47 | 0 | 9 | 56 |
| [.github/ISSUE\_TEMPLATE/feature\_request.yml](/.github/ISSUE_TEMPLATE/feature_request.yml) | YAML | 25 | 0 | 4 | 29 |
| [.github/PULL\_REQUEST\_TEMPLATE.md](/.github/PULL_REQUEST_TEMPLATE.md) | Markdown | 10 | 0 | 9 | 19 |
| [.github/workflows/build.yml](/.github/workflows/build.yml) | YAML | 34 | 10 | 12 | 56 |
| [.github/workflows/release.yml](/.github/workflows/release.yml) | YAML | 68 | 0 | 17 | 85 |
| [CHANGELOG.md](/CHANGELOG.md) | Markdown | 87 | 0 | 58 | 145 |
| [CHANGELOG\_ZH.md](/CHANGELOG_ZH.md) | Markdown | 90 | 0 | 61 | 151 |
| [README.md](/README.md) | Markdown | 14 | 0 | 12 | 26 |
| [manifest.json](/manifest.json) | JSON | 7 | 0 | 0 | 7 |
| [scripts/after\_build.lua](/scripts/after_build.lua) | Lua | 105 | 1 | 19 | 125 |
| [src/cfsp/CFSP.cpp](/src/cfsp/CFSP.cpp) | C++ | 21 | 1 | 8 | 30 |
| [src/cfsp/CFSP.h](/src/cfsp/CFSP.h) | C++ | 15 | 6 | 14 | 35 |
| [src/cfsp/Config.h](/src/cfsp/Config.h) | C++ | 28 | 0 | 6 | 34 |
| [src/cfsp/MemoryOperators.cpp](/src/cfsp/MemoryOperators.cpp) | C++ | 2 | 2 | 3 | 7 |
| [src/cfsp/PermissionConfig.h](/src/cfsp/PermissionConfig.h) | C++ | 66 | 0 | 3 | 69 |
| [src/cfsp/base/Macros.h](/src/cfsp/base/Macros.h) | C++ | 6 | 0 | 2 | 8 |
| [src/cfsp/base/OperateResult.h](/src/cfsp/base/OperateResult.h) | C++ | 44 | 0 | 9 | 53 |
| [src/cfsp/base/Schedule.h](/src/cfsp/base/Schedule.h) | C++ | 17 | 0 | 5 | 22 |
| [src/cfsp/base/Utils.cpp](/src/cfsp/base/Utils.cpp) | C++ | 45 | 0 | 6 | 51 |
| [src/cfsp/base/Utils.h](/src/cfsp/base/Utils.h) | C++ | 9 | 0 | 6 | 15 |
| [src/cfsp/core/fix/CFSPFixManager.cpp](/src/cfsp/core/fix/CFSPFixManager.cpp) | C++ | 122 | 10 | 11 | 143 |
| [src/cfsp/core/fix/CFSPFixManager.h](/src/cfsp/core/fix/CFSPFixManager.h) | C++ | 11 | 0 | 1 | 12 |
| [src/cfsp/core/group/CFSPGroup.cpp](/src/cfsp/core/group/CFSPGroup.cpp) | C++ | 30 | 0 | 4 | 34 |
| [src/cfsp/core/group/CFSPGroup.h](/src/cfsp/core/group/CFSPGroup.h) | C++ | 15 | 0 | 4 | 19 |
| [src/cfsp/core/group/GroupData.h](/src/cfsp/core/group/GroupData.h) | C++ | 14 | 0 | 2 | 16 |
| [src/cfsp/core/group/GroupPermission.h](/src/cfsp/core/group/GroupPermission.h) | C++ | 32 | 0 | 0 | 32 |
| [src/cfsp/core/helper/CFSPAutoFuncHelper.cpp](/src/cfsp/core/helper/CFSPAutoFuncHelper.cpp) | C++ | 73 | 0 | 3 | 76 |
| [src/cfsp/core/helper/CFSPHelperManager.cpp](/src/cfsp/core/helper/CFSPHelperManager.cpp) | C++ | 13 | 0 | 2 | 15 |
| [src/cfsp/core/helper/CFSPHelperManager.h](/src/cfsp/core/helper/CFSPHelperManager.h) | C++ | 18 | 0 | 5 | 23 |
| [src/cfsp/core/helper/CFSPOperateHelper.cpp](/src/cfsp/core/helper/CFSPOperateHelper.cpp) | C++ | 46 | 0 | 3 | 49 |
| [src/cfsp/core/helper/CFSPSaveHelper.cpp](/src/cfsp/core/helper/CFSPSaveHelper.cpp) | C++ | 153 | 0 | 7 | 160 |
| [src/cfsp/core/helper/CFSPSchduleHelper.cpp](/src/cfsp/core/helper/CFSPSchduleHelper.cpp) | C++ | 11 | 0 | 3 | 14 |
| [src/cfsp/core/helper/CacheUUidName.cpp](/src/cfsp/core/helper/CacheUUidName.cpp) | C++ | 0 | 0 | 1 | 1 |
| [src/cfsp/core/manager/CFSPManager.cpp](/src/cfsp/core/manager/CFSPManager.cpp) | C++ | 293 | 5 | 31 | 329 |
| [src/cfsp/core/manager/CFSPManager.h](/src/cfsp/core/manager/CFSPManager.h) | C++ | 195 | 0 | 23 | 218 |
| [src/cfsp/core/manager/CFSPManagerGroup.cpp](/src/cfsp/core/manager/CFSPManagerGroup.cpp) | C++ | 276 | 1 | 15 | 292 |
| [src/cfsp/core/manager/CFSPManagerSp.cpp](/src/cfsp/core/manager/CFSPManagerSp.cpp) | C++ | 419 | 12 | 31 | 462 |
| [src/cfsp/core/simPlayer/SimPlayer.cpp](/src/cfsp/core/simPlayer/SimPlayer.cpp) | C++ | 160 | 4 | 21 | 185 |
| [src/cfsp/core/simPlayer/SimPlayer.h](/src/cfsp/core/simPlayer/SimPlayer.h) | C++ | 72 | 0 | 11 | 83 |
| [src/cfsp/core/simPlayer/SimPlayerBase.cpp](/src/cfsp/core/simPlayer/SimPlayerBase.cpp) | C++ | 31 | 0 | 7 | 38 |
| [src/cfsp/core/simPlayer/SimPlayerInv.cpp](/src/cfsp/core/simPlayer/SimPlayerInv.cpp) | C++ | 115 | 0 | 6 | 121 |
| [src/cfsp/core/simPlayer/SimPlayerMessage.cpp](/src/cfsp/core/simPlayer/SimPlayerMessage.cpp) | C++ | 35 | 0 | 2 | 37 |
| [src/cfsp/core/simPlayer/SimPlayerMove.cpp](/src/cfsp/core/simPlayer/SimPlayerMove.cpp) | C++ | 37 | 0 | 3 | 40 |
| [src/cfsp/core/simPlayer/SimPlayerOperate.cpp](/src/cfsp/core/simPlayer/SimPlayerOperate.cpp) | C++ | 154 | 1 | 7 | 162 |
| [src/cfsp/core/simPlayer/SimPlayerPermission.h](/src/cfsp/core/simPlayer/SimPlayerPermission.h) | C++ | 32 | 0 | 1 | 33 |
| [src/cfsp/core/simPlayer/SimPlayerSave.cpp](/src/cfsp/core/simPlayer/SimPlayerSave.cpp) | C++ | 63 | 0 | 3 | 66 |
| [src/cfsp/core/simPlayer/SimPlayerSaveData.h](/src/cfsp/core/simPlayer/SimPlayerSaveData.h) | C++ | 21 | 0 | 2 | 23 |
| [src/cfsp/core/simPlayer/SimPlayerStatus.cpp](/src/cfsp/core/simPlayer/SimPlayerStatus.cpp) | C++ | 56 | 0 | 4 | 60 |
| [src/cfsp/entrance/command/ComandManager.cpp](/src/cfsp/entrance/command/ComandManager.cpp) | C++ | 69 | 3 | 8 | 80 |
| [src/cfsp/entrance/command/ComandManager.h](/src/cfsp/entrance/command/ComandManager.h) | C++ | 19 | 1 | 5 | 25 |
| [src/cfsp/entrance/command/GroupCommand.cpp](/src/cfsp/entrance/command/GroupCommand.cpp) | C++ | 514 | 13 | 17 | 544 |
| [src/cfsp/entrance/command/GuiCommand.cpp](/src/cfsp/entrance/command/GuiCommand.cpp) | C++ | 40 | 3 | 4 | 47 |
| [src/cfsp/entrance/command/SpCommand.cpp](/src/cfsp/entrance/command/SpCommand.cpp) | C++ | 544 | 15 | 23 | 582 |
| [src/cfsp/entrance/gui/GuiManager.cpp](/src/cfsp/entrance/gui/GuiManager.cpp) | C++ | 189 | 0 | 12 | 201 |
| [src/cfsp/entrance/gui/GuiManager.h](/src/cfsp/entrance/gui/GuiManager.h) | C++ | 113 | 0 | 7 | 120 |
| [src/cfsp/entrance/gui/OperateGroupGui.cpp](/src/cfsp/entrance/gui/OperateGroupGui.cpp) | C++ | 688 | 0 | 52 | 740 |
| [src/cfsp/entrance/gui/OperateSpGui.cpp](/src/cfsp/entrance/gui/OperateSpGui.cpp) | C++ | 726 | 0 | 52 | 778 |
| [src/lang/en\_US.json](/src/lang/en_US.json) | JSON | 7 | 0 | 0 | 7 |
| [src/lang/zh\_CN.json](/src/lang/zh_CN.json) | JSON | 264 | 0 | 0 | 264 |
| [tooth.json](/tooth.json) | JSON | 26 | 0 | 0 | 26 |
| [xmake.lua](/xmake.lua) | XMake | 59 | 3 | 8 | 70 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)