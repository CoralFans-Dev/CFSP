# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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

[2.2.0]: https://github.com/CoralFans-Dev/CFSP/compare/v2.1.2...v2.2.0
[2.1.2]: https://github.com/CoralFans-Dev/CFSP/compare/v2.0.0...v2.1.2
[2.0.0]: https://github.com/CoralFans-Dev/CFSP/compare/v1.0.0...v2.0.0
[1.0.0]: https://github.com/CoralFans-Dev/CFSP/releases/tag/v1.0.0
