# Details

Date : 2025-09-09 14:13:22

Directory f:\\Project\\levilamina\\1.4.x\\CFSP

Total : 65 files,  4806 codes, 77 comments, 574 blanks, all 5457 lines

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
| [src/cfsp/PermissionConfig.h](/src/cfsp/PermissionConfig.h) | C++ | 39 | 0 | 3 | 42 |
| [src/cfsp/base/Macros.h](/src/cfsp/base/Macros.h) | C++ | 6 | 0 | 2 | 8 |
| [src/cfsp/base/OperateResult.h](/src/cfsp/base/OperateResult.h) | C++ | 35 | 0 | 8 | 43 |
| [src/cfsp/base/Schedule.h](/src/cfsp/base/Schedule.h) | C++ | 17 | 0 | 5 | 22 |
| [src/cfsp/base/Utils.cpp](/src/cfsp/base/Utils.cpp) | C++ | 45 | 0 | 6 | 51 |
| [src/cfsp/base/Utils.h](/src/cfsp/base/Utils.h) | C++ | 9 | 0 | 6 | 15 |
| [src/cfsp/core/fix/CFSPFixManager.cpp](/src/cfsp/core/fix/CFSPFixManager.cpp) | C++ | 122 | 10 | 11 | 143 |
| [src/cfsp/core/fix/CFSPFixManager.h](/src/cfsp/core/fix/CFSPFixManager.h) | C++ | 11 | 0 | 1 | 12 |
| [src/cfsp/core/group/CFSPGroup.cpp](/src/cfsp/core/group/CFSPGroup.cpp) | C++ | 15 | 0 | 2 | 17 |
| [src/cfsp/core/group/CFSPGroup.h](/src/cfsp/core/group/CFSPGroup.h) | C++ | 13 | 0 | 4 | 17 |
| [src/cfsp/core/group/GroupData.h](/src/cfsp/core/group/GroupData.h) | C++ | 13 | 0 | 2 | 15 |
| [src/cfsp/core/group/GroupPermission.h](/src/cfsp/core/group/GroupPermission.h) | C++ | 5 | 0 | 0 | 5 |
| [src/cfsp/core/helper/CFSPAutoFuncHelper.cpp](/src/cfsp/core/helper/CFSPAutoFuncHelper.cpp) | C++ | 73 | 0 | 3 | 76 |
| [src/cfsp/core/helper/CFSPHelperManager.cpp](/src/cfsp/core/helper/CFSPHelperManager.cpp) | C++ | 13 | 0 | 2 | 15 |
| [src/cfsp/core/helper/CFSPHelperManager.h](/src/cfsp/core/helper/CFSPHelperManager.h) | C++ | 18 | 0 | 5 | 23 |
| [src/cfsp/core/helper/CFSPOperateHelper.cpp](/src/cfsp/core/helper/CFSPOperateHelper.cpp) | C++ | 46 | 0 | 3 | 49 |
| [src/cfsp/core/helper/CFSPSaveHelper.cpp](/src/cfsp/core/helper/CFSPSaveHelper.cpp) | C++ | 153 | 0 | 7 | 160 |
| [src/cfsp/core/helper/CFSPSchduleHelper.cpp](/src/cfsp/core/helper/CFSPSchduleHelper.cpp) | C++ | 11 | 0 | 3 | 14 |
| [src/cfsp/core/helper/CacheUUidName.cpp](/src/cfsp/core/helper/CacheUUidName.cpp) | C++ | 0 | 0 | 1 | 1 |
| [src/cfsp/core/manager/CFSPManager.cpp](/src/cfsp/core/manager/CFSPManager.cpp) | C++ | 265 | 5 | 29 | 299 |
| [src/cfsp/core/manager/CFSPManager.h](/src/cfsp/core/manager/CFSPManager.h) | C++ | 140 | 0 | 13 | 153 |
| [src/cfsp/core/manager/CFSPManagerGroup.cpp](/src/cfsp/core/manager/CFSPManagerGroup.cpp) | C++ | 19 | 0 | 3 | 22 |
| [src/cfsp/core/manager/CFSPManagerSp.cpp](/src/cfsp/core/manager/CFSPManagerSp.cpp) | C++ | 380 | 13 | 23 | 416 |
| [src/cfsp/core/manager/CFSPManagerSpInv.cpp](/src/cfsp/core/manager/CFSPManagerSpInv.cpp) | C++ | 45 | 0 | 3 | 48 |
| [src/cfsp/core/manager/CFSPManagerSpMessage.cpp](/src/cfsp/core/manager/CFSPManagerSpMessage.cpp) | C++ | 27 | 0 | 2 | 29 |
| [src/cfsp/core/manager/CFSPManagerSpMove.cpp](/src/cfsp/core/manager/CFSPManagerSpMove.cpp) | C++ | 45 | 0 | 3 | 48 |
| [src/cfsp/core/manager/CFSPManagerSpOperate.cpp](/src/cfsp/core/manager/CFSPManagerSpOperate.cpp) | C++ | 57 | 0 | 3 | 60 |
| [src/cfsp/core/manager/CFSPManagerSpStatus.cpp](/src/cfsp/core/manager/CFSPManagerSpStatus.cpp) | C++ | 30 | 0 | 2 | 32 |
| [src/cfsp/core/simPlayer/SimPlayer.cpp](/src/cfsp/core/simPlayer/SimPlayer.cpp) | C++ | 127 | 4 | 20 | 151 |
| [src/cfsp/core/simPlayer/SimPlayer.h](/src/cfsp/core/simPlayer/SimPlayer.h) | C++ | 70 | 0 | 10 | 80 |
| [src/cfsp/core/simPlayer/SimPlayerBase.cpp](/src/cfsp/core/simPlayer/SimPlayerBase.cpp) | C++ | 35 | 0 | 8 | 43 |
| [src/cfsp/core/simPlayer/SimPlayerInv.cpp](/src/cfsp/core/simPlayer/SimPlayerInv.cpp) | C++ | 99 | 0 | 6 | 105 |
| [src/cfsp/core/simPlayer/SimPlayerMessage.cpp](/src/cfsp/core/simPlayer/SimPlayerMessage.cpp) | C++ | 35 | 0 | 2 | 37 |
| [src/cfsp/core/simPlayer/SimPlayerMove.cpp](/src/cfsp/core/simPlayer/SimPlayerMove.cpp) | C++ | 33 | 0 | 3 | 36 |
| [src/cfsp/core/simPlayer/SimPlayerOperate.cpp](/src/cfsp/core/simPlayer/SimPlayerOperate.cpp) | C++ | 154 | 1 | 7 | 162 |
| [src/cfsp/core/simPlayer/SimPlayerPermission.h](/src/cfsp/core/simPlayer/SimPlayerPermission.h) | C++ | 31 | 0 | 1 | 32 |
| [src/cfsp/core/simPlayer/SimPlayerSave.cpp](/src/cfsp/core/simPlayer/SimPlayerSave.cpp) | C++ | 59 | 0 | 4 | 63 |
| [src/cfsp/core/simPlayer/SimPlayerSaveData.h](/src/cfsp/core/simPlayer/SimPlayerSaveData.h) | C++ | 21 | 0 | 3 | 24 |
| [src/cfsp/core/simPlayer/SimPlayerStatus.cpp](/src/cfsp/core/simPlayer/SimPlayerStatus.cpp) | C++ | 56 | 0 | 4 | 60 |
| [src/cfsp/entrance/command/ComandManager.cpp](/src/cfsp/entrance/command/ComandManager.cpp) | C++ | 68 | 3 | 8 | 79 |
| [src/cfsp/entrance/command/ComandManager.h](/src/cfsp/entrance/command/ComandManager.h) | C++ | 18 | 1 | 5 | 24 |
| [src/cfsp/entrance/command/GuiCommand.cpp](/src/cfsp/entrance/command/GuiCommand.cpp) | C++ | 40 | 3 | 4 | 47 |
| [src/cfsp/entrance/command/SpCommand.cpp](/src/cfsp/entrance/command/SpCommand.cpp) | C++ | 492 | 14 | 22 | 528 |
| [src/cfsp/entrance/gui/GuiManager.cpp](/src/cfsp/entrance/gui/GuiManager.cpp) | C++ | 182 | 0 | 12 | 194 |
| [src/cfsp/entrance/gui/GuiManager.h](/src/cfsp/entrance/gui/GuiManager.h) | C++ | 65 | 0 | 6 | 71 |
| [src/cfsp/entrance/gui/OperateGroupGui.cpp](/src/cfsp/entrance/gui/OperateGroupGui.cpp) | C++ | 5 | 0 | 2 | 7 |
| [src/cfsp/entrance/gui/OperateSpGui.cpp](/src/cfsp/entrance/gui/OperateSpGui.cpp) | C++ | 707 | 0 | 52 | 759 |
| [src/lang/en\_US.json](/src/lang/en_US.json) | JSON | 7 | 0 | 0 | 7 |
| [src/lang/zh\_CN.json](/src/lang/zh_CN.json) | JSON | 212 | 0 | 0 | 212 |
| [tooth.json](/tooth.json) | JSON | 26 | 0 | 0 | 26 |
| [xmake.lua](/xmake.lua) | XMake | 59 | 3 | 8 | 70 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)