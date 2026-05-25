# AGENTS

Project type: Arduino library that can be compiled for Linux and the Pico SDK.

## Building docs

requires: [doxygen]

steps:

- `cd docs`
- `doxygen`
- optional: open `docs/html/index.html`

## Build/Install library

requires: `cmake`, build tools (and `sudo` for install)

steps:

- `cmake -B build -S . [...cmake options]`
- `cmake --build build`
- `sudo cmake --install build` (required to build Linux examples)

### Build environment & dependency ordering

- Dependency ordering (use git HEAD of `master` unless specified):
  1. [RF24] (build & install)
  2. `RF24Network` (build)
- Example commands to install dependencies from source (repeat for each
  dependency):
  - `git clone <repo> && cd <repo>`
  - `cmake -B build -S .`
  - `cmake --build build`
  - `sudo cmake --install build`

## Examples (RPi/Linux)

requires: `cmake`, build tools

steps:

- `cmake -B examples_RPi/build examples_RPi [...cmake options]`
- `cmake --build examples_RPi/build`

## Pico SDK examples

requires: `cmake`, [PicoSDK] (Windows: `ninja` may be required)

steps:

- `cmake -B examples_pico/build examples_pico [...cmake opts]`
- `cmake --build examples_pico/build`

### About [RF24] dependency

The `examples_pico/CMakeLists.txt` expects [RF24] repo checked out adjacent to
this repo's root folder. Unless otherwise noted, [RF24] master branch tip is
used.

## Install Python wrapper

requires:

- `python`, `pip`, and Python C API headers
- `boost.python` (typically provided via `apt-get install`)

steps:

- Create a venv (can be done with various tools). Need only be done once.
- Activate the venv (depends on method used in previous step), if not already in
  active venv.
- `pip install -v ./RPi/pyRF24Network`

Notes about dependencies:

- Most dependencies can be installed via
  `apt-get install python3-dev libboost-python-dev`.
- The [RF24] (C++ lib) is required to be installed beforehand too.

## Format sources

requires: `clang-format` v14.x

steps:

- `clang-format-14 -i --style=file <file>`
- On Windows (powershell does not expand globs):
  `clang-format -i --style=file <file>`

## Clang-tidy / static analysis

Note: clang-tidy can only be used when targeting Linux builds. Prefer WSL when
building on a Windows host machine.

- To generate a compilation database required by `clang-tidy`:
  - `cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`
- Run `clang-tidy` with the `-p` option pointing to the build directory:
  - `clang-tidy -p build <files>`

Notes about [RF24] dependency:

- By default the CMakeLists will `find_library(RF24 rf24 REQUIRED)`
- To build RF24Network, [RF24] needs to be installed.

[RF24]: https://github.com/nRF24/RF24
[PicoSDK]: https://github.com/raspberrypi/pico-sdk
[doxygen]: https://github.com/doxygen/doxygen
