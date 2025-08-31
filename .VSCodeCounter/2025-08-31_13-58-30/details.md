# Details

Date : 2025-08-31 13:58:30

Directory f:\\Project\\levilamina\\1.4.x\\CFSP

Total : 48 files,  2134 codes, 37 comments, 403 blanks, all 2574 lines

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
| [src/cfsp/CFSP.cpp](/src/cfsp/CFSP.cpp) | C++ | 21 | 1 | 9 | 31 |
| [src/cfsp/CFSP.h](/src/cfsp/CFSP.h) | C++ | 15 | 6 | 14 | 35 |
| [src/cfsp/Config.h](/src/cfsp/Config.h) | C++ | 28 | 0 | 6 | 34 |
| [src/cfsp/MemoryOperators.cpp](/src/cfsp/MemoryOperators.cpp) | C++ | 2 | 2 | 3 | 7 |
| [src/cfsp/PermissionConfig.h](/src/cfsp/PermissionConfig.h) | C++ | 14 | 0 | 3 | 17 |
| [src/cfsp/base/Macros.h](/src/cfsp/base/Macros.h) | C++ | 11 | 0 | 5 | 16 |
| [src/cfsp/base/OperateResult.h](/src/cfsp/base/OperateResult.h) | C++ | 34 | 0 | 8 | 42 |
| [src/cfsp/base/Utils.cpp](/src/cfsp/base/Utils.cpp) | C++ | 69 | 0 | 11 | 80 |
| [src/cfsp/base/Utils.h](/src/cfsp/base/Utils.h) | C++ | 8 | 0 | 6 | 14 |
| [src/cfsp/core/group/CFSPGroup.cpp](/src/cfsp/core/group/CFSPGroup.cpp) | C++ | 13 | 0 | 2 | 15 |
| [src/cfsp/core/group/CFSPGroup.h](/src/cfsp/core/group/CFSPGroup.h) | C++ | 13 | 0 | 4 | 17 |
| [src/cfsp/core/group/GroupData.h](/src/cfsp/core/group/GroupData.h) | C++ | 13 | 0 | 2 | 15 |
| [src/cfsp/core/group/GroupPermission.h](/src/cfsp/core/group/GroupPermission.h) | C++ | 5 | 0 | 0 | 5 |
| [src/cfsp/core/helper/SimPlayerHelper.cpp](/src/cfsp/core/helper/SimPlayerHelper.cpp) | C++ | 8 | 0 | 2 | 10 |
| [src/cfsp/core/helper/SimPlayerHelper.h](/src/cfsp/core/helper/SimPlayerHelper.h) | C++ | 10 | 0 | 4 | 14 |
| [src/cfsp/core/helper/SimPlayerSaveHelper.cpp](/src/cfsp/core/helper/SimPlayerSaveHelper.cpp) | C++ | 139 | 0 | 6 | 145 |
| [src/cfsp/core/manager/CFSPManager.cpp](/src/cfsp/core/manager/CFSPManager.cpp) | C++ | 234 | 3 | 28 | 265 |
| [src/cfsp/core/manager/CFSPManager.h](/src/cfsp/core/manager/CFSPManager.h) | C++ | 58 | 0 | 7 | 65 |
| [src/cfsp/core/manager/CFSPManagerGroup.cpp](/src/cfsp/core/manager/CFSPManagerGroup.cpp) | C++ | 19 | 0 | 3 | 22 |
| [src/cfsp/core/manager/CFSPManagerSp.cpp](/src/cfsp/core/manager/CFSPManagerSp.cpp) | C++ | 106 | 2 | 7 | 115 |
| [src/cfsp/core/simPlayer/SimPlayer.h](/src/cfsp/core/simPlayer/SimPlayer.h) | C++ | 28 | 0 | 5 | 33 |
| [src/cfsp/core/simPlayer/SimPlayerBase.cpp](/src/cfsp/core/simPlayer/SimPlayerBase.cpp) | C++ | 31 | 0 | 6 | 37 |
| [src/cfsp/core/simPlayer/SimPlayerInv.cpp](/src/cfsp/core/simPlayer/SimPlayerInv.cpp) | C++ | 17 | 0 | 3 | 20 |
| [src/cfsp/core/simPlayer/SimPlayerPermission.h](/src/cfsp/core/simPlayer/SimPlayerPermission.h) | C++ | 8 | 0 | 1 | 9 |
| [src/cfsp/core/simPlayer/SimPlayerSave.cpp](/src/cfsp/core/simPlayer/SimPlayerSave.cpp) | C++ | 30 | 0 | 3 | 33 |
| [src/cfsp/core/simPlayer/SimPlayerSaveData.h](/src/cfsp/core/simPlayer/SimPlayerSaveData.h) | C++ | 21 | 0 | 3 | 24 |
| [src/cfsp/entrance/command/ComandManager.cpp](/src/cfsp/entrance/command/ComandManager.cpp) | C++ | 68 | 3 | 8 | 79 |
| [src/cfsp/entrance/command/ComandManager.h](/src/cfsp/entrance/command/ComandManager.h) | C++ | 18 | 1 | 5 | 24 |
| [src/cfsp/entrance/command/GuiCommand.cpp](/src/cfsp/entrance/command/GuiCommand.cpp) | C++ | 56 | 3 | 5 | 64 |
| [src/cfsp/entrance/command/SpCommand.cpp](/src/cfsp/entrance/command/SpCommand.cpp) | C++ | 100 | 2 | 4 | 106 |
| [src/cfsp/entrance/gui/GuiManager.cpp](/src/cfsp/entrance/gui/GuiManager.cpp) | C++ | 158 | 0 | 11 | 169 |
| [src/cfsp/entrance/gui/GuiManager.h](/src/cfsp/entrance/gui/GuiManager.h) | C++ | 33 | 0 | 5 | 38 |
| [src/cfsp/entrance/gui/OperateGroupGui.cpp](/src/cfsp/entrance/gui/OperateGroupGui.cpp) | C++ | 5 | 0 | 2 | 7 |
| [src/cfsp/entrance/gui/OperateSpGui.cpp](/src/cfsp/entrance/gui/OperateSpGui.cpp) | C++ | 70 | 0 | 3 | 73 |
| [src/lang/en\_US.json](/src/lang/en_US.json) | JSON | 7 | 0 | 0 | 7 |
| [src/lang/zh\_CN.json](/src/lang/zh_CN.json) | JSON | 92 | 0 | 0 | 92 |
| [tooth.json](/tooth.json) | JSON | 26 | 0 | 0 | 26 |
| [xmake.lua](/xmake.lua) | XMake | 59 | 3 | 8 | 70 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)