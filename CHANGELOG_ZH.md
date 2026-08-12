# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [26.20.1] - 2026-8.12

### Added

+ 创建假人时，假人的游戏模式将与创建者保持一致

### Fixed

+ 修复了假人加载范围的问题

## [26.20.0] - 2026-7.26

### Added

+ 适配26.20.0

## [26.10.0] - 2026-5.21

### Added

+ 适配1.26.10
+ 适配客户端

## [3.4.0] - 2026-2.9

### Added

+ 适配1.21.132

### Fixed

+ 解决了更多的与sapi之间的兼容性问题

## [3.3.1] - 2025-12.25

### Fixed

+ 解决了已知的与sapi之间的兼容性问题

## [3.3.0] - 2025-11.14

### Added

+ 适配1.21.120

## [3.2.3] - 2025-11-1

### Added

+ 为假人drop/dropinv操作添加times与interval参数

### Fixed

+ 修复了删除假人时即便假人背包为空依旧提示假人背包不为空的问题

## [3.2.2] - 2025-10-19

### Added

+ 为所有授权操作添加"all"参数，用于授权全部权限

### Fixed

+ 修复假人组授权操作的bug
+ 修复了对不在线的假人使用授权操作时授权的结果没有保存的bug

## [3.2.1] - 2025-10.5

### Added

+ 添加en-US语言文件

### Fixed

+ 修复了假人在lookat时无法转身的bug

## [3.2.0] - 2025-10.4

### Added

+ 适配1.21.111

## [3.1.0] - 2025-9-26

### Added

+ 适配1.21.102

## [3.0.0] - 2025-9-26

### Added

+ 项目重构
+ 更新假人/假人组权限系统，现在假人所有的功能都有独立的权限，且可以设置公共权限

### Changed

+ 假人移动时不再锁定视角（但是寻路仍会）
+ 假人移动/寻路新增参数speed，可以调节假人的移动速度
+ 假人执行操作现在可以进行持续操作，需将需要的参数times或long设置为0
+ 现在假人数据使用json与nbt进行保存，可以直接在插件目录的data文件夹下直接查看假人数据

### Removed

+ 暂时移除假人脚本系统

### Fixed

+ 修复了假人数据概率丢失的问题

## [2.7.0] - 2025-8-26

### Added

+ 适配1.21.93

## [2.6.0] - 2025-7-26

### Added

+ 适配1.21.80

## [2.5.1] - 2025-7-22

### Fixed

+ 修复了两个可能导致文件读写失败的bug

## [2.5.0] - 2025-6-6

### Added

+ 适配1.21.70
+ 现在假人将锁定uuid，这意味着假人扔出的三叉戟不会随着假人重新上线失效

### Fixed

+ 修复了假人加载实体的问题
+ 修复了创造模式假人无法飞行的问题

## [2.4.1] - 2025-5-10

### Added

+ 新增图形用户界面（GUI），可通过命令 /sp、/sp p、/sp g、/sp p <spname : string> 和 /sp g <groupname : string> 打开
+ 新增飞行操作功能
+ 新增疾跑操作功能
+ 新增传送操作及配置文件中的allowTp参数
+ 新增自动下线功能，开启后当假人频繁死亡时会自动下线，死亡数与最短死亡间隔可通过配置文件参数autoDespawnCount和autoDespawnInterval设置
+ 新增超级管理员名单，可通过修改配置文件添加超级管理员
+ 新增配置文件可选参数maxOnlinePerPlayer
+ 现支持控制台使用/sp spawn、/sp despawn和/sp respawn命令

### Changed

+ 调整了build操作，现在是使用build指令将是单次操作而非持续操作
+ 调整了destroy操作，现在他有更加严格的检测方式
+ 移除chat与runcmd操作的interval和times可选参数
+ 调整了配置文件maxGroup参数，现用于限制个人组数量而非总的组数
+ 配置文件版本更新至2
+ 实现群组所有者与管理员的权限分级

### Fixed

+ 禁止通过假人创建新的假人
+ 修复自动加入功能导致插件报错不启动的问题，现在假人将在首个玩家加入游戏时自动加入

## [2.4.0] - 2025-5-6

### Added

+ 适配1.21.60

## [2.3.1] - 2025-5-6

### Fixed

+ 修复了自动加入失效的问题

## [2.3.0] - 2025-4-26

### Added

+ 适配1.21.50

## [2.2.2] - 2024-10-19

### Fixed

+ Fixed bad memory access (0xC0000005) when simplayer data saving or loading

## [2.2.1] - 2024-10-16

### Fixed

+ Fixed simplayer chat subcommand

## [2.2.0] - 2024-10-16

### Added

+ Added lua api `vec3.newFromRotation`
+ Added lua api `vec2.newFromDirection`

### Fixed

+ Fixed simplayer lookat
+ Fixed rotation set when spawn simplayer

### Changed

+ SimPlayerInfo api now will throw error when ptr simPlayer is null

## [2.1.2] - 2024-10-14

We skipped `v2.1.0` and `v2.1.1`. They have some serious bugs.

### Added

+ Added tryPutIntoContainer
+ Added tryGetFromContainerWithName
+ Added getFirstEmptySlot
+ Added trySwapSlotWithContainer

### Fixed

+ Fixed searchInInvWithName
+ Fixed tooth.json

## [2.0.0] - 2024-10-13

### Added

+ Added some new native api

### Changed

+ `MANAGER_VERSION` changed to `2` (warning: you cannot use old simplayer data now)
+ Changed `build` subcommand and api

## [1.0.0] - 2024-10-11

### Added

+ Move CoralFans SimulatedPlayer System from [CoralFans](https://github.com/CoralFans-Dev/CoralFans) to here
+ Added Script Arg

[2.2.2]: https://github.com/CoralFans-Dev/CFSP/compare/v2.2.1...v2.2.2
[2.2.1]: https://github.com/CoralFans-Dev/CFSP/compare/v2.2.0...v2.2.1
[2.2.0]: https://github.com/CoralFans-Dev/CFSP/compare/v2.1.2...v2.2.0
[2.1.2]: https://github.com/CoralFans-Dev/CFSP/compare/v2.0.0...v2.1.2
[2.0.0]: https://github.com/CoralFans-Dev/CFSP/compare/v1.0.0...v2.0.0
[1.0.0]: https://github.com/CoralFans-Dev/CFSP/releases/tag/v1.0.0
