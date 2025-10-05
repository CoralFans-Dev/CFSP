# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.2.1] - 2025-10.5

### Added

+ add en-US lang file

### Fixed

+ fix the bug that simulatedPlayer can't change it's body rotation when use lookat

## [3.2.0] - 2025-10.4

### Added

+ Adapt to 1.21.111

## [3.1.0] - 2025-9-26

### Added

+ Adapt to 1.21.102

## [3.0.0] - 2025-9-26

### Added

+ Project refactoring
+ Updated the permission system for simulatedPlayer/simulatedPlayer groups. All simulatedPlayer functions now have independent permissions, and public permissions can be configured.

### Changed

+ SimulatedPlayer no longer lock the camera view when moving (but navigate still does)
+ Added a speed parameter to simulatedPlayer move/navigate to adjust bot movement speed
+ SimulatedPlayer actions can now be performed continuously by setting the times or long parameter to 0
+ SimulatedPlayer data is now saved using JSON and NBT, allowing direct viewing of simulatedPlayer data in the data folder under the plugin directory

### Removed

+ Temporarily removed the simulatedPlayer script system

### Fixed

+ Fixed an issue where simulatedPlayer data could be lost randomly

## [2.7.0] - 2025-8-26

### Added

+ Adapt to 1.21.93

## [2.6.0] - 2025-7-26

### Added

+ Adapt to 1.21.80

## [2.5.1] - 2025-7-22

### Fixed

+ fix two bug may cause r/w file failed

## [2.5.0] - 2025-6-7

### Added

+ Adapt to 1.21.70
+ now simulatedPlayer will lock it's uuid. This means tridents thrown by simulated player will not be unused when simulated player rejoined to the world

### Fixed

+ fix the entities loading problem around simulated players
+ fix the bug that creative gamemode simulated player can't fly.

## [2.4.1] - 2025-5-10

### Added

+ Added GUI, you can open it with command /sp, /sp p, /sp g, /sp p <spname : string> and /sp g <groupname : string>
+ Added flying operation
+ Added sprinting operation
+ Added tp operation and it's parameter allowTp in config file
+ Added autoDespawn func, and it's parameters autoDespawnCount and autoDespawninterval in config file
+ Added supermanager list, you can change config file to add super manager
+ Added a new optional parameter maxOnlinePerPlayer in config file
+ Now you can use Command /sp spawn, /sp despawn and /sp respawn on cmd

### Changed

+ Changed the operation build, now it build once instead of building continously
+ Changed the operation destroy, now it work with more strictly inspection
+ Remove the interval and times optional parameters of the operation chat and runcmd
+ the parameter maxGroup in config file changed, now it limit personal group number insteal of total
+ Update the Config version to 2
+ Differentiated the owner and administrator of the group in terms of permissions.

### Fixed

+ Now you can't operate simulated plyaer to create new simulated player
+ Fix a bug that could make autoJoin useless, now simulated player will join when the first player join the game

## [2.4.0] - 2025-5-6

### Added

+ Adapted to 1.21.60

## [2.3.1] - 2025-5-6

### Fixed

+ Fix the bug that cfsp don't respawn when dead

## [2.3.0] - 2025-4-26

### Added

+ Adapted to 1.21.50

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
