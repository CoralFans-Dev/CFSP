# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
