# Details

Date : 2025-08-29 18:04:51

Directory f:\\Project\\levilamina\\1.4.x\\CFSP

Total : 44 files,  1626 codes, 34 comments, 371 blanks, all 2031 lines

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
| [src/cfsp/CFSP.cpp](/src/cfsp/CFSP.cpp) | C++ | 18 | 1 | 9 | 28 |
| [src/cfsp/CFSP.h](/src/cfsp/CFSP.h) | C++ | 15 | 6 | 14 | 35 |
| [src/cfsp/Config.h](/src/cfsp/Config.h) | C++ | 28 | 0 | 6 | 34 |
| [src/cfsp/MemoryOperators.cpp](/src/cfsp/MemoryOperators.cpp) | C++ | 2 | 2 | 3 | 7 |
| [src/cfsp/PermissionConfig.h](/src/cfsp/PermissionConfig.h) | C++ | 17 | 0 | 4 | 21 |
| [src/cfsp/base/Macros.h](/src/cfsp/base/Macros.h) | C++ | 11 | 0 | 5 | 16 |
| [src/cfsp/base/OperateResult.h](/src/cfsp/base/OperateResult.h) | C++ | 27 | 0 | 6 | 33 |
| [src/cfsp/base/Utils.cpp](/src/cfsp/base/Utils.cpp) | C++ | 85 | 0 | 13 | 98 |
| [src/cfsp/base/Utils.h](/src/cfsp/base/Utils.h) | C++ | 10 | 0 | 8 | 18 |
| [src/cfsp/core/group/CFSPGroup.cpp](/src/cfsp/core/group/CFSPGroup.cpp) | C++ | 12 | 0 | 1 | 13 |
| [src/cfsp/core/group/CFSPGroup.h](/src/cfsp/core/group/CFSPGroup.h) | C++ | 11 | 0 | 3 | 14 |
| [src/cfsp/core/group/GroupData.h](/src/cfsp/core/group/GroupData.h) | C++ | 13 | 0 | 2 | 15 |
| [src/cfsp/core/group/GroupPermission.h](/src/cfsp/core/group/GroupPermission.h) | C++ | 5 | 0 | 0 | 5 |
| [src/cfsp/core/helper/SimPlayerHelper.cpp](/src/cfsp/core/helper/SimPlayerHelper.cpp) | C++ | 8 | 0 | 2 | 10 |
| [src/cfsp/core/helper/SimPlayerHelper.h](/src/cfsp/core/helper/SimPlayerHelper.h) | C++ | 10 | 0 | 4 | 14 |
| [src/cfsp/core/helper/SimPlayerSaveHelper.cpp](/src/cfsp/core/helper/SimPlayerSaveHelper.cpp) | C++ | 139 | 0 | 6 | 145 |
| [src/cfsp/core/manager/CFSPManager.cpp](/src/cfsp/core/manager/CFSPManager.cpp) | C++ | 177 | 4 | 17 | 198 |
| [src/cfsp/core/manager/CFSPManager.h](/src/cfsp/core/manager/CFSPManager.h) | C++ | 46 | 0 | 7 | 53 |
| [src/cfsp/core/manager/CFSPManagerSp.cpp](/src/cfsp/core/manager/CFSPManagerSp.cpp) | C++ | 57 | 2 | 6 | 65 |
| [src/cfsp/core/simPlayer/SimPlayer.h](/src/cfsp/core/simPlayer/SimPlayer.h) | C++ | 28 | 0 | 5 | 33 |
| [src/cfsp/core/simPlayer/SimPlayerBase.cpp](/src/cfsp/core/simPlayer/SimPlayerBase.cpp) | C++ | 31 | 0 | 6 | 37 |
| [src/cfsp/core/simPlayer/SimPlayerInv.cpp](/src/cfsp/core/simPlayer/SimPlayerInv.cpp) | C++ | 17 | 0 | 3 | 20 |
| [src/cfsp/core/simPlayer/SimPlayerPermission.h](/src/cfsp/core/simPlayer/SimPlayerPermission.h) | C++ | 8 | 0 | 1 | 9 |
| [src/cfsp/core/simPlayer/SimPlayerSave.cpp](/src/cfsp/core/simPlayer/SimPlayerSave.cpp) | C++ | 29 | 0 | 3 | 32 |
| [src/cfsp/core/simPlayer/SimPlayerSaveData.h](/src/cfsp/core/simPlayer/SimPlayerSaveData.h) | C++ | 22 | 0 | 3 | 25 |
| [src/cfsp/entrance/command/ComandManager.cpp](/src/cfsp/entrance/command/ComandManager.cpp) | C++ | 31 | 2 | 6 | 39 |
| [src/cfsp/entrance/command/ComandManager.h](/src/cfsp/entrance/command/ComandManager.h) | C++ | 17 | 0 | 5 | 22 |
| [src/cfsp/entrance/command/GuiCommand.cpp](/src/cfsp/entrance/command/GuiCommand.cpp) | C++ | 56 | 3 | 5 | 64 |
| [src/cfsp/entrance/gui/GuiManager.cpp](/src/cfsp/entrance/gui/GuiManager.cpp) | C++ | 55 | 0 | 5 | 60 |
| [src/cfsp/entrance/gui/GuiManager.h](/src/cfsp/entrance/gui/GuiManager.h) | C++ | 22 | 0 | 4 | 26 |
| [src/lang/en\_US.json](/src/lang/en_US.json) | JSON | 7 | 0 | 0 | 7 |
| [src/lang/zh\_CN.json](/src/lang/zh_CN.json) | JSON | 40 | 0 | 0 | 40 |
| [tooth.json](/tooth.json) | JSON | 26 | 0 | 0 | 26 |
| [xmake.lua](/xmake.lua) | XMake | 59 | 3 | 8 | 70 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)