# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
<!-- markdownlint-disable MD024 -->

## [2.0.5] - 2025-05-04

### <!-- 1 --> 🚀 Added

- Add new type by \@TMRh20 in [#250](https://github.com/nRF24/RF24Network/pull/250)

### <!-- 4 --> 🛠️ Fixed

- Fix write to pipe by \@TMRh20 in [#248](https://github.com/nRF24/RF24Network/pull/248)
- Fix handling of radio/network overruns by \@TMRh20 in [#249](https://github.com/nRF24/RF24Network/pull/249)

### <!-- 9 --> 🗨️ Changed

- Utilize new stopListening function by \@TMRh20 in [#251](https://github.com/nRF24/RF24Network/pull/251)

[2.0.5]: https://github.com/nRF24/RF24Network/compare/v2.0.4...v2.0.5

Full commit diff: [`v2.0.4...v2.0.5`][2.0.5]

## [2.0.4] - 2025-04-19

### <!-- 3 --> 🗑️ Removed

- Fix for large payloads with NRF52 by \@TMRh20 in [#244](https://github.com/nRF24/RF24Network/pull/244)

### <!-- 9 --> 🗨️ Changed

- Bump version to v2.0.4 by \@TMRh20 in [`380efb8`](https://github.com/nRF24/RF24Network/commit/380efb83c9ee8d0455279926fd9a6944dff5f437)

[2.0.4]: https://github.com/nRF24/RF24Network/compare/v2.0.3...v2.0.4

Full commit diff: [`v2.0.3...v2.0.4`][2.0.4]

## [2.0.3] - 2025-01-10

### <!-- 4 --> 🛠️ Fixed

- Fix buffering of user payloads by \@TMRh20 in [#243](https://github.com/nRF24/RF24Network/pull/243)

### <!-- 9 --> 🗨️ Changed

- Bump version to v2.0.3 by \@2bndy5 in [`731a368`](https://github.com/nRF24/RF24Network/commit/731a36864daa72a605eb170aed8b29493ecfd6f4)

[2.0.3]: https://github.com/nRF24/RF24Network/compare/v2.0.2...v2.0.3

Full commit diff: [`v2.0.2...v2.0.3`][2.0.3]

## [2.0.2] - 2024-10-06

### <!-- 1 --> 🚀 Added

- Add NRF52x info to docs by \@TMRh20 in [#230](https://github.com/nRF24/RF24Network/pull/230)
- Add pyproject.toml by \@2bndy5 in [#234](https://github.com/nRF24/RF24Network/pull/234)

### <!-- 4 --> 🛠️ Fixed

- Add call to txStandBy() by \@TMRh20 in [#229](https://github.com/nRF24/RF24Network/pull/229)
- Fix doc typo by \@2bndy5 in [`c5b62bc`](https://github.com/nRF24/RF24Network/commit/c5b62bc3546be85455c06031f676563ec9ae4764)
- Need NUM_PIPES defined for all devices by \@TMRh20 in [#231](https://github.com/nRF24/RF24Network/pull/231)

### <!-- 8 --> 📝 Documentation

- Remove sphinx and update RTD config by \@2bndy5 in [#235](https://github.com/nRF24/RF24Network/pull/235)

### <!-- 9 --> 🗨️ Changed

- Change SERIAL_DEBUG into RF24NETWORK_DEBUG by \@2bndy5 in [#228](https://github.com/nRF24/RF24Network/pull/228)
- [CMake] default to SPIDEV driver by \@2bndy5 in [#232](https://github.com/nRF24/RF24Network/pull/232)
- Bump version to v2.0.2 by \@2bndy5 in [`fd8126a`](https://github.com/nRF24/RF24Network/commit/fd8126a9910404bdd1d0014dcf438ba9a7859d37)

[2.0.2]: https://github.com/nRF24/RF24Network/compare/v2.0.1...v2.0.2

Full commit diff: [`v2.0.1...v2.0.2`][2.0.2]

## [2.0.1] - 2024-06-11

### <!-- 1 --> 🚀 Added

- Add mentions of nrf52x by \@TMRh20 in [#221](https://github.com/nRF24/RF24Network/pull/221)

### <!-- 4 --> 🛠️ Fixed

- Fix for Multicast to level 4 by \@TMRh20 in [#227](https://github.com/nRF24/RF24Network/pull/227)

### <!-- 8 --> 📝 Documentation

- Minor doc updates by \@2bndy5 in [#225](https://github.com/nRF24/RF24Network/pull/225)

### <!-- 9 --> 🗨️ Changed

- Extra Pipes by \@TMRh20 in [#215](https://github.com/nRF24/RF24Network/pull/215)
- Update Network_Priority_RX.ino by \@TMRh20 in [#216](https://github.com/nRF24/RF24Network/pull/216)
- Modifications for 64-bit OS by \@TMRh20 in [#218](https://github.com/nRF24/RF24Network/pull/218)
- Update clang format by \@2bndy5 in [#219](https://github.com/nRF24/RF24Network/pull/219)
- Update README.md by \@TMRh20 in [`4c093cd`](https://github.com/nRF24/RF24Network/commit/4c093cd628ae69ec5f567c3099520a097db8d2b5)
- Update addressing & Title Info by \@TMRh20 in [#223](https://github.com/nRF24/RF24Network/pull/223)
- Update addressing.md by \@TMRh20 in [#224](https://github.com/nRF24/RF24Network/pull/224)
- Bump version to v2.0.1 by \@2bndy5 in [`774afe3`](https://github.com/nRF24/RF24Network/commit/774afe30d0e0d7f382960199253dc2b026a186fe)

[2.0.1]: https://github.com/nRF24/RF24Network/compare/v2.0.0...v2.0.1

Full commit diff: [`v2.0.0...v2.0.1`][2.0.1]

## [2.0.0] - 2023-06-08

### <!-- 9 --> 🗨️ Changed

- Template layer for radio HW abstraction by \@2bndy5 in [#210](https://github.com/nRF24/RF24Network/pull/210)

[2.0.0]: https://github.com/nRF24/RF24Network/compare/v1.0.18...v2.0.0

Full commit diff: [`v1.0.18...v2.0.0`][2.0.0]

## [1.0.18] - 2023-06-06

### <!-- 1 --> 🚀 Added

- Support NRF52x by \@TMRh20 in [`7a7a854`](https://github.com/nRF24/RF24Network/commit/7a7a8543443eeb1352d9bb99264d0e8c76baa624)
- Add `deploy-release` arg to PIO reusable CI call by \@2bndy5 in [`89fa59a`](https://github.com/nRF24/RF24Network/commit/89fa59a203384364492107c3b36f8e516863a903)

### <!-- 3 --> 🗑️ Removed

- Workaround for printf issue by \@TMRh20 in [#207](https://github.com/nRF24/RF24Network/pull/207)

### <!-- 4 --> 🛠️ Fixed

- Fix 200 by \@2bndy5 in [#202](https://github.com/nRF24/RF24Network/pull/202)
- Fix badge in README by \@2bndy5 in [`2a7c942`](https://github.com/nRF24/RF24Network/commit/2a7c942a66e10e3b7d0b09cd58c7b1a6427ff1b1)

### <!-- 8 --> 📝 Documentation

- Manually trigger docs CI by \@2bndy5 in [`5bdbe14`](https://github.com/nRF24/RF24Network/commit/5bdbe147fab89852dddcfc5590f57c850e4b1917)
- Doc updates and reusable CI by \@2bndy5 in [#203](https://github.com/nRF24/RF24Network/pull/203)

### <!-- 9 --> 🗨️ Changed

- Default to large max payload size by \@TMRh20 in [#196](https://github.com/nRF24/RF24Network/pull/196)
- Update for nrf52 by \@TMRh20 in [`229b886`](https://github.com/nRF24/RF24Network/commit/229b88672a4bc7ebd8c56532a5a2baf7451d0315)
- Revert NRF52 changes by \@TMRh20 in [`8c90bbe`](https://github.com/nRF24/RF24Network/commit/8c90bbe2fc5e63330599e8a8f75ed90391cf5cb1)
- Update version for release by \@TMRh20 in [`7c41ace`](https://github.com/nRF24/RF24Network/commit/7c41aceaa0b2dc7822b8a115c6390b04fe577e85)

[1.0.18]: https://github.com/nRF24/RF24Network/compare/v1.0.17...v1.0.18

Full commit diff: [`v1.0.17...v1.0.18`][1.0.18]

## [1.0.17] - 2022-07-09

### <!-- 1 --> 🚀 Added

- Add .clang-format config and adjust CI workflows by \@2bndy5 in [`d7a449a`](https://github.com/nRF24/RF24Network/commit/d7a449a05355fb1ad2222aa9cad2a9ec1fc3a6af)

### <!-- 3 --> 🗑️ Removed

- Remove old astyle config by \@2bndy5 in [`9003bc8`](https://github.com/nRF24/RF24Network/commit/9003bc8ed221146e98fbcbbdf82edc5805b06bd3)

### <!-- 4 --> 🛠️ Fixed

- Fix py wrappers linker order by \@2bndy5 in [`ebfdba2`](https://github.com/nRF24/RF24Network/commit/ebfdba27db3383777ff9ada399f3c7aa8ccd382d)
- Fix setup.py for no IRQ support by \@2bndy5 in [`ffcdf8d`](https://github.com/nRF24/RF24Network/commit/ffcdf8d9bbdb0e78163ccb8d09e4b3d559e5976c)

### <!-- 8 --> 📝 Documentation

- Various improvments to docs by \@2bndy5 in [#193](https://github.com/nRF24/RF24Network/pull/193)
- Update .readthedocs.yaml by \@2bndy5 in [`7fb9ce5`](https://github.com/nRF24/RF24Network/commit/7fb9ce573cd50b9011bf48cdc3bb2fe675292057)
- Update sphinx doc config by \@2bndy5 in [`1aacb14`](https://github.com/nRF24/RF24Network/commit/1aacb14c88d7a7b3d80b92cfc3898dd95b803f6f)

### <!-- 9 --> 🗨️ Changed

- Only run linux CI on "created release" events by \@2bndy5 in [`36a2ca7`](https://github.com/nRF24/RF24Network/commit/36a2ca7840ec48f0b2954c2ecf6a8eabdabdc7f3)
- Update actions/checkout to v2 by \@2bndy5 in [`f47339e`](https://github.com/nRF24/RF24Network/commit/f47339e327a4a5d651b1bc3af197a769c4d8f43d)
- Conditionally build examples with pigpio by \@2bndy5 in [`20c8621`](https://github.com/nRF24/RF24Network/commit/20c8621f0bf867672eae827bf381dc64779bd79b)
- Revert "conditionally build examples with pigpio" by \@2bndy5 in [`5e5af7e`](https://github.com/nRF24/RF24Network/commit/5e5af7e97e5bce2cb29f75d04a1707af3f7e9abc)
- Reduce duplicate messages in cmake output by \@2bndy5 in [`3e4404b`](https://github.com/nRF24/RF24Network/commit/3e4404b29bcb9acd269be29925246d392864cee7)
- Syntax is/was invalid because IDK Makefile syntax by \@2bndy5 in [`6854923`](https://github.com/nRF24/RF24Network/commit/685492334df807909c8031da38ebc1a05e7ebd3e)
- Attempt 3 to modify the Makefile by \@2bndy5 in [`3985205`](https://github.com/nRF24/RF24Network/commit/398520517f4ba0d0c52b3e826f07d65ca879a6b1)
- Makefile syntax doesn't like recursive var values by \@2bndy5 in [`256a204`](https://github.com/nRF24/RF24Network/commit/256a204f7e4fb808116f4e89916b760d8c43a215)
- Hardcode -lpigpio if RPi is detected in makefile by \@2bndy5 in [`21e3116`](https://github.com/nRF24/RF24Network/commit/21e3116b6ffbae81192d7a0d33e48ea9abb98843)
- (wtf) RPi not detected on my RPi4? by \@2bndy5 in [`2046b15`](https://github.com/nRF24/RF24Network/commit/2046b15f65e223c7dea8dff862ae93caa97834a6)
- Undo my changes to the Makefile by \@2bndy5 in [`2c42989`](https://github.com/nRF24/RF24Network/commit/2c429895efad8d9daefeea477ba8ed50ad792d7e)
- [cmake] build examples w/ a specified driver by \@2bndy5 in [`dcb895b`](https://github.com/nRF24/RF24Network/commit/dcb895b09b2804782332ba15caf81a5f301ec5e6)
- -DRF24_NO_IRQ when no pigpio while building lib by \@2bndy5 in [`b78096d`](https://github.com/nRF24/RF24Network/commit/b78096d6cf98a1b5d682f740752c8378e858eab6)
- Allow RF24_NO_IRQ for building examples by \@2bndy5 in [`296efb5`](https://github.com/nRF24/RF24Network/commit/296efb513b398bece1facd525e1d34b20df34b1c)
- Merge remote-tracking branch 'origin/pigpio-support' by \@TMRh20 in [`5d994b1`](https://github.com/nRF24/RF24Network/commit/5d994b1c7da01e673fe4676718438e3e1952be5f)
- Update python examples by \@2bndy5 in [`a20c632`](https://github.com/nRF24/RF24Network/commit/a20c632167179bb58d96b42f7ba5a5f597cc710f)
- Merge pull request \#194 from nRF24/update-py-examples by \@TMRh20 in [#194](https://github.com/nRF24/RF24Network/pull/194)
- Ran clang-format on all C++ sources by \@2bndy5 in [`1e79873`](https://github.com/nRF24/RF24Network/commit/1e7987334f8c1ee2d0b087401110d474c6d83dc9)
- Enable size delta reports in Arduino CI by \@2bndy5 in [`0222a34`](https://github.com/nRF24/RF24Network/commit/0222a34337b1d90cf0e1634fff1cb15b2f362219)
- Avoid duplicate builds for reported boards by \@2bndy5 in [`0d96436`](https://github.com/nRF24/RF24Network/commit/0d964366978b53554eff5337e50416381c86795e)
- Duplicate builds needed to compute size deltas by \@2bndy5 in [`eb40f2e`](https://github.com/nRF24/RF24Network/commit/eb40f2e3d3d759259d8de464137650943864bbc9)
- Revert add duplicate build jobs; use matrix params by \@2bndy5 in [`680f7f1`](https://github.com/nRF24/RF24Network/commit/680f7f1e14f5ed5599f331f6b4f09bf65508ec4a)
- Allow short case labels by \@2bndy5 in [`d7006b4`](https://github.com/nRF24/RF24Network/commit/d7006b4a445ec17834fcdf7e4dacda2d398f2a4d)
- Merge pull request \#195 from nRF24/clang-format by \@TMRh20 in [#195](https://github.com/nRF24/RF24Network/pull/195)
- Disable latex builds on RTD by \@2bndy5 in [`82b7037`](https://github.com/nRF24/RF24Network/commit/82b7037d6be259db6fba910fcbd438094809e8b9)
- Minor version bump by \@2bndy5 in [`122cb6c`](https://github.com/nRF24/RF24Network/commit/122cb6c399c577dac87ae196a9f6063e3c95164c)

[1.0.17]: https://github.com/nRF24/RF24Network/compare/v1.0.16...v1.0.17

Full commit diff: [`v1.0.16...v1.0.17`][1.0.17]

## [1.0.16] - 2021-11-17

### <!-- 1 --> 🚀 Added

- Add CMake 4 Linux; clean up tests/jamfile relics by \@2bndy5 in [`1e600d7`](https://github.com/nRF24/RF24Network/commit/1e600d7927e5a0c9775c3ccffb5fd7108f513bc2)
- Add CMake user preset json to ignore by \@2bndy5 in [`733177e`](https://github.com/nRF24/RF24Network/commit/733177e9fefc78e9ce3fb9449be836476b8eae6d)
- Add if(!radio.begin()) to linux examples; fmt code by \@2bndy5 in [`74f446a`](https://github.com/nRF24/RF24Network/commit/74f446a5fcbc9ed65432092cb99ec3309aa8d664)
- Add note about MAIN_BUFFER_SIZE by \@2bndy5 in [`2658760`](https://github.com/nRF24/RF24Network/commit/2658760a1850e90f7a7ed84fe2ad768dd811924f)
- Add to last commit for completeness by \@2bndy5 in [`d42fe5e`](https://github.com/nRF24/RF24Network/commit/d42fe5e52621b062755aff6659acc67c6ba0ff5d)
- Add newer begin() to py wrapper by \@2bndy5 in [`20977e0`](https://github.com/nRF24/RF24Network/commit/20977e0f6bb166bd36b2699f119e154afb3c28de)
- Add comments about ATTiny defaults by \@2bndy5 in [`8892524`](https://github.com/nRF24/RF24Network/commit/889252468266ea6f2e08ada4ed86952130962ed5)
- Add PicoSDK CI workflow by \@2bndy5 in [`2921124`](https://github.com/nRF24/RF24Network/commit/2921124ca655e00595afdd27f1b3215a96b4822d)
- Merge pull request \#176 from nRF24/CMake-4-Linux by \@TMRh20 in [#176](https://github.com/nRF24/RF24Network/pull/176)
- Add link to blog for zigbee vs nrf by \@TMRh20 in [`3b1352c`](https://github.com/nRF24/RF24Network/commit/3b1352c9aa82263afbfbc60bb78ed38f2a717565)
- Add multicast description by \@TMRh20 in [`6dcd5f8`](https://github.com/nRF24/RF24Network/commit/6dcd5f8ec3c9eb38ec738a42c81be1a86cbbb83d)

### <!-- 2 --> 🚫 Deprecated

- Examples don't use deprecated begin(channel, addr) by \@2bndy5 in [`5b8a9bf`](https://github.com/nRF24/RF24Network/commit/5b8a9bf902f4dd7a9a496a65be777a4e8c83b80b)

### <!-- 3 --> 🗑️ Removed

- Remove useless line from docs code snippet by \@2bndy5 in [`f01c4cd`](https://github.com/nRF24/RF24Network/commit/f01c4cdf89415b073e6f4a12e8c01845898e4a6e)
- Remove useless boost.py overload macros by \@2bndy5 in [`49e04db`](https://github.com/nRF24/RF24Network/commit/49e04db208d046d622f0042155f2210672d06f6e)
- [no ci] remove doc artifact about new flag by \@2bndy5 in [`d03d6ee`](https://github.com/nRF24/RF24Network/commit/d03d6eec6d7c269e11c03bdd545a0b90fc73f760)

### <!-- 4 --> 🛠️ Fixed

- Fix bad link in docs; uniform SERIAL_DEBUG* calls by \@2bndy5 in [`95b5e09`](https://github.com/nRF24/RF24Network/commit/95b5e094bb5e3993bbae10dd85fbfca491fa04b9)
- Fix warnings about %lu by \@2bndy5 in [`2df98c8`](https://github.com/nRF24/RF24Network/commit/2df98c8d9c80f24e08eda222f2afbaf5a884507b)
- Fix problem in last commit by \@2bndy5 in [`ae8ec89`](https://github.com/nRF24/RF24Network/commit/ae8ec89cdea8c59fd0dd2adeded6cddfa3b1bca8)
- Fix url from last commit by \@2bndy5 in [`f33bacf`](https://github.com/nRF24/RF24Network/commit/f33bacf13809a47b14baefddc00293a2588a69e0)
- Fix formatting in an example by \@2bndy5 in [`8acdf8e`](https://github.com/nRF24/RF24Network/commit/8acdf8ee32d39a7cf2a09cf88fd9226d7da96535)
- Fix image display by \@TMRh20 in [`d9e7909`](https://github.com/nRF24/RF24Network/commit/d9e79090ef21e4952f3590c5771d69819ee9cb17)

### <!-- 8 --> 📝 Documentation

- Enhance docs by \@2bndy5 in [`eb2a6ea`](https://github.com/nRF24/RF24Network/commit/eb2a6ead130f306d1c8a1a8c73a469a56813528d)
- Update some docs; use macro instead of def'd value by \@2bndy5 in [`9f2e62b`](https://github.com/nRF24/RF24Network/commit/9f2e62b16ddb63308773bde1a39d3078a73f2014)
- [docs] expand on use of frame_buffer by \@2bndy5 in [`86c0d3b`](https://github.com/nRF24/RF24Network/commit/86c0d3b778cefafdc6daab1ac047f64f64e97c53)
- Ammend multicast* docs by \@2bndy5 in [`321ce84`](https://github.com/nRF24/RF24Network/commit/321ce84be6817f34faadea56c3a0aaa3b9eb7c89)
- Docs ready for RTD hosting by \@2bndy5 in [`9d92dbd`](https://github.com/nRF24/RF24Network/commit/9d92dbdaa7b1b8b27f39181dde58fe4ca0a75707)
- Use URL blob for new img in docs main pg by \@2bndy5 in [`0df0abf`](https://github.com/nRF24/RF24Network/commit/0df0abf178aa84868db97e34ad226343ea59ac39)
- Sphinx docs conf.py gets lib ver from json by \@2bndy5 in [`dbaf785`](https://github.com/nRF24/RF24Network/commit/dbaf785f87eb8ff630f528aad853287f3e7f7f63)
- More docs updates by \@2bndy5 in [`55e9032`](https://github.com/nRF24/RF24Network/commit/55e9032c2f829a5bb24ee6fe49c00cc977b4e2db)
- More doc updates by \@2bndy5 in [`0c41d05`](https://github.com/nRF24/RF24Network/commit/0c41d057d405c2c13bc4dd862a837ccfd18f5a19)
- Smal docs update by \@2bndy5 in [`2a29943`](https://github.com/nRF24/RF24Network/commit/2a299434e2c2281b99ff8ca3a192d1bc47ba7fe9)
- [docs] fix multiline API signatures by \@2bndy5 in [`2498529`](https://github.com/nRF24/RF24Network/commit/24985294a97b111ff02ad03198f837db6eff70ef)

### <!-- 9 --> 🗨️ Changed

- Initial CMake files for pico-sdk by \@kripton in [#174](https://github.com/nRF24/RF24Network/pull/174)
- This builds locally by \@2bndy5 in [`cfe3c5c`](https://github.com/nRF24/RF24Network/commit/cfe3c5c4892c44bf5642317db3074f692b3e6b56)
- Avoid duplicate workflows when PR is open by \@2bndy5 in [`47da072`](https://github.com/nRF24/RF24Network/commit/47da072201ae04a04aa8ea2c7579dda0a4aaa952)
- Conform code fmt; limit CI triggers by \@2bndy5 in [`2ac9a46`](https://github.com/nRF24/RF24Network/commit/2ac9a4601234ea96b167bd30eca2ed80a7f62a01)
- Missed a fmt inconsistency by \@2bndy5 in [`4c87a28`](https://github.com/nRF24/RF24Network/commit/4c87a28741f648052cbd0afe28cfcd2cd4109a66)
- Update CI scripts by \@2bndy5 in [`01f2200`](https://github.com/nRF24/RF24Network/commit/01f2200e647079dfd50544b7f0e87c8d862d2734)
- [no ci] rename uploaded artifact by \@2bndy5 in [`3ad612a`](https://github.com/nRF24/RF24Network/commit/3ad612a3421b1aa0ed7500196f1f4963884efb81)
- Default BUILD_TYPE to Release by \@2bndy5 in [`49827f1`](https://github.com/nRF24/RF24Network/commit/49827f10e078610d3d59c0925c0c743bb4a45041)
- Update CONTRIBUTING.md by \@2bndy5 in [`dc9baae`](https://github.com/nRF24/RF24Network/commit/dc9baaef95dd6f123830c989ecd734d264a8682e)
- Manually trigger CI workflows by \@2bndy5 in [`58f116a`](https://github.com/nRF24/RF24Network/commit/58f116aa58054eae11a3b50c3187a90abfefc897)
- Gimme them badges by \@2bndy5 in [`634f7a5`](https://github.com/nRF24/RF24Network/commit/634f7a5c580c024af955da2e8c998e79b19a899f)
- Run new PlatformIO CI by \@2bndy5 in [`3cdb937`](https://github.com/nRF24/RF24Network/commit/3cdb93714942a18d235204826d35eb9dd5fd6185)
- PIO CI needs to know about dependencies by \@2bndy5 in [`6704178`](https://github.com/nRF24/RF24Network/commit/6704178850c47fb6cc17927075bb8883d543f025)
- Force install deps in PIO workflow by \@2bndy5 in [`4dd3c19`](https://github.com/nRF24/RF24Network/commit/4dd3c19612d501464e15f70d8a8c6c1fae768be2)
- Pio lib install now installs deps from json by \@2bndy5 in [`9233555`](https://github.com/nRF24/RF24Network/commit/92335554b752738da2b84d76a524c678b807918f)
- [library.json] update excludes feild by \@2bndy5 in [`8466197`](https://github.com/nRF24/RF24Network/commit/84661973dcf1aa01607e501d0231614261eeb4c6)
- Polish library.json by \@2bndy5 in [`6eec391`](https://github.com/nRF24/RF24Network/commit/6eec391185ce8b88149228c3fb4cf548b5044c66)
- Exclude linux stuff from PIO & validate lib.json by \@2bndy5 in [`f05ebda`](https://github.com/nRF24/RF24Network/commit/f05ebda6b239258c1b868f8ffb6d2ca0a5bb7698)
- [PIO CI] rename artifact by \@2bndy5 in [`ab81be6`](https://github.com/nRF24/RF24Network/commit/ab81be6e8a293d76e397a57953e53ca96ebdd863)
- Exclude more useless stuff from PIO pkg by \@2bndy5 in [`7986d7a`](https://github.com/nRF24/RF24Network/commit/7986d7a764aebb504a87298a70866685fb60c294)
- Testing examples on qtpy (atsamd21) by \@2bndy5 in [`795ae09`](https://github.com/nRF24/RF24Network/commit/795ae0948dc2a0e2d853d01052245bab3b129f90)
- Consolidate duplicate includes by \@2bndy5 in [`23ef6da`](https://github.com/nRF24/RF24Network/commit/23ef6da38c322c61916fffd6cede38bf2cbf96bc)
- Use CMake CLI to change RF24Network_config macros by \@2bndy5 in [`2d6780e`](https://github.com/nRF24/RF24Network/commit/2d6780e3d735db9c7c97e68c989c1faec9f3f8e1)
- Trigger arduino CI by \@2bndy5 in [`adfb1ad`](https://github.com/nRF24/RF24Network/commit/adfb1ad8efe551e71c92418b1ef290731b6964f3)
- Uniform examples by \@2bndy5 in [`bcf5471`](https://github.com/nRF24/RF24Network/commit/bcf5471f4b40a0ae2f2443877f6a20d1280759fb)
- Use macro name instead of macro value by \@2bndy5 in [`251255e`](https://github.com/nRF24/RF24Network/commit/251255e7d32fba40dca8b41fcf5151198498ca99)
- Solution for \#179 by \@2bndy5 in [`72533fe`](https://github.com/nRF24/RF24Network/commit/72533fe52a1b08b06ca5caaaf4a4ec104619e967)
- Use %iu instead of %ui by \@2bndy5 in [`3f48c4f`](https://github.com/nRF24/RF24Network/commit/3f48c4f96019ccaf4d7e985a0b64ba03cdac5530)
- Use just %u by \@2bndy5 in [`9926e15`](https://github.com/nRF24/RF24Network/commit/9926e150e4ed5007b87f7b1bc0bb8d445d8931ca)
- [py wrapper] augment setup.py by \@2bndy5 in [`58c69b3`](https://github.com/nRF24/RF24Network/commit/58c69b3d015713bbe32f634142f4047443959ca9)
- Py pkgs from apt are very out-of-date by \@2bndy5 in [`790e441`](https://github.com/nRF24/RF24Network/commit/790e441159a54401a3a262d33da5c399684a9ecd)
- Let maxlen param be optional by \@2bndy5 in [`bd01c62`](https://github.com/nRF24/RF24Network/commit/bd01c625e7ffc43806298c3521e20500d7be9788)
- + temp.py for testing; adjust read/peek overloads by \@2bndy5 in [`1195151`](https://github.com/nRF24/RF24Network/commit/119515186da700944910e547b6f0b68ca09551ef)
- Working solution for \#181 by \@2bndy5 in [`aaa78a9`](https://github.com/nRF24/RF24Network/commit/aaa78a9560c8ba740e33ed8fe99d88413807c123)
- Explain NETWORK_ACK better by \@2bndy5 in [`ca156a1`](https://github.com/nRF24/RF24Network/commit/ca156a16a188162afcfdc0b210b13784f78fdaf8)
- Use MESH_SLOW_ADDR_RESPONSE to slow POLL responses by \@2bndy5 in [`f301912`](https://github.com/nRF24/RF24Network/commit/f301912ec12bbbad6c6b7d57ff62b44d65c8e983)
- Move SLOW_RESPONSE macro to Network lvl by \@2bndy5 in [`b7d052d`](https://github.com/nRF24/RF24Network/commit/b7d052d2442758355a8d1cdb4cf10c068596d9df)
- Revert SLOW_ADDR_POLL_RESPONSE by \@2bndy5 in [`763ed5a`](https://github.com/nRF24/RF24Network/commit/763ed5aa6ff5c4107cff325ea26da790bbbfe3fe)
- Introduce FLAG_FIRST_FRAG to avoid repetitive SPI calls by \@2bndy5 in [`d6ecb50`](https://github.com/nRF24/RF24Network/commit/d6ecb5008cc8caf85dc245b34468c656c2bdf8ee)
- Revert "revert SLOW_ADDR_POLL_RESPONSE" by \@2bndy5 in [`5b9b5e5`](https://github.com/nRF24/RF24Network/commit/5b9b5e5a8fa89f56388013c0ff67cd4df06218aa)
- Testing optimizational solutions on Linux by \@2bndy5 in [`4ea2952`](https://github.com/nRF24/RF24Network/commit/4ea29525ebce172bc3010d5cb1d985483dd765f5)
- Use message type instead of a new flag by \@2bndy5 in [`859f9c4`](https://github.com/nRF24/RF24Network/commit/859f9c4a329d39cfda6fdc94e1aa79ab540504e1)
- Update python examples by \@2bndy5 in [`33d26fe`](https://github.com/nRF24/RF24Network/commit/33d26fed9b72571b70ffc8e3041b1facd43d640a)
- Resolves nRF24/RF24Mesh\#197 by \@2bndy5 in [`fde923a`](https://github.com/nRF24/RF24Network/commit/fde923a068d1db010e6147458735c1d25d1bb545)
- Resolves \#185 by \@2bndy5 in [`1794450`](https://github.com/nRF24/RF24Network/commit/179445054e500ebb16b8c9fab08ceaff3883bb15)
- Relative CMakeLists.txt by \@2bndy5 in [`8b4dd69`](https://github.com/nRF24/RF24Network/commit/8b4dd695a3d2b7a3de91de69eadcf9f8f3fa12ae)
- Satisfy \#187 by \@2bndy5 in [`936d564`](https://github.com/nRF24/RF24Network/commit/936d56445d22ea401982022d93746d788b8c6202)
- [CMake] distinguish project_option/warnings by \@2bndy5 in [`50b5be2`](https://github.com/nRF24/RF24Network/commit/50b5be247da60ae4dad2368ef19bd91b313bef3d)
- Allow pybind11 to build in source by \@2bndy5 in [`149ebcd`](https://github.com/nRF24/RF24Network/commit/149ebcd74ba8d9a45a17efb4847d2c55371782d2)
- Satisfy \#186 by \@2bndy5 in [`553b1a5`](https://github.com/nRF24/RF24Network/commit/553b1a59a7f4232dbd0b45d6c116ff862bc8d321)
- Working blindly with GH runner's FS by \@2bndy5 in [`5b6b2e0`](https://github.com/nRF24/RF24Network/commit/5b6b2e0dc7cd33708fa2c724c1e70f36c093cf18)
- Checkout RF24 lib's rp2xxx branch by \@2bndy5 in [`0762837`](https://github.com/nRF24/RF24Network/commit/0762837d8b29e4a36b1221d28e9cc130c7f947c8)
- LOL. checkout the actual RF24 repo by \@2bndy5 in [`279d9f2`](https://github.com/nRF24/RF24Network/commit/279d9f23c9a930e42defb10396b44db6b9586106)
- Save the artifacts from proper path by \@2bndy5 in [`739d366`](https://github.com/nRF24/RF24Network/commit/739d36697c0b7aa03c72556f6675fb7e72dc75b0)
- [no ci] gimme that badge by \@2bndy5 in [`13b6fb0`](https://github.com/nRF24/RF24Network/commit/13b6fb06218b5f3db2b71901ae71304432d089dc)
- Update root CMakeLists.txt by \@2bndy5 in [`d275379`](https://github.com/nRF24/RF24Network/commit/d2753790600983f0914738c864ee2648334efd62)
- Introduce undefined macro USE_RF24_LIB_SRC by \@2bndy5 in [`2a36d4b`](https://github.com/nRF24/RF24Network/commit/2a36d4b6242faa92ffab66d2d6685f8185afa1fb)
- Further the utility of USE_RF24_LIB_SRC macro by \@2bndy5 in [`88645ac`](https://github.com/nRF24/RF24Network/commit/88645ac51cb07eb5af17af40fad2eca810460942)
- [CMakeLists.txt] fix last commit by \@2bndy5 in [`6ff8bdd`](https://github.com/nRF24/RF24Network/commit/6ff8bddeb133a0942f784e9331deedb95b8cbacc)
- Declare failures() if  defined ENABLE_NET_STATS by \@2bndy5 in [`b221759`](https://github.com/nRF24/RF24Network/commit/b22175973c4279e5a48fe4987bc93a80418d70a9)
- Auto-publish new releases to PIO by \@2bndy5 in [`3e0cee1`](https://github.com/nRF24/RF24Network/commit/3e0cee11d8ad8e90d751faf8608159ab50602bf4)
- Use --non-interactive option in PIO CI by \@2bndy5 in [`d5e0e6b`](https://github.com/nRF24/RF24Network/commit/d5e0e6ba8a1c500eb59dcf4ee3748b9a8c04921b)
- Linux CI uses RF24 master branch by \@2bndy5 in [`97d3679`](https://github.com/nRF24/RF24Network/commit/97d3679483c404c13aa1cf47bb3bf1fa53c7bba6)
- Typo in linux CI triggers' paths by \@2bndy5 in [`7653422`](https://github.com/nRF24/RF24Network/commit/7653422af8c7d51b639eae2d484f05887aa4f634)
- Reviewed pico examples by \@2bndy5 in [`110b22a`](https://github.com/nRF24/RF24Network/commit/110b22a91b57dd32acb50a3229bd2f9814e84f89)
- Gimme that RTD badge by \@2bndy5 in [`81b6ab9`](https://github.com/nRF24/RF24Network/commit/81b6ab96644bf7afe667724ef2a2baf28957f9a7)
- Max net lvl is 4 (not 3) by \@2bndy5 in [`6a01eba`](https://github.com/nRF24/RF24Network/commit/6a01ebac5cc4ceba4097e19ca79961aa511ed2eb)
- Merge branch 'master' into CMake-4-Linux by \@TMRh20 in [`1b0fabe`](https://github.com/nRF24/RF24Network/commit/1b0fabe22df5611b4fef80df25d46ac93338cbff)

[1.0.16]: https://github.com/nRF24/RF24Network/compare/v1.0.15...v1.0.16

Full commit diff: [`v1.0.15...v1.0.16`][1.0.16]

## [1.0.15] - 2021-04-13

### <!-- 1 --> 🚀 Added

- Add RF24 as dependency lib by \@2bndy5 in [`d9d6f16`](https://github.com/nRF24/RF24Network/commit/d9d6f16ca357b5208905c7c87775172b54b0d00c)
- Add linux CI workflow by \@2bndy5 in [`5ae85a1`](https://github.com/nRF24/RF24Network/commit/5ae85a19336050798fb32f48dd21ea1a727318a4)

### <!-- 3 --> 🗑️ Removed

- Remove synch.*; fmt examples; (+) Arduino CI by \@2bndy5 in [`876762b`](https://github.com/nRF24/RF24Network/commit/876762b0f1c7d3342c0b416e7de0486e2a9cd364)

### <!-- 4 --> 🛠️ Fixed

- Fix unmatched parenthesis by \@2bndy5 in [`6eb8629`](https://github.com/nRF24/RF24Network/commit/6eb862957b89038ca38d94b9a9d1c701bd638991)

### <!-- 8 --> 📝 Documentation

- Docs abstraction & code formating; update python examples by \@2bndy5 in [#170](https://github.com/nRF24/RF24Network/pull/170)

### <!-- 9 --> 🗨️ Changed

- Isolate sleep to avr only by \@2bndy5 in [`e655b37`](https://github.com/nRF24/RF24Network/commit/e655b37ba236c4dc9881ee2fdecb62dabad682a1)
- Define ENABLE_SLEEP_MODE for sleep example by \@2bndy5 in [`6c89b65`](https://github.com/nRF24/RF24Network/commit/6c89b6584ad822a31af242398638517dd39c8c1a)
- Just don't build the sleep example (need advice) by \@2bndy5 in [`1c8c61f`](https://github.com/nRF24/RF24Network/commit/1c8c61f55682d5ceb7a4f88c8c105b0e4c081b66)
- Skip any example that includes avr/pgmspace by \@2bndy5 in [`022c797`](https://github.com/nRF24/RF24Network/commit/022c797c9dd11b0896f8f5c5379a00daacb84a08)
- Bad yml workflow structure by \@2bndy5 in [`aeb813e`](https://github.com/nRF24/RF24Network/commit/aeb813e2956b809579151c47b7ff4e4e82db9adf)
- Fetch all history on checkout action by \@2bndy5 in [`9ae9449`](https://github.com/nRF24/RF24Network/commit/9ae94493083a584b7879a2ee8ce630c33a9e5521)
- Clone RF24 directly from CLI (no checkout action) by \@2bndy5 in [`551c350`](https://github.com/nRF24/RF24Network/commit/551c3505bdf591f981eaf2257d8628acbb6afeac)
- Display contents of usr/local/lib & include/rf24 by \@2bndy5 in [`7fc71ae`](https://github.com/nRF24/RF24Network/commit/7fc71ae8153697024feda2fd412ff176c9b1f938)
- Typo by \@2bndy5 in [`3b1268c`](https://github.com/nRF24/RF24Network/commit/3b1268c969a67f09133d07afb01ef65dab089791)
- How is librf24-bcm.so incompatible? by \@2bndy5 in [`43f4197`](https://github.com/nRF24/RF24Network/commit/43f41971d0b9bff4205592eb8658e6465eda9ea1)
- File root to symlink by \@2bndy5 in [`bd907d4`](https://github.com/nRF24/RF24Network/commit/bd907d4fa2617b6bcdcc24db7346b6bd105f38c0)
- Abandon linux build CI for now by \@2bndy5 in [`ef0fd5b`](https://github.com/nRF24/RF24Network/commit/ef0fd5b439a3232663ad838b185bf9f1c3cbd279)
- Oops (-) AStyle.exe & ignore *.exe by \@2bndy5 in [`355b88a`](https://github.com/nRF24/RF24Network/commit/355b88a8fe0a6225e9e5e5e3e25794cb20d6ee2a)
- Merge pull request \#166 from nRF24/add-arduino-ci by \@TMRh20 in [#166](https://github.com/nRF24/RF24Network/pull/166)
- Make most \#defines in RF24Network_config.h conditional so they can be set from project code or compiler command line. by \@kripton in [#171](https://github.com/nRF24/RF24Network/pull/171)
- Version bump (patch+1) by \@2bndy5 in [`0afb14d`](https://github.com/nRF24/RF24Network/commit/0afb14d529a2758098eb99b732d0e6de4540310b)

[1.0.15]: https://github.com/nRF24/RF24Network/compare/v1.0.14...v1.0.15

Full commit diff: [`v1.0.14...v1.0.15`][1.0.15]

## New Contributors
* \@kripton made their first contribution in [#171](https://github.com/nRF24/RF24Network/pull/171)
## [1.0.14] - 2020-12-14

### <!-- 3 --> 🗑️ Removed

- Remove dual head radio stuff by \@TMRh20 in [`69c0e91`](https://github.com/nRF24/RF24Network/commit/69c0e91bda2ff44466c9df016cbe291a37f273ff)
- Remove redundant call to available() by \@TMRh20 in [`23d7fc4`](https://github.com/nRF24/RF24Network/commit/23d7fc4cb71e576acc2d6379c97505d5e727d652)

### <!-- 4 --> 🛠️ Fixed

- Fix warnings; can't do anything about c'tor params by \@2bndy5 in [`054a8ea`](https://github.com/nRF24/RF24Network/commit/054a8ea719bcb0b88ef244add5c69611cb55d355)
- Fix missing images by \@2bndy5 in [`b4e68d0`](https://github.com/nRF24/RF24Network/commit/b4e68d03f6aa406c521383ac1e997ab630abbe08)

### <!-- 8 --> 📝 Documentation

- Redirect doc links to nRF24 org by \@2bndy5 in [`bea7a94`](https://github.com/nRF24/RF24Network/commit/bea7a94a7d2fb58ab54b732b5f70cf4001390cb2)
- Merge pull request \#162 from 2bndy5/master by \@TMRh20 in [#162](https://github.com/nRF24/RF24Network/pull/162)

### <!-- 9 --> 🗨️ Changed

- Edit doxyfile OUTPUT_DIR & add workflow by \@2bndy5 in [`3f1e75a`](https://github.com/nRF24/RF24Network/commit/3f1e75a0babd359b829a876fa3a9d223f8fc52b4)
- Format doxygen css file & include latest changes by \@2bndy5 in [`51d72ba`](https://github.com/nRF24/RF24Network/commit/51d72ba8730703cfe1d9478608bc814844d1b4c5)
- Only run doxygen on master branch by \@2bndy5 in [`4569034`](https://github.com/nRF24/RF24Network/commit/4569034b28ea89e8adeba989f14f2183a4f5e2f9)
- Merge pull request \#163 from 2bndy5/master by \@2bndy5 in [#163](https://github.com/nRF24/RF24Network/pull/163)
- Simplify update() function by \@TMRh20 in [`597e7d2`](https://github.com/nRF24/RF24Network/commit/597e7d22525578dd42b6126e64d69a2f7674fe35)
- Update library.json by \@TMRh20 in [`78d1ff5`](https://github.com/nRF24/RF24Network/commit/78d1ff5f8d362ac9a44015638b981367c24837e1)
- Update library.properties by \@TMRh20 in [`710d1a5`](https://github.com/nRF24/RF24Network/commit/710d1a5dc6052ec6a15b4049bcbbfdc827c7e31b)

[1.0.14]: https://github.com/nRF24/RF24Network/compare/v1.0.13...v1.0.14

Full commit diff: [`v1.0.13...v1.0.14`][1.0.14]

## New Contributors
* \@2bndy5 made their first contribution in [#163](https://github.com/nRF24/RF24Network/pull/163)
## [1.0.13] - 2020-08-30

### <!-- 9 --> 🗨️ Changed

- Bugfix affecting RF24Mesh by \@TMRh20 in [`610d237`](https://github.com/nRF24/RF24Network/commit/610d237a8b902f63ad5a015ee19960f2543b5f1a)
- Update library.properties by \@TMRh20 in [`dbeecc9`](https://github.com/nRF24/RF24Network/commit/dbeecc9925ca0e14efb755706d09f30b84726448)
- Update library.json by \@TMRh20 in [`55a6eef`](https://github.com/nRF24/RF24Network/commit/55a6eeff29a982cd03837799fc48ddde362f74d8)

[1.0.13]: https://github.com/nRF24/RF24Network/compare/v1.0.12...v1.0.13

Full commit diff: [`v1.0.12...v1.0.13`][1.0.13]

## [1.0.12] - 2020-08-17

### <!-- 3 --> 🗑️ Removed

- Allow any size MAX_PAYLOAD_SIZE by \@TMRh20 in [`3474b15`](https://github.com/nRF24/RF24Network/commit/3474b150ed3c32b4202255544cd6433d77e384b9)
- Bug fix & cleanup by \@TMRh20 in [`17120b3`](https://github.com/nRF24/RF24Network/commit/17120b30e522e3317b96d83aae6d17666ec2fb3a)
- Bug fix & adjustment by \@TMRh20 in [`e5c6d5d`](https://github.com/nRF24/RF24Network/commit/e5c6d5da80a624be836b441e3bf1cd53047a8ddb)

### <!-- 4 --> 🛠️ Fixed

- Don't rtrn hdr type for routed payloads by \@TMRh20 in [`1e261b7`](https://github.com/nRF24/RF24Network/commit/1e261b7de5a05b1aa1696c6919fb3ec7f79aeab6)

### <!-- 9 --> 🗨️ Changed

- Update for release by \@TMRh20 in [`86f4955`](https://github.com/nRF24/RF24Network/commit/86f49559dd6b4cb07e66e551b71f651752694228)

[1.0.12]: https://github.com/nRF24/RF24Network/compare/v1.0.11...v1.0.12

Full commit diff: [`v1.0.11...v1.0.12`][1.0.12]

## [1.0.11] - 2020-08-08

### <!-- 3 --> 🗑️ Removed

- General cleanup & fixes by \@TMRh20 in [`75348f1`](https://github.com/nRF24/RF24Network/commit/75348f1cf0cfb3f46a00e48523321f52f80122bd)
- Remove warings by \@TMRh20 in [`e9a68a2`](https://github.com/nRF24/RF24Network/commit/e9a68a26e9986881b2da75881204b6462f31c87f)
- Remove incoming holds by \@TMRh20 in [`a2db440`](https://github.com/nRF24/RF24Network/commit/a2db440fcf9983162b9c83ff277187d530719855)
- Bit of a clean up by \@TMRh20 in [`0aa83e0`](https://github.com/nRF24/RF24Network/commit/0aa83e0be6f34b607104da9100df312b0b228c4b)

### <!-- 4 --> 🛠️ Fixed

- Fix config for ATTiny by \@TMRh20 in [`8df2ed9`](https://github.com/nRF24/RF24Network/commit/8df2ed9984e5a0d76f25f3875f6f1b798a5d0a6d)

### <!-- 8 --> 📝 Documentation

- Update readme to just point to the official docs by \@TMRh20 in [`f32e2b1`](https://github.com/nRF24/RF24Network/commit/f32e2b183a69cedb7d6917b389c68660b256df5e)

### <!-- 9 --> 🗨️ Changed

- Removal of BCM2835 constants by \@TMRh20 in [`2d75981`](https://github.com/nRF24/RF24Network/commit/2d759814fe61cbbf4f94adda4d5eb535e99de2f2)
- Update for release & architectures by \@TMRh20 in [`f9d23b0`](https://github.com/nRF24/RF24Network/commit/f9d23b0f36274e1b233113787597ab9bd7ab3181)
- Update for release by \@TMRh20 in [`293c55e`](https://github.com/nRF24/RF24Network/commit/293c55e748d588714dbafa0203bb8f7554a3011e)

[1.0.11]: https://github.com/nRF24/RF24Network/compare/v1.0.10...v1.0.11

Full commit diff: [`v1.0.10...v1.0.11`][1.0.11]

## [1.0.10] - 2020-07-16

### <!-- 1 --> 🚀 Added

- Add detection of certain failures by \@TMRh20 in [`4a40d32`](https://github.com/nRF24/RF24Network/commit/4a40d32836929ed9c5249952c631a71f71f6f474)
- Add 2 new examples by \@TMRh20 in [`c1cb484`](https://github.com/nRF24/RF24Network/commit/c1cb484192588858723f52515cb4a8140ff3a96f)
- Add examples, update docs by \@TMRh20 in [`c8cb7fa`](https://github.com/nRF24/RF24Network/commit/c8cb7fa3d87f6271e7a267ca5b4b54233e7898a4)

### <!-- 9 --> 🗨️ Changed

- Update library.properties by \@TMRh20 in [`7628187`](https://github.com/nRF24/RF24Network/commit/7628187362ebabcb90cd9ed115eb282706007322)
- Update version for release by \@TMRh20 in [`4023933`](https://github.com/nRF24/RF24Network/commit/402393314a86deefc135d89f3f8690c81790a409)

[1.0.10]: https://github.com/nRF24/RF24Network/compare/v1.0.9...v1.0.10

Full commit diff: [`v1.0.9...v1.0.10`][1.0.10]

## [1.0.9] - 2019-05-08

### <!-- 1 --> 🚀 Added

- Added packet peek functionality by \@Avamander in [#106](https://github.com/nRF24/RF24Network/pull/106)
- Adds define NETWORK_DEFAULT_ADDRESS by \@MAKOMO in [`1177ebd`](https://github.com/nRF24/RF24Network/commit/1177ebd8f868c800c42182839ad543c9108c3f58)
- Adds delay to avoid clashes on multicastRelay by \@MAKOMO in [`394286d`](https://github.com/nRF24/RF24Network/commit/394286da69dfe9a5f7dae92a080912bc2ccd9e67)
- Adding overload peek function for the expanded peek functionality. by \@cnlangness in [`2a8234f`](https://github.com/nRF24/RF24Network/commit/2a8234f4616465610cedae596cc7dc4183c61450)
- Adding additional overload items. by \@cnlangness in [`d0bdb38`](https://github.com/nRF24/RF24Network/commit/d0bdb3879a56dd797020f4c5e706ef21f9ab5be6)
- Adding missing length definition. by \@cnlangness in [`c939062`](https://github.com/nRF24/RF24Network/commit/c9390623aa49b56a30f8abf70b2bc4e74835794d)
- Adding type definition of a tuple for peek. by \@cnlangness in [`eae724f`](https://github.com/nRF24/RF24Network/commit/eae724fc0912024550bbbd36b4be2dacda237050)
- Add timeout to update() while loop by \@TMRh20 in [`df7dab1`](https://github.com/nRF24/RF24Network/commit/df7dab1f9bd10cbde7c6272ded85e8bf0820fc78)

### <!-- 3 --> 🗑️ Removed

- Redefinition of returnVal removed by \@wilmsn in [`460bf34`](https://github.com/nRF24/RF24Network/commit/460bf34c1ecb172074da25ac7141298d889223ba)
- Merge pull request \#125 from wilmsn/patch-1 by \@TMRh20 in [#125](https://github.com/nRF24/RF24Network/pull/125)
- Edit docs: max # of nodes, remove dual head ref by \@TMRh20 in [`a35ce88`](https://github.com/nRF24/RF24Network/commit/a35ce88686988247aeee0cb96b97023d8bde83f8)

### <!-- 4 --> 🛠️ Fixed

- Fixed dependecy issue by \@ivankravets in [`0656885`](https://github.com/nRF24/RF24Network/commit/06568851fc3ce61d4c48b1607dd0aa1449bf0a4f)
- Fixes position of NETWORK_DEFAULT_ADDRESS define by \@MAKOMO in [`4f4b239`](https://github.com/nRF24/RF24Network/commit/4f4b239840fcab860d05ee273339662bd94b613a)
- Fix/adds debug messages and tightens multi-cast relay delay by \@MAKOMO in [`840c856`](https://github.com/nRF24/RF24Network/commit/840c8569f55143220c3b666718b94c986cb01953)

### <!-- 9 --> 🗨️ Changed

- Create LICENSE by \@Avamander in [`2292755`](https://github.com/nRF24/RF24Network/commit/2292755ecb3f7912cf69bae7618c48f34ea612fe)
- Update library.properties by \@Avamander in [`a2ce4ae`](https://github.com/nRF24/RF24Network/commit/a2ce4aee1f94f742018b5b4d50665d9cbfb360b7)
- Update version to 1.0.8 by \@ivankravets in [#121](https://github.com/nRF24/RF24Network/pull/121)
- Update library.json by \@Avamander in [`2e11c3b`](https://github.com/nRF24/RF24Network/commit/2e11c3b66b727a7d1864a02f7fb812437b2fdb3c)
- Merge pull request \#122 from ivankravets/patch-3 by \@TMRh20 in [#122](https://github.com/nRF24/RF24Network/pull/122)
- Comment RPi incompatible debug messages by \@MAKOMO in [`a9a91c8`](https://github.com/nRF24/RF24Network/commit/a9a91c803301bd8d7771cbf7d1e0c538a7fa1693)
- Merge pull request \#116 from MAKOMO/master by \@TMRh20 in [#116](https://github.com/nRF24/RF24Network/pull/116)
- Update helloworld_tx.ino by \@Avamander in [`9950344`](https://github.com/nRF24/RF24Network/commit/99503449af9a4172781fd5d6ee9e5be3c6948627)
- Update helloworld_rx.ino by \@Avamander in [`2d10995`](https://github.com/nRF24/RF24Network/commit/2d1099581aebede5e0c40eb14cf6adaccbce3ba0)
- Attempt 1 at fix for issue \#126. by \@cnlangness in [`c6c554f`](https://github.com/nRF24/RF24Network/commit/c6c554fcacd0e760be44a46aa5b615fcc24a237f)
- Removing the type definition to address peek reading the message and not allowing a read of the message. by \@cnlangness in [`90d841a`](https://github.com/nRF24/RF24Network/commit/90d841a2d62ad96c838ee03ded77918e324df000)
- Updating the classes portion for the peek return of the header. by \@cnlangness in [`2fbb79f`](https://github.com/nRF24/RF24Network/commit/2fbb79fe8c442c93f6dc23be076af9742b2794fe)
- Merge pull request \#127 from cnlangness/patch_126 by \@TMRh20 in [#127](https://github.com/nRF24/RF24Network/pull/127)
- Update library.json by \@Avamander in [`0e1e982`](https://github.com/nRF24/RF24Network/commit/0e1e982b17c5ac14c6ebca1af175883df77625d4)
- Update library.properties by \@Avamander in [`5e0fcbe`](https://github.com/nRF24/RF24Network/commit/5e0fcbe7a3c9858d6a91fe70308661db8856efd5)
- Update version for release by \@TMRh20 in [`2d93030`](https://github.com/nRF24/RF24Network/commit/2d93030cc497bccf3f710829973a1561105c5996)

[1.0.9]: https://github.com/nRF24/RF24Network/compare/v1.0.8...v1.0.9

Full commit diff: [`v1.0.8...v1.0.9`][1.0.9]

## New Contributors
* \@cnlangness made their first contribution
* \@wilmsn made their first contribution
* \@MAKOMO made their first contribution
## [1.0.8] - 2017-01-25

### <!-- 1 --> 🚀 Added

- Support for ATXMega256D3 by \@akatran in [`df6d268`](https://github.com/nRF24/RF24Network/commit/df6d2684e6f87f0af6b2a7322190c8b0116c3a46)
- Added warning about having sleep mode defined. by \@Avamander in [`69b497c`](https://github.com/nRF24/RF24Network/commit/69b497c2065b4668cebf7b160a83f50603291779)

### <!-- 4 --> 🛠️ Fixed

- Miscalculation of max fragments by \@TMRh20 in [`53e1a44`](https://github.com/nRF24/RF24Network/commit/53e1a4456f71b89d0bdb018458c9a1142a548428)
- Fix typo by \@comdata in [`46fb07e`](https://github.com/nRF24/RF24Network/commit/46fb07e4ea5cbbbf4e8081445543a28e447b7b4d)
- Fixed typo by \@comdata in [`9fac7f5`](https://github.com/nRF24/RF24Network/commit/9fac7f5b7c310ddadfa6691abc26f5b97bcd98dc)

### <!-- 9 --> 🗨️ Changed

- Per RF24 issue change binary values to hex by \@TMRh20 in [`85550ee`](https://github.com/nRF24/RF24Network/commit/85550eed8663a2a60dc5def5f7dbd02d2f775446)
- Possibility to setup the interrupt_mode in sleepnode by \@ricgyver in [`7169f23`](https://github.com/nRF24/RF24Network/commit/7169f23662116545ecdcc2478a3c2366096b2185)
- Styling of the interrupt setup by \@ricgyver in [`69b7340`](https://github.com/nRF24/RF24Network/commit/69b7340652d55b7bc237bc801283a062b59b0330)
- Merge pull request \#99 from ricmail85/master by \@ricgyver in [#99](https://github.com/nRF24/RF24Network/pull/99)
- Merge branch 'master' of https://github.com/TMRh20/RF24Network by \@akatran in [`4b3ae24`](https://github.com/nRF24/RF24Network/commit/4b3ae2447c9a95f539b35af36b468777c3a023f4)
- Update from TMRh20/master and merge with local. by \@akatran in [`430dcae`](https://github.com/nRF24/RF24Network/commit/430dcae6cdbb7614a0284a9df0c1ce90f5c36571)
- Update README.md by \@akatran in [`379cb54`](https://github.com/nRF24/RF24Network/commit/379cb54533e375c464e37f2cd116c4255893c2d1)
- Change README.md back to defaults. by \@akatran in [`0dbde60`](https://github.com/nRF24/RF24Network/commit/0dbde601b2dd506326e78260af1365dbd0f4d769)
- Merge remote-tracking branch 'refs/remotes/TMRh20/master' by \@akatran in [`fbe0473`](https://github.com/nRF24/RF24Network/commit/fbe0473ed468c2a8b30907ac71f67766dcf5003f)
- Changes to include XMega D3 family. by \@akatran in [`3d85de9`](https://github.com/nRF24/RF24Network/commit/3d85de9042bba1d4b791c9f1582c6a245094dcc1)
- Merge pull request \#76 from akatran/master by \@akatran in [#76](https://github.com/nRF24/RF24Network/pull/76)
- Undefine sleep mode for ESP by \@comdata in [`302eb03`](https://github.com/nRF24/RF24Network/commit/302eb0347a7b3353ada530467ebe13a72acbe842)
- Merge pull request \#1 from comdata/patch-1 by \@comdata in [`415fdc9`](https://github.com/nRF24/RF24Network/commit/415fdc96c9727fec9715d110b46f6cf9514ac10f)
- Made sure sleep mode is not defined if using ESP8266. by \@Avamander in [#102](https://github.com/nRF24/RF24Network/pull/102)
- Update library.properties by \@Avamander in [`c30db56`](https://github.com/nRF24/RF24Network/commit/c30db56cf8444db1cbf15d470ce92eb59597c3bc)

[1.0.8]: https://github.com/nRF24/RF24Network/compare/v1.0.7...v1.0.8

Full commit diff: [`v1.0.7...v1.0.8`][1.0.8]

## New Contributors
* \@comdata made their first contribution
* \@akatran made their first contribution in [#76](https://github.com/nRF24/RF24Network/pull/76)
* \@ricgyver made their first contribution in [#99](https://github.com/nRF24/RF24Network/pull/99)
## [1.0.7] - 2016-09-20

### <!-- 1 --> 🚀 Added

- Merge pull request \#90 from aaddame/master by \@Avamander in [#90](https://github.com/nRF24/RF24Network/pull/90)

### <!-- 4 --> 🛠️ Fixed

- Fix peek length by \@TMRh20 in [`3fe2560`](https://github.com/nRF24/RF24Network/commit/3fe2560f8d22cfbdeed52e327eb76524aff528b4)
- Don't use W_TX_PAYLOAD_NO_ACK flag by \@TMRh20 in [`deeb0b2`](https://github.com/nRF24/RF24Network/commit/deeb0b2284b74ef012664fd5cc12ecb923846d9d)

### <!-- 9 --> 🗨️ Changed

- Update RF24Network_config.h by \@aaddame in [`1d4f7f7`](https://github.com/nRF24/RF24Network/commit/1d4f7f79f57774a6e6945ce91f6fc1f1d28d4cb5)
- Revert "Update RF24Network_config.h" by \@TMRh20 in [`415fa8d`](https://github.com/nRF24/RF24Network/commit/415fa8d17095fd500f26ea2180220cd75096c775)
- Merge pull request \#91 from TMRh20/revert-90-master by \@TMRh20 in [#91](https://github.com/nRF24/RF24Network/pull/91)
- Initialize variables per \#83 \@mz-fuzzy by \@TMRh20 in [`a00090c`](https://github.com/nRF24/RF24Network/commit/a00090cae05063d17a05763e0bc92bd0ee8762c3)
- Errors/Crashing on non-AVR, non-Linux devices by \@TMRh20 in [`61a35e4`](https://github.com/nRF24/RF24Network/commit/61a35e4d9e5f11a6697a72f4bd4ef4c4be14eb23)
- Update library properties for release by \@TMRh20 in [`01cdecb`](https://github.com/nRF24/RF24Network/commit/01cdecb1b27a2d0767260c2e2d555821e4bb1dc3)

[1.0.7]: https://github.com/nRF24/RF24Network/compare/v1.0.6...v1.0.7

Full commit diff: [`v1.0.6...v1.0.7`][1.0.7]

## New Contributors
* \@aaddame made their first contribution
## [1.0.6] - 2016-04-12

### <!-- 3 --> 🗑️ Removed

- Fix alignment/padding issues non-linux devices \#78 by \@TMRh20 in [`609bc2f`](https://github.com/nRF24/RF24Network/commit/609bc2f0ab684c0345113aecfefd71c0bbcb1e05)

### <!-- 9 --> 🗨️ Changed

- Linux examples makefile fix for ubuntu - moving by \@martin-mat in [`0849bee`](https://github.com/nRF24/RF24Network/commit/0849bee79971a28d78dc000b3ccbeb18e9a7b013)
- Increment Version for latest fix by \@TMRh20 in [`dd4e04d`](https://github.com/nRF24/RF24Network/commit/dd4e04d2008e4271868d79c38e3d01608a7fd0b8)

[1.0.6]: https://github.com/nRF24/RF24Network/compare/v1.0.5...v1.0.6

Full commit diff: [`v1.0.5...v1.0.6`][1.0.6]

## [1.0.5] - 2016-03-27

### <!-- 3 --> 🗑️ Removed

- Fix incorrect pipe for node 05, other by \@TMRh20 in [`3a42eb9`](https://github.com/nRF24/RF24Network/commit/3a42eb9d8e4764f1c007fd4ee2517c13e37b764e)

### <!-- 4 --> 🛠️ Fixed

- Fix peek per \#78 & \#80 by \@TMRh20 in [`09a9ccf`](https://github.com/nRF24/RF24Network/commit/09a9ccf581273eb5f488dd5446d1a2c22a5d8c2b)

### <!-- 9 --> 🗨️ Changed

- RPI2 autodetection by \@yozik04 in [`4812504`](https://github.com/nRF24/RF24Network/commit/48125041bbd0646ba7db8e3733258d94bf32adde)
- Renamed readme files. by \@Avamander in [`3bbe66b`](https://github.com/nRF24/RF24Network/commit/3bbe66b4b27f8aa644dbe4ee99fcaf3ac2f680ab)
- Archived old sketch to consume less space. by \@Avamander in [`7e47a11`](https://github.com/nRF24/RF24Network/commit/7e47a11d3c55e12066f92657e90d0ba27bb0d339)
- Merge pull request \#81 from Avamander/master by \@Avamander in [`6f622f8`](https://github.com/nRF24/RF24Network/commit/6f622f81593857fb008d4b00071b28308f670d07)
- Update pyRF24Network.cpp by \@gregj1 in [`796c6d0`](https://github.com/nRF24/RF24Network/commit/796c6d02e5e1d228f934e318266a4f60796dd01b)
- Merge pull request \#75 from gregj1/master by \@martin-mat in [`6909ede`](https://github.com/nRF24/RF24Network/commit/6909ede59c0f054ecb936eab82b4b4678477c58b)
- Testing changes for \#78 & \#80 by \@TMRh20 in [`18a0a2e`](https://github.com/nRF24/RF24Network/commit/18a0a2e9660515a496068bee2e884e46bb18821e)
- Merge remote-tracking branch 'refs/remotes/origin/Development' by \@TMRh20 in [`0e102a2`](https://github.com/nRF24/RF24Network/commit/0e102a24c9dee7335a9fd0d0f7b56a6f3fbb39f1)
- Minor change for network_poll by \@TMRh20 in [`4c1293d`](https://github.com/nRF24/RF24Network/commit/4c1293d229755eea1d101c072813254e78aecc7a)
- Increment version for release by \@TMRh20 in [`a25d806`](https://github.com/nRF24/RF24Network/commit/a25d806ca11ef4859ab2817ab8f69f97d7957a61)

[1.0.5]: https://github.com/nRF24/RF24Network/compare/v1.0.4...v1.0.5

Full commit diff: [`v1.0.4...v1.0.5`][1.0.5]

## New Contributors
* \@martin-mat made their first contribution
* \@gregj1 made their first contribution
* \@yozik04 made their first contribution
## [1.0.4] - 2015-12-15

### <!-- 4 --> 🛠️ Fixed

- Fix for last change by \@TMRh20 in [`5f1cf94`](https://github.com/nRF24/RF24Network/commit/5f1cf94964489389c590ff9a16831d971c6499f3)

### <!-- 9 --> 🗨️ Changed

- Adjustment for last change by \@TMRh20 in [`950c94e`](https://github.com/nRF24/RF24Network/commit/950c94e33bbeb4e84ae9e46fc388707df2f5ff69)
- Update library properties by \@TMRh20 in [`49897bd`](https://github.com/nRF24/RF24Network/commit/49897bd801ac0589d79a7ed30a3d805841d50d83)

[1.0.4]: https://github.com/nRF24/RF24Network/compare/v1.0.3...v1.0.4

Full commit diff: [`v1.0.3...v1.0.4`][1.0.4]

## [1.0.3] - 2015-12-14

### <!-- 1 --> 🚀 Added

- Adds helloworld_tx.python3.py by \@joernesdohr in [`cdaa108`](https://github.com/nRF24/RF24Network/commit/cdaa1081ce2956592fde1b088ca9fc850517ed12)
- Adds helloworld_rx.python3.py by \@joernesdohr in [`84f8dff`](https://github.com/nRF24/RF24Network/commit/84f8dffb79bb86a31194bc2199ec81bd6ec60200)

### <!-- 3 --> 🗑️ Removed

- Removes getPayloadsize reference by \@joernesdohr in [`dd7808f`](https://github.com/nRF24/RF24Network/commit/dd7808f9b051329b629ecf9def9f9ffd27476077)
- Removes build folder by \@joernesdohr in [`31d4694`](https://github.com/nRF24/RF24Network/commit/31d46943d45081b6d74dd6ab84954824582cbbe4)
- Removes obsolete Python 3 specific examples by \@joernesdohr in [`61be0bf`](https://github.com/nRF24/RF24Network/commit/61be0bf6e4ed85f2fb615a63e95170501e4b3f1e)
- Increase throughput, adjust delays, etc by \@TMRh20 in [`baacbd2`](https://github.com/nRF24/RF24Network/commit/baacbd2114e40175d5ef4b8e3f6225953b2cd350)

### <!-- 4 --> 🛠️ Fixed

- Fixes copy-paste error by \@joernesdohr in [`ba96a30`](https://github.com/nRF24/RF24Network/commit/ba96a30c3d04314d8e59edc49254a24703bb2fe5)
- Fix payload struct format by \@joernesdohr in [`b9d7bdd`](https://github.com/nRF24/RF24Network/commit/b9d7bdd5120fec00a730e37d5a8d8fa327c12347)

### <!-- 9 --> 🗨️ Changed

- Changes std::string to Python MemoryView object to fix String encoding errors in Python 3 by \@joernesdohr in [`3f0b2ef`](https://github.com/nRF24/RF24Network/commit/3f0b2ef67a685c17ca1d006bcd601e5c7d45170d)
- Merge remote-tracking branch 'remotes/upstream/master' by \@joernesdohr in [`f6e0360`](https://github.com/nRF24/RF24Network/commit/f6e03605cef0c30d76269c593a6d72d2e4cd66ce)
- Changed String objects to bp:object types in Python wrapper by \@joernesdohr in [`b0fbd1d`](https://github.com/nRF24/RF24Network/commit/b0fbd1d0374230019a58dca7af2a64665ffbde1c)
- Updates wrapper definition by \@joernesdohr in [`e9846e4`](https://github.com/nRF24/RF24Network/commit/e9846e403cac7234ba5120816a29036db1c2b3c7)
- Updates setup.py to compile Python wrapper with libboost python 3 if available by \@joernesdohr in [`1dcd3f5`](https://github.com/nRF24/RF24Network/commit/1dcd3f50585dc6f14578f06f2784ee753a0ce3b4)
- Changes struct format to two unsigned Long types in helloworld python examples by \@joernesdohr in [`736234c`](https://github.com/nRF24/RF24Network/commit/736234c0ef53ffe42f3f6a9506eea574ebcea4fc)
- Forgot sys module import by \@joernesdohr in [`4e2889b`](https://github.com/nRF24/RF24Network/commit/4e2889b133fc3f027bf40db640c6fbbaac96f909)
- Changes radio pin configuration by \@joernesdohr in [`8406ca5`](https://github.com/nRF24/RF24Network/commit/8406ca5e54014a946d7c82b6ed0e516d439b211b)
- Changes incorrect self.radio to radio variable by \@joernesdohr in [`ace215b`](https://github.com/nRF24/RF24Network/commit/ace215b11c6b86e0eb59ddc69c60021e707afdad)
- Reading 8 bit, not 12 by \@joernesdohr in [`4601267`](https://github.com/nRF24/RF24Network/commit/4601267e54f9c44f40b535d76007e734e273a728)
- Bytearray to String with str() in Python 2.7 by \@joernesdohr in [`814c5e0`](https://github.com/nRF24/RF24Network/commit/814c5e01edf5760b3fd0291dde634c86b8086f3e)
- Makes helloworld_rx.py, helloworld_tx.py scrips compatible with Python 3. Fixes millis() byte length by \@joernesdohr in [`5e6a16e`](https://github.com/nRF24/RF24Network/commit/5e6a16ed34a57783bb1703060da695ea7f65d02a)
- Merge pull request \#68 from joernesdohr/master by \@TMRh20 in [`53b0a75`](https://github.com/nRF24/RF24Network/commit/53b0a758c113dc4c74d48cf43afdadcb418b97bc)
- Update library.properties by \@Avamander in [`7e2768d`](https://github.com/nRF24/RF24Network/commit/7e2768d9180fe8d15a865e378537cb07c21a52e9)

[1.0.3]: https://github.com/nRF24/RF24Network/compare/v1.0.2...v1.0.3

Full commit diff: [`v1.0.2...v1.0.3`][1.0.3]

## New Contributors
* \@joernesdohr made their first contribution
## [1.0.2] - 2015-11-27

### <!-- 1 --> 🚀 Added

- Add flag to prevent polling. Fix header type change. by \@TMRh20 in [`cce19b2`](https://github.com/nRF24/RF24Network/commit/cce19b2fc4c28526098f64595a14b35d6f9e517b)

### <!-- 4 --> 🛠️ Fixed

- Fix for ATtiny per \#67 by \@TMRh20 in [`7eba4ad`](https://github.com/nRF24/RF24Network/commit/7eba4adfdc1e2b2505e1f905dcf6f4a5dc5269e8)
- Fix per \#66 by \@TMRh20 in [`8552324`](https://github.com/nRF24/RF24Network/commit/855232488dfb9d34cae97626986c1d56c58cbbe6)

### <!-- 9 --> 🗨️ Changed

- Cleanup config.h file by \@TMRh20 in [`d4c6790`](https://github.com/nRF24/RF24Network/commit/d4c679046a8da6e66202fd91b1868a477ab72e98)
- Increment version by \@TMRh20 in [`612b569`](https://github.com/nRF24/RF24Network/commit/612b5697a4db6f12606494c8239728277455ae79)

[1.0.2]: https://github.com/nRF24/RF24Network/compare/v1.0.1...v1.0.2

Full commit diff: [`v1.0.1...v1.0.2`][1.0.2]

## [1.0.1] - 2015-11-06

### <!-- 3 --> 🗑️ Removed

- Slight modification for RF24Mesh changes by \@TMRh20 in [`9c1803a`](https://github.com/nRF24/RF24Network/commit/9c1803a3954c0539e802cb02c5bd30b2ff67e350)

[1.0.1]: https://github.com/nRF24/RF24Network/compare/v1.0...v1.0.1

Full commit diff: [`v1.0...v1.0.1`][1.0.1]

## [1.0] - 2015-09-26

### <!-- 1 --> 🚀 Added

- Added gitignore by \@maniacbug in [`89c2a82`](https://github.com/nRF24/RF24Network/commit/89c2a82323cab4b8ff07b12b5d14e0db86a12839)
- Added by \@maniacbug in [`aa79084`](https://github.com/nRF24/RF24Network/commit/aa790843357f04fd21d68720f1e67e4c41c1a59e)
- Added more debugging text by \@maniacbug in [`4c9141b`](https://github.com/nRF24/RF24Network/commit/4c9141b78ec0da570f2ed03e697aa1a37b922d42)
- Added id field to header by \@maniacbug in [`48d22a6`](https://github.com/nRF24/RF24Network/commit/48d22a6669830f353ae846443ad286a473709f2a)
- Add gitignore by \@maniacbug in [`12c22a1`](https://github.com/nRF24/RF24Network/commit/12c22a1b714d720dd1d88808ed46cb742fab1cca)
- Added upload caability by \@maniacbug in [`7f70e7d`](https://github.com/nRF24/RF24Network/commit/7f70e7d105040bd1cbf990f3ec706ab06950a1ce)
- Added node configuration capability by \@maniacbug in [`7cfee69`](https://github.com/nRF24/RF24Network/commit/7cfee69db18e7ce756e742d796032d312528f86f)
- Added an error message if no good address is found by \@maniacbug in [`108dd5e`](https://github.com/nRF24/RF24Network/commit/108dd5e40b5a98d3a172c8c884e6f43c0227d065)
- Added copyright header by \@maniacbug in [`8c5aa77`](https://github.com/nRF24/RF24Network/commit/8c5aa77afb4acdc144355bcd9069292f82e6700e)
- Added -Wextra -Werror and cleaned up the resulting errors by \@maniacbug in [`a39c724`](https://github.com/nRF24/RF24Network/commit/a39c724226c07bf679b692d3c18ceed58cb13b39)
- Added vesion tracking for git.  Is this overkill for an example?? by \@maniacbug in [`2a2f893`](https://github.com/nRF24/RF24Network/commit/2a2f893d7f5f9de540e84ee814d28b576db51c3b)
- Added type and time fields to header by \@maniacbug in [`550bf11`](https://github.com/nRF24/RF24Network/commit/550bf117ae125c0f48b848ac8390ae90200660cf)
- Add version.h by \@maniacbug in [`031d39f`](https://github.com/nRF24/RF24Network/commit/031d39f0acfa9307bb1ab7ddfb523e886e36a4dc)
- Added checking for valid address by \@maniacbug in [`82576d0`](https://github.com/nRF24/RF24Network/commit/82576d0a31140d72c36753c8663df19d38d9ba57)
- Added version.h back into meshping.pde by \@maniacbug in [`79e49a3`](https://github.com/nRF24/RF24Network/commit/79e49a37cc63dd762e44079f745add8ac0fe16ee)
- Added stub test framework by \@maniacbug in [`fc43ebc`](https://github.com/nRF24/RF24Network/commit/fc43ebc5a6a3c6a5c98ca6c44f0a8ad524df661b)
- Added 'echo' ping test by \@maniacbug in [`99f61e4`](https://github.com/nRF24/RF24Network/commit/99f61e4677d8ee5ae81a762d32ca9608924b3384)
- Added Sync::reset, so resetting data doesn't turn into spurrious sync messages by \@maniacbug in [`0be5493`](https://github.com/nRF24/RF24Network/commit/0be5493e4b4d39fad4ca7e62748f13f9d194914e)
- Add optional TX LED by \@maniacbug in [`6bf2d2c`](https://github.com/nRF24/RF24Network/commit/6bf2d2c4564cff74b39156042ea3496ca9f5cfb3)
- Added build instructions for Duinode V1 by \@maniacbug in [`42a43be`](https://github.com/nRF24/RF24Network/commit/42a43bef824aa785431552b4478fd209bbacfbc6)
- Added startup LED sequence and ability to enter test mode by \@maniacbug in [`5999a2c`](https://github.com/nRF24/RF24Network/commit/5999a2c0f02ad7e134cd74b7595f8a58a1e0ea9f)
- Added extra LED indicators in test mode by \@maniacbug in [`5954338`](https://github.com/nRF24/RF24Network/commit/5954338870ac3de24d4b026bc25d5903e7a8f3d0)
- Added calibration mode, and theUpdater by \@maniacbug in [`78bea2f`](https://github.com/nRF24/RF24Network/commit/78bea2f4d0979dc7eddc2c614016561286de4d06)
- Add relay role, configurable in eeprom by \@maniacbug in [`1fa87ee`](https://github.com/nRF24/RF24Network/commit/1fa87ee61e654698d6c283ea9321e7379663bc0c)
- Add manual temperature calibration by \@maniacbug in [`3a1a2a7`](https://github.com/nRF24/RF24Network/commit/3a1a2a799d76ea670a7d8afc37313e7500ffa0db)
- Add lost packets to S message by \@maniacbug in [`50bd166`](https://github.com/nRF24/RF24Network/commit/50bd16694f36df03b744bfea6ebaa93db0ab5dfb)
- Add radio sending retries.  Needed now that RF24 doesn't have them by \@maniacbug in [`7d4b5b1`](https://github.com/nRF24/RF24Network/commit/7d4b5b195684a607c4676aea31614dc5c66eb657)
- Add eeprom_update_block for older avrlibc's which are missing it by \@maniacbug in [`95241ba`](https://github.com/nRF24/RF24Network/commit/95241bac37d93b5762d0cae163bac7f35b76032b)
- Add sleep for ATTiny  \#8 by \@TMRh20 in [`80a6dcd`](https://github.com/nRF24/RF24Network/commit/80a6dcde0224fcf06735546f0b218646f0f9aa25)
- Add compatibility for Arduino ESP8266 architecture by \@crcastle in [`1839bce`](https://github.com/nRF24/RF24Network/commit/1839bce965c7f8d84c7213ef6b45c2bcf97edab9)
- Add Pandora's box routing by \@TMRh20 in [`2474311`](https://github.com/nRF24/RF24Network/commit/247431141e3d682091e10a11fdd56f9026c92440)
- Add network wide multicasting and multicast routing by \@TMRh20 in [`6e25453`](https://github.com/nRF24/RF24Network/commit/6e25453ffabf583c0304b701a3cfcf8f98c9fc94)
- Add a frame structure for each message. by \@reixd in [`fda8602`](https://github.com/nRF24/RF24Network/commit/fda8602c7beeb747b31175203f39bb9caf5c14dd)
- Add copyright by \@reixd in [`b1e8590`](https://github.com/nRF24/RF24Network/commit/b1e85902e46d69209ec3ef40cde3ab7ea1b31546)
- Add conversion by \@TMRh20 in [`8261dc2`](https://github.com/nRF24/RF24Network/commit/8261dc202c695f9a9005f5c61e958dc21a37e6da)
- Add support for RF24Mesh address lookups by \@TMRh20 in [`d8dc1d6`](https://github.com/nRF24/RF24Network/commit/d8dc1d64289b40f2ae9e19c7ccf578e20f4d01b6)
- Add fragmentation for Arduino, related changes by \@TMRh20 in [`a6b3b5e`](https://github.com/nRF24/RF24Network/commit/a6b3b5e9e290765ff91ff3dcb70b3dae87ee298f)
- Add config of user buffer size to _config.h file by \@TMRh20 in [`1ce964c`](https://github.com/nRF24/RF24Network/commit/1ce964cd553437e4717fd63497ccad53082b84a5)
- Add separate defaults for ATTiny, minor debug chg by \@TMRh20 in [`a1706ef`](https://github.com/nRF24/RF24Network/commit/a1706ef8d7d9bf6143bd7395c3454b5c37e68476)
- Add returnSysMsgs variable by \@TMRh20 in [`506239c`](https://github.com/nRF24/RF24Network/commit/506239c18af1d91b4e3996848756ab94ea48b13a)
- Added sleepInterrupted Flag to determine if SleepNode was Interrupted by \@dsbaha in [`ce3b522`](https://github.com/nRF24/RF24Network/commit/ce3b522b85c021576eb38bb908d3418fa797ea70)

### <!-- 3 --> 🗑️ Removed

- Removed explicity directionality in the net layer by \@maniacbug in [`7fb1a5a`](https://github.com/nRF24/RF24Network/commit/7fb1a5ae8acc80da48226e9280d2c1613b69465c)
- Removed time from header.  Added reserved byte for future use. by \@maniacbug in [`13d52c2`](https://github.com/nRF24/RF24Network/commit/13d52c269c25c85065bda0c6f842261dc619521e)
- Removed NodeLine struct, not neeed anymore by \@maniacbug in [`3684a91`](https://github.com/nRF24/RF24Network/commit/3684a91d5e417fd6b754adfc83db69573d8a28c6)
- Remove serial debugging by \@maniacbug in [`3a30dbe`](https://github.com/nRF24/RF24Network/commit/3a30dbe280945338d917b8c584f2be053a3d1749)
- Remove directionality by \@maniacbug in [`32e49c8`](https://github.com/nRF24/RF24Network/commit/32e49c8d95001b3c32f5278581a855669d85bf7a)
- Remove use of 'prog_char' for compliance with GCC 4.6.2. by \@maniacbug in [`98830b3`](https://github.com/nRF24/RF24Network/commit/98830b3b71f3912e155181b5bb704b66a9f913fe)
- Refactor platform pin names to use same names as my 1284P platform.  Remove un-needed counter from S message, which duplicates the id in the header.  Print node names in octal. by \@maniacbug in [`da171e8`](https://github.com/nRF24/RF24Network/commit/da171e8282a282effbfad871efc5f8430812c43a)
- Optimized Network Lib: Sleep, Extended Timeouts by \@TMRh20 in [`e8df6e9`](https://github.com/nRF24/RF24Network/commit/e8df6e91e555edca8099c3fcc122f7d8944fd5c7)
- Disable broken sleep mode by \@TMRh20 in [`9428e97`](https://github.com/nRF24/RF24Network/commit/9428e9711721af4e228e161b2d921f70885e8c99)
- Update the pin layout table and consolidate the RPi readme files. by \@reixd in [`69af19c`](https://github.com/nRF24/RF24Network/commit/69af19cb496f6a7c2e98590d1be85dffbf11017d)
- Fix RF24NetworkHeader::toString() and remove commented out line by \@flavio-fernandes in [`37bd59a`](https://github.com/nRF24/RF24Network/commit/37bd59ab8e51688a0cbf69ced9c487d7ed3e3bc8)
- Merge pull request \#30 from flavio-fernandes/master by \@TMRh20 in [`ed05223`](https://github.com/nRF24/RF24Network/commit/ed0522394aca223472b5c76e9067720987c220fd)
- Use the RF24NetworkFrame type for internal management of frames by \@reixd in [`64147ff`](https://github.com/nRF24/RF24Network/commit/64147ff40ef55c11a7b48a605253cbf58808ecf0)
- Fully functional fragmentation and reassembly implementation for large payloads. by \@reixd in [`d49e957`](https://github.com/nRF24/RF24Network/commit/d49e9579b1bdf173606084223c0bf42175a7cb66)
- Fully functional fragmentation implementation for large payloads. by \@reixd in [`a4dbdd1`](https://github.com/nRF24/RF24Network/commit/a4dbdd149a4b2c81f97f95ed2de4249aa08f78c5)
- Remove not needed CircularBuffer data structure by \@reixd in [`f454415`](https://github.com/nRF24/RF24Network/commit/f45441532f2014a990e12d2e565e7b0a8a51baab)
- Cleanup code. by \@reixd in [`ee420d1`](https://github.com/nRF24/RF24Network/commit/ee420d1734b5823ca163ac4a5c46d37ecb2b5d18)
- Remove unwanted check in appendFragmentToFrame(). by \@reixd in [`e0ec131`](https://github.com/nRF24/RF24Network/commit/e0ec131ea3e38559c291667631dfb64d0f3f4055)
- Cleanup for Aduino code by \@TMRh20 in [`94dfbd5`](https://github.com/nRF24/RF24Network/commit/94dfbd50cecd21306e3b2bffbc34e048117c4d8f)
- Clean up a bit from recent changes by \@TMRh20 in [`8062093`](https://github.com/nRF24/RF24Network/commit/806209301bf04b51ce46d01593c55486f5593778)
- Fragmentation and cache cleanup by \@TMRh20 in [`44f5594`](https://github.com/nRF24/RF24Network/commit/44f559455a374cd22412dc3d67ea6192403a2ea4)
- Fragmentation/Re-assembly improvements by \@TMRh20 in [`56716ae`](https://github.com/nRF24/RF24Network/commit/56716aeda2aa260b40cf1d635198890220914df9)
- Remove stray printf by \@TMRh20 in [`403c2ad`](https://github.com/nRF24/RF24Network/commit/403c2ad024b4d7e471e36c5224e48072a44403ac)
- Remove leftover printf from last commit by \@TMRh20 in [`1b0a701`](https://github.com/nRF24/RF24Network/commit/1b0a701beadaeee50d6663789dba65beab2209d9)
- Fixes, Changes & Improvements by \@TMRh20 in [`1296058`](https://github.com/nRF24/RF24Network/commit/129605806c1521f3ffbf00403bcd501f1155cc12)
- Update RF24Network_config.h file by \@TMRh20 in [`19d4631`](https://github.com/nRF24/RF24Network/commit/19d4631d08d449a60c19442cd656269c7dd96d6a)
- Much improved handling of user data by \@TMRh20 in [`ac45c6a`](https://github.com/nRF24/RF24Network/commit/ac45c6a5e992e7373e5d8af49daf5a5808c51cd0)
- Fix for \#124 by \@TMRh20 in [`c88ccdc`](https://github.com/nRF24/RF24Network/commit/c88ccdc804a2d8611fc10f43b593aeb11491794b)

### <!-- 4 --> 🛠️ Fixed

- Fixed problem with buffer.  Throw out bogus packets. by \@maniacbug in [`20ab90d`](https://github.com/nRF24/RF24Network/commit/20ab90dd47e890cb386c7e05426139eed9ca8393)
- Fixed version.h by \@maniacbug in [`e112cb9`](https://github.com/nRF24/RF24Network/commit/e112cb9ce1dbb6d8b1c68b9b93b1a55f92bb4fdc)
- Fixed a bug where radio was not automatically listening.  Also, pipes 2+ were not being opened correctly. by \@maniacbug in [`5bab390`](https://github.com/nRF24/RF24Network/commit/5bab390f92d334a086adf47d87c88c62f570c9aa)
- Fixed a bug, now properly pings base. by \@maniacbug in [`97b957b`](https://github.com/nRF24/RF24Network/commit/97b957b19b98f2d9491786ad1e0853f5450ed8e3)
- Fixed missing includes, thanks to Spoiler for reporting the problem. by \@maniacbug in [`e03ee47`](https://github.com/nRF24/RF24Network/commit/e03ee4742d6579cf13a2852754bb95e84a9ed002)
- Fix a couple bugs by \@maniacbug in [`c25b5bd`](https://github.com/nRF24/RF24Network/commit/c25b5bdb0992bddf82e0fbbd33a3c01924c66415)
- Fix bug where relay nodes were shutting down the radio by \@maniacbug in [`5fafdc5`](https://github.com/nRF24/RF24Network/commit/5fafdc53c764ea33655a0b027184e91d437554a5)
- Fix formula comment by \@maniacbug in [`9a06b03`](https://github.com/nRF24/RF24Network/commit/9a06b03157990128281104a6cf5b3224c69a5217)
- Fix bug in Celsius calculation, was forgetting to convert to 1.1V max by \@maniacbug in [`ecf5f11`](https://github.com/nRF24/RF24Network/commit/ecf5f1163bdc387d5ff8d73b408c5adca147abf5)
- Standby-I in sleep mode, other changes by \@TMRh20 in [`f88f3cb`](https://github.com/nRF24/RF24Network/commit/f88f3cbbf1c737132b32bf59e8284dc08d2afefb)
- Fix typo in readme for RPi by \@TMRh20 in [`288797a`](https://github.com/nRF24/RF24Network/commit/288797ab38b0a11b7c335495e1a3e6f8ecd6915d)
- Fix for ATTiny in IDE 1.5.7 by \@TMRh20 in [`9f19793`](https://github.com/nRF24/RF24Network/commit/9f19793a0affbcc4282dc2f6fe248e0738b042e9)
- Fix wrong pin layout by \@reixd in [`89bc249`](https://github.com/nRF24/RF24Network/commit/89bc249287f4a99b401d073abee4b9001a772d7c)
- Fix some indentation issues. by \@reixd in [`5696f31`](https://github.com/nRF24/RF24Network/commit/5696f310f146b7da485d3de6a94987507d108f08)
- Fixes/Workarounds for fragmentation support by \@TMRh20 in [`e34a394`](https://github.com/nRF24/RF24Network/commit/e34a39472ba06ecf867aace81be4098d29c878d8)
- Fix compile errors by \@TMRh20 in [`f20337c`](https://github.com/nRF24/RF24Network/commit/f20337c0ca1e0b8e018c22905012a33eba0f9e04)
- Fix compile errors by \@TMRh20 in [`bf287d6`](https://github.com/nRF24/RF24Network/commit/bf287d6b82adff7c72f1343fc5b24782f3c293ff)
- Corrupt payloads Add: Address Confirm by \@TMRh20 in [`c73eabc`](https://github.com/nRF24/RF24Network/commit/c73eabc438282da66f71b52883e8efabf92ee5b6)
- Fix fragmentation, frag test changes by \@TMRh20 in [`90d826b`](https://github.com/nRF24/RF24Network/commit/90d826b89515a7beebc13843704fbd03a886eae1)
- Fix compile errors on IDE 1.05 by \@TMRh20 in [`333d332`](https://github.com/nRF24/RF24Network/commit/333d3322a9bfdbc31ca617ab92a654f362a012be)
- Fix compile error with IDE 1.0.5 by \@TMRh20 in [`840036b`](https://github.com/nRF24/RF24Network/commit/840036b402fd5c9ffca5ff042df81d9de0a4ac06)
- Fix broken routing on Arduino by \@TMRh20 in [`bdc6dc2`](https://github.com/nRF24/RF24Network/commit/bdc6dc283e76dd7c4f68bedcbfa780e77959aa19)
- Fix external data disabled without debug minimal by \@TMRh20 in [`1f1a0ce`](https://github.com/nRF24/RF24Network/commit/1f1a0ce7601e8fe1ef7a8141720a2a67fda49d0f)
- Fixes etc by \@TMRh20 in [`18aaa23`](https://github.com/nRF24/RF24Network/commit/18aaa232b47c23e4db440cc865305b6e96ac7563)
- Fix broken responses for RF24Mesh by \@TMRh20 in [`b4090e8`](https://github.com/nRF24/RF24Network/commit/b4090e88f24b778846f4d0facec6a1a342327a34)
- External data type with small payloads by \@TMRh20 in [`8f97fe1`](https://github.com/nRF24/RF24Network/commit/8f97fe14c5dea05836f59e79e08af4f4b585b108)
- Multicast relay interfering with RF24Mesh by \@TMRh20 in [`0771e19`](https://github.com/nRF24/RF24Network/commit/0771e190b68f188e0b625b78b941562608e3346b)
- RPi reassembly for multicast payloads by \@TMRh20 in [`a413e14`](https://github.com/nRF24/RF24Network/commit/a413e149f7e3cc13c6d4b09985da68e24fd45613)
- Fix non-fragmented writes from last change by \@TMRh20 in [`8cb0b35`](https://github.com/nRF24/RF24Network/commit/8cb0b35f631a0d33013d6e9db238490a7b53e088)
- Fix mistake in read function for Arduino by \@TMRh20 in [`a3b27d9`](https://github.com/nRF24/RF24Network/commit/a3b27d95f020b7a4e68f055d2f9fb48516e5375a)
- Fix memory issue, code clean up, testing changes by \@TMRh20 in [`c90027f`](https://github.com/nRF24/RF24Network/commit/c90027f1767c1689ec5cfa34afb225519e1b6537)
- Fix one thing, break another... by \@TMRh20 in [`3e2da3a`](https://github.com/nRF24/RF24Network/commit/3e2da3a73374067eaea28dea4d6d160368f03a1e)
- Fix for RF24Mesh by \@TMRh20 in [`3bf56cb`](https://github.com/nRF24/RF24Network/commit/3bf56cb74b0ab0ffa999006d8a98297c65fbb94a)
- Multicast, network_ack, RF24Mesh, debug by \@TMRh20 in [`ee34bf1`](https://github.com/nRF24/RF24Network/commit/ee34bf1a15069468281ed102418f640fab01ba5b)
- Fix example Makefile for non-RPi by \@TMRh20 in [`ebc57bd`](https://github.com/nRF24/RF24Network/commit/ebc57bdc3f27338363335f83a4ce6cae0fdee7a7)
- Hanging on Due/Teensy by \@TMRh20 in [`5622c65`](https://github.com/nRF24/RF24Network/commit/5622c6505e67b35f993061fe979486405a43cce4)
- Fixes for \#48 by \@TMRh20 in [`f866332`](https://github.com/nRF24/RF24Network/commit/f866332830d121cc5bf249a4bd41eb260107eeea)
- Fix for ATtiny - buffer size declaration by \@TMRh20 in [`e60d6ea`](https://github.com/nRF24/RF24Network/commit/e60d6eac7ca43541f40ca55362dadd3529f59ec1)
- Fix unicast write after latest ack changes by \@TMRh20 in [`37abbca`](https://github.com/nRF24/RF24Network/commit/37abbca9274675d978165f9ac49634df50640845)

### <!-- 7 -->🚦 Tests

- Finder, echo, and sync tests all working nicely now.  Yay by \@maniacbug in [`06a2341`](https://github.com/nRF24/RF24Network/commit/06a2341afaac161469ef9607c8a17effabb20af9)

### <!-- 8 --> 📝 Documentation

- Massive documentation update by \@maniacbug in [`b23ff53`](https://github.com/nRF24/RF24Network/commit/b23ff531e50b2cd6eaf03cb7205b73f272a167d3)
- Docupdate by \@maniacbug in [`7d31cc4`](https://github.com/nRF24/RF24Network/commit/7d31cc459c69e06e328a43fd5f76d6bd074dc551)
- Docfix by \@maniacbug in [`f154300`](https://github.com/nRF24/RF24Network/commit/f154300a751a4072804ac49813a7e9c5f473ef3d)
- Moredocs by \@maniacbug in [`3b216f9`](https://github.com/nRF24/RF24Network/commit/3b216f96a5f7b5b7f732e7b98184b2d9eace4b4a)
- Updatedocs by \@maniacbug in [`6c7479a`](https://github.com/nRF24/RF24Network/commit/6c7479a319de192cd3221ebff886b62e41f45ed3)
- Docs by \@maniacbug in [`e5e22f0`](https://github.com/nRF24/RF24Network/commit/e5e22f0bb7c0b5edb9a832f54100ae7cc53bf656)
- Trying to fix a broken doc page link by \@maniacbug in [`5dbd26b`](https://github.com/nRF24/RF24Network/commit/5dbd26b9a738281a25e5a95ca343ae8fa633c0fa)
- Minor doc change, Fix example topology by \@TMRh20 in [`3494280`](https://github.com/nRF24/RF24Network/commit/34942806818ad43f8a1057e2210629aafd626a42)
- Update to main doc page by \@TMRh20 in [`0e24467`](https://github.com/nRF24/RF24Network/commit/0e24467db2f369c752d9732bb2f505534673ac36)
- Staggered timeouts, Dual head mode by \@TMRh20 in [`b47f480`](https://github.com/nRF24/RF24Network/commit/b47f480f50c942a3fafde6cc758465071616bdfc)
- \@PlatformIO Library Registry manifest file by \@ivankravets in [`2db741c`](https://github.com/nRF24/RF24Network/commit/2db741c7663f6387cb3dc982891485718310ca1f)
- SleepNodeInterrupted, doc cleanup by \@TMRh20 in [`dc8cf7b`](https://github.com/nRF24/RF24Network/commit/dc8cf7baffd5b5a273f76e107155148e8e726d5c)

### <!-- 9 --> 🗨️ Changed

- Initial interface defined by \@maniacbug in [`f54c0ac`](https://github.com/nRF24/RF24Network/commit/f54c0ac0ee74d2fc9490740004bd3222b170ae85)
- Modified for OSX by \@maniacbug in [`11c351f`](https://github.com/nRF24/RF24Network/commit/11c351fca5d0a170f337d2b8b557429b6bce0f25)
- Implemented begin() by \@maniacbug in [`eb2b8f4`](https://github.com/nRF24/RF24Network/commit/eb2b8f443da2e10599bf7e8fb8e4e45b32d23fc6)
- Implemented everything.  Ready to test by \@maniacbug in [`cc00335`](https://github.com/nRF24/RF24Network/commit/cc00335f03936e26ab81ff50ecabf5a8c7288feb)
- Explicitly handle the case of sending to ourself by \@maniacbug in [`7c2a253`](https://github.com/nRF24/RF24Network/commit/7c2a253cdf1eb561f1030ad8ab7c0681fb32e5ac)
- Various cleanups by \@maniacbug in [`582d245`](https://github.com/nRF24/RF24Network/commit/582d245705ed1ef6a10a1189b3b63e42d59eb3d0)
- Update by \@maniacbug in [`873ae7e`](https://github.com/nRF24/RF24Network/commit/873ae7e8df902b045d2e0cdcbd59059962ab86e6)
- More by \@maniacbug in [`d7be898`](https://github.com/nRF24/RF24Network/commit/d7be89824a6bb10171a12f5407372881d09d62a1)
- Fully working now by \@maniacbug in [`26bf3ad`](https://github.com/nRF24/RF24Network/commit/26bf3ad7674983af6f52dc4e66d326c832eab32e)
- Trying to get headerscan to work, and failed by \@maniacbug in [`18163e9`](https://github.com/nRF24/RF24Network/commit/18163e99605e9e79e2ff197b6b69c3b5fdbf3eff)
- Move to eeprom-based node address identification by \@maniacbug in [`1429fa0`](https://github.com/nRF24/RF24Network/commit/1429fa0ce5495512691e631b1ef7ed8e88ecf71e)
- Allow addresses from 0-9 by \@maniacbug in [`a7348ba`](https://github.com/nRF24/RF24Network/commit/a7348ba3d18a0cf86dd8e4473e6d60129175ddae)
- New example by \@maniacbug in [`9eb2a8c`](https://github.com/nRF24/RF24Network/commit/9eb2a8c74c8bb709494749c334d900a0f46c7c1d)
- Update comments by \@maniacbug in [`57c40ed`](https://github.com/nRF24/RF24Network/commit/57c40edd545352bebb314664471a70fd2e8babfa)
- Implemented uni-directional mode by \@maniacbug in [`5329633`](https://github.com/nRF24/RF24Network/commit/5329633810933cb8e3df4ec25a090a8f26f62cf0)
- Refactored sleep helpers by \@maniacbug in [`9ea4cb4`](https://github.com/nRF24/RF24Network/commit/9ea4cb40028bc7a55cccd66081162b4199c245e6)
- Morecomments by \@maniacbug in [`6e16fc0`](https://github.com/nRF24/RF24Network/commit/6e16fc02c38d9eea21aba3908f9b701255d3d211)
- Major conversion to octal addressing by \@maniacbug in [`c4fb0dc`](https://github.com/nRF24/RF24Network/commit/c4fb0dc2cbc105c0fbf46f9af39fa9972f2f2a6c)
- Converting to octal addressing.  Not done yet. by \@maniacbug in [`bbef737`](https://github.com/nRF24/RF24Network/commit/bbef7373683d71bb1a0691c815768b670398b8ff)
- Finished moving sensornet to octal addressing by \@maniacbug in [`59c6296`](https://github.com/nRF24/RF24Network/commit/59c6296ccd2650ec1493917c8aa08830f3f18833)
- Sensornet now actually takes a sensor reading. by \@maniacbug in [`ce1ccb0`](https://github.com/nRF24/RF24Network/commit/ce1ccb093d1fe4734b1eca3ca5101a2a500c80de)
- Meshping now only pings the base. by \@maniacbug in [`734dc66`](https://github.com/nRF24/RF24Network/commit/734dc66330ec621503538a425ac404c56c30371d)
- Always build usb ports by \@maniacbug in [`e4f56cc`](https://github.com/nRF24/RF24Network/commit/e4f56cc13060748a2363c9e6d305dd27cf26e951)
- Now maintains a list of known nodes, and pings those. by \@maniacbug in [`e60373a`](https://github.com/nRF24/RF24Network/commit/e60373a372c624dd6d84604734403f131e8f64da)
- Base now sends out the list of nodes to all nodes on the network by \@maniacbug in [`17abe16`](https://github.com/nRF24/RF24Network/commit/17abe164c5ad7e3d12d223cd72d0acd0d8e9714a)
- Comments by \@maniacbug in [`54c7144`](https://github.com/nRF24/RF24Network/commit/54c7144275a1cf42d5a209a3fe0c1dee5531b44f)
- Stylistic node address changes by \@maniacbug in [`6d59a38`](https://github.com/nRF24/RF24Network/commit/6d59a38f6104cdc3660c7fa8c6c3e5e1484631fe)
- Always open all pipes for reading.  This is possible now that we know exactly what nodes to listen for. by \@maniacbug in [`fce4cf4`](https://github.com/nRF24/RF24Network/commit/fce4cf4242a4f4961e11709dc36ef8c3c72df76b)
- Cleaned up serial debug prints by \@maniacbug in [`59fb4d4`](https://github.com/nRF24/RF24Network/commit/59fb4d4a487d5909ca3f1f0fbc15cdfad0d5121c)
- Moredebugginginfo by \@maniacbug in [`8857b6a`](https://github.com/nRF24/RF24Network/commit/8857b6a6c001dbcf9f12dfd1bac876abd13480f9)
- Use node index in eeprom instead of node address by \@maniacbug in [`8b2642e`](https://github.com/nRF24/RF24Network/commit/8b2642e114a5a81663f48f5be413b81c325b8ced)
- Converte header-to-string to print proper octal addresses by \@maniacbug in [`7b64c51`](https://github.com/nRF24/RF24Network/commit/7b64c51b7046af58db0ee558078942f631c992c9)
- Two new super-simple examples by \@maniacbug in [`a4571e6`](https://github.com/nRF24/RF24Network/commit/a4571e66753decb16363078900a53eb111db1381)
- Arduino.h for Arduino 1.0 by \@maniacbug in [`ab3c980`](https://github.com/nRF24/RF24Network/commit/ab3c9805d15a80658e2555c04f5b8fc15b9c5984)
- Using new-style build system by \@maniacbug in [`6dd0a51`](https://github.com/nRF24/RF24Network/commit/6dd0a5106ba2fa72e9fcaf8a6068dc396a477487)
- Update to 1.0 Arduino and Maple.  And an example for Maple. by \@maniacbug in [`1abad5b`](https://github.com/nRF24/RF24Network/commit/1abad5bdc7c184a3b004d687d8aa44f2ad2398a0)
- Better indicator of ok/failure via LED by \@maniacbug in [`44e7980`](https://github.com/nRF24/RF24Network/commit/44e7980be06e04ca8de083fb483edd66c1040bfd)
- Update test to actually start the network by \@maniacbug in [`6754887`](https://github.com/nRF24/RF24Network/commit/6754887ac58e1fc02899df893fa99348176798d2)
- Renames by \@maniacbug in [`9438bd1`](https://github.com/nRF24/RF24Network/commit/9438bd138770c329b6a00958bd640c717dbeabac)
- Handle zero-length messages (where the header is enough info) by \@maniacbug in [`1ea7f80`](https://github.com/nRF24/RF24Network/commit/1ea7f809dddc25cf8a1898d561df2cd1e950432d)
- Experimental functionality to keep a variable space in sync, including unit test by \@maniacbug in [`bf24995`](https://github.com/nRF24/RF24Network/commit/bf249951ebc91f7e93838433a15c0bc688056883)
- Initial implementation of FINDER message on RX by \@maniacbug in [`879fb9a`](https://github.com/nRF24/RF24Network/commit/879fb9aefa58a4eead7eea4a29c48d1423c54dc9)
- Conversions to Arduino 1.0 by \@maniacbug in [`eeffa16`](https://github.com/nRF24/RF24Network/commit/eeffa16a4348ae1cb596ce8d58b095336aacf0e9)
- Updated Jamfile from most recent Maple changes by \@maniacbug in [`d0cbe6e`](https://github.com/nRF24/RF24Network/commit/d0cbe6e4c87d3addd7a8befa373b596856157b12)
- Finder test works.  Other problems with not receiving packet \#2 on tx side and \#3 on rx side. by \@maniacbug in [`9a321c3`](https://github.com/nRF24/RF24Network/commit/9a321c3fd75a74dea01e2931b091416f8dcd5623)
- Stub finder class.  Will take over finder message duties by \@maniacbug in [`6416d82`](https://github.com/nRF24/RF24Network/commit/6416d82009ad160297a8b1c7aae862a9056a94fe)
- Moved finder to separate object, compiles. by \@maniacbug in [`641e1eb`](https://github.com/nRF24/RF24Network/commit/641e1eb3197f739576f44a93f2799dfa5f6f1683)
- Using new location for cxxtest by \@maniacbug in [`267ef8c`](https://github.com/nRF24/RF24Network/commit/267ef8c3fefd72ce9701c8a723adde9b6f3b11cf)
- Merge branch 'topic_echo' of nero:/srv/git/RF24Network into topic_echo by \@maniacbug in [`6730408`](https://github.com/nRF24/RF24Network/commit/673040832df191dc093ca1c1fbc53c15e3c7f939)
- Moved network startup into main(), so it only fires up the network ONCE.  Compiles only, not tested. by \@maniacbug in [`37e55d4`](https://github.com/nRF24/RF24Network/commit/37e55d496d5fc6a331b6ea1b79cca7fd95bf1b81)
- Merge branch 'topic_echo' by \@maniacbug in [`a10fc9a`](https://github.com/nRF24/RF24Network/commit/a10fc9abdc139ddd8daa1a1b2b1763064750fde9)
- Update sensornet example to 1.0 by \@maniacbug in [`942b664`](https://github.com/nRF24/RF24Network/commit/942b6643c3a77f52f9703a4acd089e876316d47c)
- Updated sensornet example to send TWO sensor values by \@maniacbug in [`02e50fb`](https://github.com/nRF24/RF24Network/commit/02e50fb0064caef36c2bebb5cfbf866ab1f74bfc)
- Increase use of PROGMEM, less RAM use by \@maniacbug in [`d55df40`](https://github.com/nRF24/RF24Network/commit/d55df4039bc929e6ef32744951d9ab0414f3ac3a)
- Inconsequential standards compliance change by \@maniacbug in [`ba94b74`](https://github.com/nRF24/RF24Network/commit/ba94b7483e31f478b1e61b9f1cff5de1fb3260fe)
- Move to channel 92 for compatability with NanodeUIP/examples/sensor_gateway_rf24 by \@maniacbug in [`09dc4e7`](https://github.com/nRF24/RF24Network/commit/09dc4e7d882e9c54d43b59f785e2821b951f63e5)
- Turn off serial debugging by \@maniacbug in [`4088dd4`](https://github.com/nRF24/RF24Network/commit/4088dd4553c29cbe8265ef01184847e5e006ce49)
- Use latest Jamfiles by \@maniacbug in [`95835ec`](https://github.com/nRF24/RF24Network/commit/95835ec7d3528ac8f0b479c4b072e05dc50c3ef1)
- Use Getting Starting board, and other changes designed to make it easier for beginners. by \@maniacbug in [`9b4f8d5`](https://github.com/nRF24/RF24Network/commit/9b4f8d505da789ddda4d9fb6313de675d5a3a57e)
- Use latest Jamfile by \@maniacbug in [`598e78c`](https://github.com/nRF24/RF24Network/commit/598e78c8f64b3de533ff1fd725e68cdffa468f9d)
- Using Getting Started board.  REmove old PSTR warning workarounds, unneeded in gcc >=4.6 by \@maniacbug in [`b8efa19`](https://github.com/nRF24/RF24Network/commit/b8efa19c6be1ecd8afb62dbb90ab20f89938ac5f)
- Ignore 8000000 directory too by \@maniacbug in [`d86062b`](https://github.com/nRF24/RF24Network/commit/d86062bbaa3d446818fa0b7c68e2ee3d945d554e)
- Modernize based on latest approach to multiple nodes. by \@maniacbug in [`1b4aef1`](https://github.com/nRF24/RF24Network/commit/1b4aef1b6ffa14277b75cfdd2fcd1675eaefbc43)
- Made multi-platform aware.  Moved message definition out to own file.  Making first-level nodes not sleep.  Added packet counter. by \@maniacbug in [`eda3268`](https://github.com/nRF24/RF24Network/commit/eda3268ff77b60ccd3c7fb501c0bb6a89f52c530)
- Shell file for creating a 8MHz DuinodeV3 build by \@maniacbug in [`6b90d10`](https://github.com/nRF24/RF24Network/commit/6b90d10284268d9c08415ef3d808a1f2f8dc30ed)
- Sent voltage reading as a meaningful reading by \@maniacbug in [`442edb2`](https://github.com/nRF24/RF24Network/commit/442edb29f002b2bd67c46e5f842812ed0fd4824f)
- Change temperature sensor to transmit meaningful C*256 values by \@maniacbug in [`7c76f97`](https://github.com/nRF24/RF24Network/commit/7c76f97b5b7e49848e45493a540bf73ca551a623)
- Move usb ports by \@maniacbug in [`b76752a`](https://github.com/nRF24/RF24Network/commit/b76752a22ded807355664c48e11a2c49fdacb9a9)
- Hardware definition for V5 duinodes by \@maniacbug in [`f797b2e`](https://github.com/nRF24/RF24Network/commit/f797b2eab759d7441221882cb721a098480c3093)
- Refactor the constant checking for valid LED pins into a class.  Also added the ability to turn test mode OFF. by \@maniacbug in [`d1ada9a`](https://github.com/nRF24/RF24Network/commit/d1ada9a03866823dfc0a1ea40f3fdf9f56c3f5eb)
- Rename LED::set to LED::write, and change led status to bool by \@maniacbug in [`54af0f9`](https://github.com/nRF24/RF24Network/commit/54af0f9bbad6d70d69919b51dd473e52ca1ccc07)
- Using operator= for leds by \@maniacbug in [`c435724`](https://github.com/nRF24/RF24Network/commit/c435724eb25305005d8e7313dbbebc97b8a76f7a)
- Renames and cleanups by \@maniacbug in [`d311cff`](https://github.com/nRF24/RF24Network/commit/d311cff584583767b8e23858aa59028793b442be)
- Use a struct for eeprom info by \@maniacbug in [`1c7d712`](https://github.com/nRF24/RF24Network/commit/1c7d712c2351919ac51e1d0b0f83961c2a1936a9)
- Get header from correct place by \@maniacbug in [`01a317a`](https://github.com/nRF24/RF24Network/commit/01a317a74c71415a12d4fca2e7083bea79c6a6dc)
- Tweaks for current build system by \@maniacbug in [`887dc6f`](https://github.com/nRF24/RF24Network/commit/887dc6f1d8bc045247cc98baa5aa31abcb56c253)
- Use 8s interval for readings by \@maniacbug in [`962e5fa`](https://github.com/nRF24/RF24Network/commit/962e5fa86869a1c236bc0eed56b5c48dba117d4c)
- Jamfile changes to fit in all-up Arduino jam-based build system by \@maniacbug in [`9d56d5a`](https://github.com/nRF24/RF24Network/commit/9d56d5a09741ab1a33b721d6b80de726a35b9ca6)
- Bring more examples into all-up jam-based build system by \@maniacbug in [`7a7bbfb`](https://github.com/nRF24/RF24Network/commit/7a7bbfb6d4e217c6558e0648d754142483b21141)
- Better handling for test mode.  Prints a message, sends more often by \@maniacbug in [`0281913`](https://github.com/nRF24/RF24Network/commit/0281913a0bd2181c3d316d659423f815bed16b55)
- Slight refactoring of eeprom handling. by \@maniacbug in [`b550786`](https://github.com/nRF24/RF24Network/commit/b5507868eba0e99bc44a79349441565908176275)
- More messaging around node config changes by \@maniacbug in [`5042b2c`](https://github.com/nRF24/RF24Network/commit/5042b2ca7d67316655ac22128479a3b34c3c456f)
- Make it easy to send a packet to your direct parent without having to work out the exact address by \@maniacbug in [`629c333`](https://github.com/nRF24/RF24Network/commit/629c333ce2a53cd35781c62c7016f1c79635ddcf)
- Send test packet to parent, not all the way to base.  Factor out voltage/temp-sending helpers by \@maniacbug in [`832fb9a`](https://github.com/nRF24/RF24Network/commit/832fb9ae34a9f6f59d8067ff8efe4095415c6696)
- Respond to test message with calibration messages by \@maniacbug in [`7432b0d`](https://github.com/nRF24/RF24Network/commit/7432b0df6dc996fbdaa9a4afb09ca90e574a83ee)
- Receive and print calibration messages by \@maniacbug in [`ecff492`](https://github.com/nRF24/RF24Network/commit/ecff49265dda8c43c3c2b15a66acca69bf598e38)
- Only send calibration request messages during calibration mode by \@maniacbug in [`d17ff67`](https://github.com/nRF24/RF24Network/commit/d17ff675faedca192803a2b078c037528e2e7cc5)
- Automatically collects calibration data by \@maniacbug in [`d9e291b`](https://github.com/nRF24/RF24Network/commit/d9e291b8de0043af07ab9425479a70fe7055a972)
- Automatically applies calibration data by \@maniacbug in [`1092c6c`](https://github.com/nRF24/RF24Network/commit/1092c6ce55007fb92f247dc5ec68850fe017c64f)
- Commit temp calibration to eeprom by \@maniacbug in [`01ded76`](https://github.com/nRF24/RF24Network/commit/01ded76e31890084d8b0f0d0c76b23d9434aabe3)
- Make sending to ones own parent more explicit by \@maniacbug in [`cb6d43a`](https://github.com/nRF24/RF24Network/commit/cb6d43abdf20736d278c74d89a8edbc613d046c1)
- Increase sending period to 30 seconds by \@maniacbug in [`b66483b`](https://github.com/nRF24/RF24Network/commit/b66483beee34ce5a8b031d60ae300a104b05af46)
- Handle the case here read() gets a 0 len by \@maniacbug in [`5953473`](https://github.com/nRF24/RF24Network/commit/5953473078b2f51ab36e12ed45525b6327cdcc85)
- Harden against accidentally setting node address to 0 by \@maniacbug in [`a9961f2`](https://github.com/nRF24/RF24Network/commit/a9961f2c4396ecd4f4d38081589778d299c17258)
- Initial implemntation of system config k/K messages by \@maniacbug in [`174609e`](https://github.com/nRF24/RF24Network/commit/174609eb116bf9fa5a0e5c8b0a78d795d970d333)
- Reliably fetch system config from parent by \@maniacbug in [`42977db`](https://github.com/nRF24/RF24Network/commit/42977db343674e17caee772e603550086381675e)
- Changes needed for relay nodes to deal with problems found in deployment by \@maniacbug in [`59b347e`](https://github.com/nRF24/RF24Network/commit/59b347e68b7ed72fccc4314bf0e507cf1c6e5446)
- Account for an 'invalid' radio by \@maniacbug in [`bd2bc20`](https://github.com/nRF24/RF24Network/commit/bd2bc20a93ef35dc70ed0b67dc2c6e3c86456848)
- Raspberry Pi support by \@TMRh20 in [`1fa44cc`](https://github.com/nRF24/RF24Network/commit/1fa44cce04661714e3ad4e6ddba8c7d95213e944)
- Https://github.com/TMRh20/RF24/pull/2 by \@TMRh20 in [`fab8a65`](https://github.com/nRF24/RF24Network/commit/fab8a65bcdd63f1fcff24fc0ea98fd7fedcd8034)
- Update makefile and examples by \@TMRh20 in [`37406db`](https://github.com/nRF24/RF24Network/commit/37406db3d3d5927545b9782978d9420b6427436a)
- Modify for Due support per \#4 by \@TMRh20 in [`fe1fe3b`](https://github.com/nRF24/RF24Network/commit/fe1fe3b7137c55e9510b2f21549138ed0b9b53fd)
- Updated examples: Addressing by \@TMRh20 in [`5c0efe5`](https://github.com/nRF24/RF24Network/commit/5c0efe5e7b99ec85acedadb4e89ea76efc577d84)
- Select a standard pin layout for the examples. by \@reixd in [`f39c958`](https://github.com/nRF24/RF24Network/commit/f39c9582359b1da1802f98272cf7f4172bbfc85b)
- Merge pull request \#23 from reixd/examples by \@TMRh20 in [`5014442`](https://github.com/nRF24/RF24Network/commit/50144424192be8cd1598f4986be5d8d52228f6ad)
- Update Doxyfile to current by \@TMRh20 in [`0be242c`](https://github.com/nRF24/RF24Network/commit/0be242c3d33aacf71650afdc03cded9aff95014b)
- Update readme for RF24 RPi/Arduino code merge by \@TMRh20 in [`9b1b674`](https://github.com/nRF24/RF24Network/commit/9b1b67407adce536693b4fec4c1f918af649976f)
- Correct readme for RF24 core lib install on RPi by \@TMRh20 in [`420b4a7`](https://github.com/nRF24/RF24Network/commit/420b4a7b2b5f67b57e59e577bb24b3163965653b)
- Python wrapper for RPi added in [`cba2626`](https://github.com/nRF24/RF24Network/commit/cba26260a2582b298df37f04970f719225c3cde5)
- Merge pull request \#27 from mz-fuzzy/master by \@TMRh20 in [`697ead9`](https://github.com/nRF24/RF24Network/commit/697ead948912d8e3ba4e43650c1c4d23620c375a)
- Intel gallileo support by \@spaniakos in [`3e46b9a`](https://github.com/nRF24/RF24Network/commit/3e46b9a395d62ff2c32ca383ddb597973f657f5b)
- Header should work also on non-Arduino ENVs by \@lnxbil in [`5a22075`](https://github.com/nRF24/RF24Network/commit/5a22075653f8c534c8f53ba5c4172952e946c686)
- Merge pull request \#36 from lnxbil/header-fix-for-non-arduino by \@TMRh20 in [`dd7d04b`](https://github.com/nRF24/RF24Network/commit/dd7d04b6f155c54de561f06793492feb1e087472)
- Merge remote-tracking branch 'upstream/master' by \@spaniakos in [`ac63091`](https://github.com/nRF24/RF24Network/commit/ac630917e7e51cff7208ce5e9469b65ca0393658)
- Merge pull request \#38 from spaniakos/master by \@TMRh20 in [`c8a3e85`](https://github.com/nRF24/RF24Network/commit/c8a3e85d3b4196476cc2295a4c31ac04c1a941b0)
- Merge pull request \#50 from ivankravets/patch-1 by \@TMRh20 in [`e32e350`](https://github.com/nRF24/RF24Network/commit/e32e350c66643aa7d5574873f8cac3c4ed7f576a)
- Merge pull request \#52 from crcastle/patch-1 by \@TMRh20 in [`546e238`](https://github.com/nRF24/RF24Network/commit/546e238897d0a0a5bd7b2c2b93ef48b3a0584845)
- Development for planned changes by \@TMRh20 in [`ad40fcb`](https://github.com/nRF24/RF24Network/commit/ad40fcb5c48694a701d82d143f4ca6a3fae1acee)
- Small update to correct issues by \@TMRh20 in [`b405da4`](https://github.com/nRF24/RF24Network/commit/b405da474f243a8782a4e463ea8675f2967c803b)
- Changes to routing by \@TMRh20 in [`213ce12`](https://github.com/nRF24/RF24Network/commit/213ce12d954cd789257d1efbddcf15b8d47955be)
- Extendend the frame struct to allow setting payload. by \@reixd in [`03a1bc7`](https://github.com/nRF24/RF24Network/commit/03a1bc7e47570c30a2316b7fa0841af1a9a64158)
- Many changes in the fragmentation by \@reixd in [`e1bfd7a`](https://github.com/nRF24/RF24Network/commit/e1bfd7ae27b727c874ae764e5aadbf162849d82e)
- Change isEmpty test by \@reixd in [`a20004f`](https://github.com/nRF24/RF24Network/commit/a20004f4f5d5576dab59fe3e7a6b45ebb23be0e9)
- Delete not needed data structure by \@reixd in [`4967084`](https://github.com/nRF24/RF24Network/commit/49670849f32182b719f36efceea3d11a6066939f)
- Merge branch 'feature/fragmentation_v2' of https://github.com/reixd/RF24Network into feature/fragmentation_v2 by \@reixd in [`fb4254c`](https://github.com/nRF24/RF24Network/commit/fb4254caa229bc7cfc904161d381c2b6c086ee96)
- Merge pull request \#13 from reixd/feature/fragmentation_v2 by \@TMRh20 in [`68aee19`](https://github.com/nRF24/RF24Network/commit/68aee19d372bef92a2afd225e0aaac6710744645)
- Arduino support for routing fragmented payloads by \@TMRh20 in [`cf2c29f`](https://github.com/nRF24/RF24Network/commit/cf2c29f37a8cda5cad60c4134141a9714260b81d)
- Per \#14 add first/last fragment, debugging cleanup by \@TMRh20 in [`a4b6076`](https://github.com/nRF24/RF24Network/commit/a4b60763ed03d1a125a79b8a4183361874c01dc0)
- \#15 Fix user-defined header types w/fragmentation by \@TMRh20 in [`6a79f9b`](https://github.com/nRF24/RF24Network/commit/6a79f9b32573fb5d643a35f274aebb9546842293)
- \#15 \#14 Improve fragmentation and transfer speed by \@TMRh20 in [`14db5e3`](https://github.com/nRF24/RF24Network/commit/14db5e34052cc0ab8f7b6f634f9c0724c130757f)
- Set MAX_USER_DEFINED_HEADER_TYPE for better readibility. by \@reixd in [`16fe4f1`](https://github.com/nRF24/RF24Network/commit/16fe4f153a43eb34d7d64876275420efeff5a243)
- Merge remote-tracking branch 'origin/TMRh20' into Development by \@TMRh20 in [`0d69e00`](https://github.com/nRF24/RF24Network/commit/0d69e00ac4a2f58508f901efa06098ab4f0ab463)
- Sending large/frag payloads on Arduino by \@TMRh20 in [`c4ddcfa`](https://github.com/nRF24/RF24Network/commit/c4ddcfa79fdcac5c549ff57c231cd6dd34392311)
- Define disable_fragmentation for Arduino by \@TMRh20 in [`f75d06e`](https://github.com/nRF24/RF24Network/commit/f75d06eaa2f0de0471f6da026f0638d1ea856c4c)
- Testing some changes/additions by \@TMRh20 in [`48de499`](https://github.com/nRF24/RF24Network/commit/48de4994494007a9d9faa832e8b7b4f98c407092)
- Testing some RPi changes/additions by \@TMRh20 in [`e3d0100`](https://github.com/nRF24/RF24Network/commit/e3d010047a53ccdcb045782edfbcf0a6b127d468)
- Merge remote-tracking branch 'origin/TMRh20' into Development by \@TMRh20 in [`ade6bd3`](https://github.com/nRF24/RF24Network/commit/ade6bd3c72d1b92cae61d097269548aa6eaa1b8e)
- Small fixes by \@TMRh20 in [`5f29c22`](https://github.com/nRF24/RF24Network/commit/5f29c228b2bbf273bce6061c3129bf4359db6e22)
- Minor updates for RF24Mesh by \@TMRh20 in [`420ca1e`](https://github.com/nRF24/RF24Network/commit/420ca1ed8bc97ce27fbe473fb11529f141cba140)
- Update config file by \@TMRh20 in [`eff7ee0`](https://github.com/nRF24/RF24Network/commit/eff7ee0a6f4636573274c81856f7f0f723ec0ad1)
- Cleanup, change non-debug printf, minor changes by \@TMRh20 in [`324e413`](https://github.com/nRF24/RF24Network/commit/324e413d942a2ecda34372da27a65c1152050e96)
- A bit of cleanup and additions for RF24Mesh testing by \@TMRh20 in [`414192f`](https://github.com/nRF24/RF24Network/commit/414192fdea4053df7d158d3c24e05f578625201d)
- Minor changes for auto-addressing reqs by \@TMRh20 in [`e7fd5a7`](https://github.com/nRF24/RF24Network/commit/e7fd5a723d43149b4cfb4df2e543cd2fb2fbcac6)
- Major changes - Network ACKs, Fixes by \@TMRh20 in [`1560be5`](https://github.com/nRF24/RF24Network/commit/1560be5ead2c525469b14e36c68bf780fa001331)
- Simplify system msg handling, add addr release by \@TMRh20 in [`9bb97f4`](https://github.com/nRF24/RF24Network/commit/9bb97f4fd99d56a5462e602e02bd82c27b08cd81)
- Forgot to add addr release for RPi by \@TMRh20 in [`4a66840`](https://github.com/nRF24/RF24Network/commit/4a66840f36010ea15aaa35f87a4b5e9a4e86a09e)
- Update readme for RF24 RPi/Arduino code merge by \@TMRh20 in [`b2a7fd3`](https://github.com/nRF24/RF24Network/commit/b2a7fd3b0dff446c0079e7741cd2f868116a679f)
- Update other readme for RF24/RPi code merge by \@TMRh20 in [`31ba314`](https://github.com/nRF24/RF24Network/commit/31ba31452d0ff66fd1619baedbdaf4a78fbe4d9b)
- Correct RF24 Core lib install info (code merge) by \@TMRh20 in [`f881426`](https://github.com/nRF24/RF24Network/commit/f881426ffb72489b5b4202b55fe81b04f0482287)
- Set default max payload to 128bytes for Arduino by \@TMRh20 in [`1dd7282`](https://github.com/nRF24/RF24Network/commit/1dd728229c632aed29c9d89a60f16e2cd3cacfd9)
- RPi - Sending multicast fragments by \@TMRh20 in [`596dd3c`](https://github.com/nRF24/RF24Network/commit/596dd3cc0b66de3f0dbe5fc44eb2abc10b845169)
- Rough Arduino/RPi code merge by \@TMRh20 in [`3b44981`](https://github.com/nRF24/RF24Network/commit/3b44981d27ccbc9754374e989586bb5842c33d15)
- Re-enable user payloads by \@TMRh20 in [`40b5824`](https://github.com/nRF24/RF24Network/commit/40b5824b63e69b0bf8600783cd3fd1bb4e31ef45)
- Intel galilleo support added by \@spaniakos in [`f404507`](https://github.com/nRF24/RF24Network/commit/f40450723723e74cddaa531884008eaf2b769864)
- Intel galileo support added by \@spaniakos in [`f56bb0a`](https://github.com/nRF24/RF24Network/commit/f56bb0aea890147f4df7e106a0818aad58b5e1de)
- Merge pull request \#33 from spaniakos/Development by \@TMRh20 in [`549cb67`](https://github.com/nRF24/RF24Network/commit/549cb670aed11359255cf6cb956a1c5895816f65)
- Reduce Errors - add 200ms delay to writes by \@TMRh20 in [`622989c`](https://github.com/nRF24/RF24Network/commit/622989cede7a919ccad0a08bbf97fc7cee2c1795)
- Intel Galileo Support by \@spaniakos in [`276a598`](https://github.com/nRF24/RF24Network/commit/276a598d8ec06d3a8438478190c35f95c0337a88)
- Delete RF24Network.o by \@spaniakos in [`1215f3d`](https://github.com/nRF24/RF24Network/commit/1215f3d78ae590073a167f146ff9f7c213d0a76e)
- Delete librf24network.so.1.0 by \@spaniakos in [`0faca8c`](https://github.com/nRF24/RF24Network/commit/0faca8c27ef9c278c7d6dd40e0888219a66420ee)
- Revert change by \@spaniakos in [`1f6b56b`](https://github.com/nRF24/RF24Network/commit/1f6b56b2feb56044bb3242c6d5c011a6f1323662)
- Merge pull request \#39 from spaniakos/Development by \@TMRh20 in [`acf9dd0`](https://github.com/nRF24/RF24Network/commit/acf9dd091e815147a48faaa75b11f7e0f43b44ac)
- Update config file for RF24Ethernet by \@TMRh20 in [`fdaf171`](https://github.com/nRF24/RF24Network/commit/fdaf171bfc8c9c9829987127b987613c8393a5ed)
- Revert 'optimization' ... dejavu by \@TMRh20 in [`001aa0e`](https://github.com/nRF24/RF24Network/commit/001aa0e6530b9d8fffef1baba5a27d8e03af3ed4)
- Code cleanup and fixes by \@TMRh20 in [`d185bd0`](https://github.com/nRF24/RF24Network/commit/d185bd0d5c84072d86c24b747ca5436621336e27)
- Revert - Sleep_mode should be disabled by default by \@TMRh20 in [`26485db`](https://github.com/nRF24/RF24Network/commit/26485db594788b22963d1ce521c788f6afe703e7)
- Update Makefile for non-RPi Linux devices by \@TMRh20 in [`c6fbaaf`](https://github.com/nRF24/RF24Network/commit/c6fbaaf46bfc5b4ff05e5cb00f446226b2f238f3)
- Update Makefile with better RPi detection by \@TMRh20 in [`c1c3a45`](https://github.com/nRF24/RF24Network/commit/c1c3a45e3db5260c5cd762bda9b5c9c211c94068)
- Return external_data_types for RPi/Linux devices by \@TMRh20 in [`aae90b7`](https://github.com/nRF24/RF24Network/commit/aae90b7b0adfc69f729a44848cae1fa693efda23)
- Revert changes to RF24Network_config.h by \@TMRh20 in [`30c5205`](https://github.com/nRF24/RF24Network/commit/30c5205f98c18b24fa78a34190e85210d81d6cf7)
- Introduce network.external_queue by \@TMRh20 in [`ce0b421`](https://github.com/nRF24/RF24Network/commit/ce0b421e21a1ac6b5a56ca2bbd44eb171a50011d)
- Allow begin() on the current channel by \@TMRh20 in [`76eb89d`](https://github.com/nRF24/RF24Network/commit/76eb89d303105550479ef09204ea8d902ccf785f)
- Reduce CPU usage, simplify reassembly on Linux/RPi by \@TMRh20 in [`22be01d`](https://github.com/nRF24/RF24Network/commit/22be01d9a6120aa307b4d99fc092953c90bb3891)
- Init frag buffers in constructor (Arduino) by \@TMRh20 in [`3d0eff5`](https://github.com/nRF24/RF24Network/commit/3d0eff5e45db4e3eb3b74e61595bb20f8666604a)
- Merge remote-tracking branch 'origin/Development' by \@TMRh20 in [`ceecf38`](https://github.com/nRF24/RF24Network/commit/ceecf381e207b4822eaca5c0afd9010056b0285e)
- Examples should be excluded from platformio library.json file by \@gehel in [`759e805`](https://github.com/nRF24/RF24Network/commit/759e8055233863d3bbb399e23f8ca95c95e19e24)
- Merge pull request \#58 from gehel/master by \@TMRh20 in [`cb56755`](https://github.com/nRF24/RF24Network/commit/cb56755e96a21e6921a9258d795c20ae3b47b685)
- Test chgs for backwards compat with clones by \@TMRh20 in [`5b366aa`](https://github.com/nRF24/RF24Network/commit/5b366aa9b62a57bb0ad59432ee8d5577bcad030c)
- Re-enable dynamic payloads by default by \@TMRh20 in [`27446c3`](https://github.com/nRF24/RF24Network/commit/27446c3cadbc9e3020fb1b92f21d3a79ec359a7e)
- Merge branch 'Development' by \@TMRh20 in [`5e62300`](https://github.com/nRF24/RF24Network/commit/5e623005df7061a1c32f4301496130e29add17a6)
- Create library.properties by \@Avamander in [`a08de45`](https://github.com/nRF24/RF24Network/commit/a08de45ed6de302ee1ff7fae921d92d70b6dd042)
- Merge pull request \#60 from Avamander/patch-1 by \@TMRh20 in [`bb41408`](https://github.com/nRF24/RF24Network/commit/bb41408b93bc5fda7538326bcad86bf2a1f37ca1)

[1.0]: https://github.com/nRF24/RF24Network/compare/f54c0ac0ee74d2fc9490740004bd3222b170ae85...v1.0

Full commit diff: [`f54c0ac...v1.0`][1.0]

## New Contributors
* \@TMRh20 made their first contribution
* \@Avamander made their first contribution
* \@gehel made their first contribution
* \@crcastle made their first contribution
* \@ivankravets made their first contribution
* \@dsbaha made their first contribution
* \@spaniakos made their first contribution
* \@lnxbil made their first contribution
* \@flavio-fernandes made their first contribution
* \@ made their first contribution
* \@reixd made their first contribution
* \@maniacbug made their first contribution
<!-- generated by git-cliff -->
