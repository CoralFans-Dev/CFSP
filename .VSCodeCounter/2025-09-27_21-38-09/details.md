# Details

Date : 2025-09-27 21:38:09

Directory f:\\Project\\levilamina\\1.5.x\\CFSP

Total : 61 files,  7323 codes, 95 comments, 767 blanks, all 8185 lines

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [.github/ISSUE\_TEMPLATE/bug\_report.yml](/.github/ISSUE_TEMPLATE/bug_report.yml) | YAML | 47 | 0 | 9 | 56 |
| [.github/ISSUE\_TEMPLATE/feature\_request.yml](/.github/ISSUE_TEMPLATE/feature_request.yml) | YAML | 25 | 0 | 4 | 29 |
| [.github/PULL\_REQUEST\_TEMPLATE.md](/.github/PULL_REQUEST_TEMPLATE.md) | Markdown | 10 | 0 | 9 | 19 |
| [.github/workflows/build.yml](/.github/workflows/build.yml) | YAML | 34 | 10 | 12 | 56 |
| [.github/workflows/release.yml](/.github/workflows/release.yml) | YAML | 68 | 0 | 17 | 85 |
| [CHANGELOG.md](/CHANGELOG.md) | Markdown | 106 | 0 | 73 | 179 |
| [CHANGELOG\_ZH.md](/CHANGELOG_ZH.md) | Markdown | 106 | 0 | 73 | 179 |
| [README.md](/README.md) | Markdown | 14 | 0 | 12 | 26 |
| [manifest.json](/manifest.json) | JSON | 7 | 0 | 0 | 7 |
| [scripts/after\_build.lua](/scripts/after_build.lua) | Lua | 105 | 1 | 19 | 125 |
| [src/cfsp/CFSP.cpp](/src/cfsp/CFSP.cpp) | C++ | 22 | 1 | 9 | 32 |
| [src/cfsp/CFSP.h](/src/cfsp/CFSP.h) | C++ | 15 | 6 | 14 | 35 |
| [src/cfsp/Config.h](/src/cfsp/Config.h) | C++ | 28 | 0 | 6 | 34 |
| [src/cfsp/MemoryOperators.cpp](/src/cfsp/MemoryOperators.cpp) | C++ | 2 | 2 | 3 | 7 |
| [src/cfsp/PermissionConfig.h](/src/cfsp/PermissionConfig.h) | C++ | 70 | 0 | 3 | 73 |
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
| [src/cfsp/core/helper/CFSPHelperManager.h](/src/cfsp/core/helper/CFSPHelperManager.h) | C++ | 22 | 0 | 5 | 27 |
| [src/cfsp/core/helper/CFSPOperateHelper.cpp](/src/cfsp/core/helper/CFSPOperateHelper.cpp) | C++ | 46 | 0 | 3 | 49 |
| [src/cfsp/core/helper/CFSPSaveHelper.cpp](/src/cfsp/core/helper/CFSPSaveHelper.cpp) | C++ | 153 | 0 | 7 | 160 |
| [src/cfsp/core/helper/CFSPSchduleHelper.cpp](/src/cfsp/core/helper/CFSPSchduleHelper.cpp) | C++ | 11 | 0 | 3 | 14 |
| [src/cfsp/core/manager/CFSPGroupManager.cpp](/src/cfsp/core/manager/CFSPGroupManager.cpp) | C++ | 193 | 1 | 12 | 206 |
| [src/cfsp/core/manager/CFSPManager.cpp](/src/cfsp/core/manager/CFSPManager.cpp) | C++ | 236 | 0 | 25 | 261 |
| [src/cfsp/core/manager/CFSPManager.h](/src/cfsp/core/manager/CFSPManager.h) | C++ | 206 | 0 | 24 | 230 |
| [src/cfsp/core/manager/CFSPPermissionManager.cpp](/src/cfsp/core/manager/CFSPPermissionManager.cpp) | C++ | 365 | 5 | 15 | 385 |
| [src/cfsp/core/manager/CFSPSpManager.cpp](/src/cfsp/core/manager/CFSPSpManager.cpp) | C++ | 330 | 12 | 28 | 370 |
| [src/cfsp/core/simPlayer/SimPlayer.cpp](/src/cfsp/core/simPlayer/SimPlayer.cpp) | C++ | 161 | 4 | 21 | 186 |
| [src/cfsp/core/simPlayer/SimPlayer.h](/src/cfsp/core/simPlayer/SimPlayer.h) | C++ | 72 | 0 | 11 | 83 |
| [src/cfsp/core/simPlayer/SimPlayerAction.cpp](/src/cfsp/core/simPlayer/SimPlayerAction.cpp) | C++ | 161 | 1 | 7 | 169 |
| [src/cfsp/core/simPlayer/SimPlayerBase.cpp](/src/cfsp/core/simPlayer/SimPlayerBase.cpp) | C++ | 31 | 0 | 7 | 38 |
| [src/cfsp/core/simPlayer/SimPlayerInv.cpp](/src/cfsp/core/simPlayer/SimPlayerInv.cpp) | C++ | 115 | 0 | 6 | 121 |
| [src/cfsp/core/simPlayer/SimPlayerMessage.cpp](/src/cfsp/core/simPlayer/SimPlayerMessage.cpp) | C++ | 35 | 0 | 2 | 37 |
| [src/cfsp/core/simPlayer/SimPlayerMove.cpp](/src/cfsp/core/simPlayer/SimPlayerMove.cpp) | C++ | 37 | 0 | 3 | 40 |
| [src/cfsp/core/simPlayer/SimPlayerPermission.h](/src/cfsp/core/simPlayer/SimPlayerPermission.h) | C++ | 32 | 0 | 1 | 33 |
| [src/cfsp/core/simPlayer/SimPlayerSave.cpp](/src/cfsp/core/simPlayer/SimPlayerSave.cpp) | C++ | 63 | 0 | 3 | 66 |
| [src/cfsp/core/simPlayer/SimPlayerSaveData.h](/src/cfsp/core/simPlayer/SimPlayerSaveData.h) | C++ | 21 | 0 | 2 | 23 |
| [src/cfsp/core/simPlayer/SimPlayerStatus.cpp](/src/cfsp/core/simPlayer/SimPlayerStatus.cpp) | C++ | 56 | 0 | 4 | 60 |
| [src/cfsp/entrance/command/ComandManager.cpp](/src/cfsp/entrance/command/ComandManager.cpp) | C++ | 113 | 4 | 9 | 126 |
| [src/cfsp/entrance/command/ComandManager.h](/src/cfsp/entrance/command/ComandManager.h) | C++ | 19 | 1 | 5 | 25 |
| [src/cfsp/entrance/command/GroupCommand.cpp](/src/cfsp/entrance/command/GroupCommand.cpp) | C++ | 587 | 15 | 19 | 621 |
| [src/cfsp/entrance/command/GuiCommand.cpp](/src/cfsp/entrance/command/GuiCommand.cpp) | C++ | 41 | 3 | 4 | 48 |
| [src/cfsp/entrance/command/SpCommand.cpp](/src/cfsp/entrance/command/SpCommand.cpp) | C++ | 622 | 16 | 24 | 662 |
| [src/cfsp/entrance/gui/GuiManager.cpp](/src/cfsp/entrance/gui/GuiManager.cpp) | C++ | 193 | 0 | 12 | 205 |
| [src/cfsp/entrance/gui/GuiManager.h](/src/cfsp/entrance/gui/GuiManager.h) | C++ | 128 | 0 | 7 | 135 |
| [src/cfsp/entrance/gui/OperateGroupGui.cpp](/src/cfsp/entrance/gui/OperateGroupGui.cpp) | C++ | 858 | 0 | 85 | 943 |
| [src/cfsp/entrance/gui/OperateSpGui.cpp](/src/cfsp/entrance/gui/OperateSpGui.cpp) | C++ | 922 | 0 | 87 | 1,009 |
| [src/lang/en\_US.json](/src/lang/en_US.json) | JSON | 7 | 0 | 0 | 7 |
| [src/lang/zh\_CN.json](/src/lang/zh_CN.json) | JSON | 313 | 0 | 0 | 313 |
| [tooth.json](/tooth.json) | JSON | 26 | 0 | 0 | 26 |
| [xmake.lua](/xmake.lua) | XMake | 58 | 3 | 8 | 69 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)