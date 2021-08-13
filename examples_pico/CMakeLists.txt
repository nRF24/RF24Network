cmake_minimum_required(VERSION 3.12)

#[[
    for these examples we are assuming the following structure:
    path/to/github/repos/
        RF24/
        RF24Network/
            examples_pico/  <-- you are here
        pico-sdk/

    Be sure to set the PICO_SDK_PATH environment variable
        `export PICO_SDK_PATH=/path/to/github/repos/pico-sdk/`
]]

# Pull in SDK (must be before project)
include(../../RF24/cmake/pico_sdk_import.cmake)

project(pico_examples C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# In YOUR project, include RF24's CMakeLists.txt
# giving the path depending on where the library
# is cloned to in your project
include(../../RF24/CMakeLists.txt)  # for RF24 lib
include(../CMakeLists.txt)   # for RF24Network lib

# iterate over a list of examples by name
set(EXAMPLES_LIST
    helloworld_rx
    helloworld_tx
)

foreach(example ${EXAMPLES_LIST})
    # make a target
    add_executable(${example} ${example}.cpp defaultPins.h)

    # link the necessary libs to the target
    target_link_libraries(${example} PUBLIC
        RF24
        RF24Network
        pico_stdlib
        hardware_spi
        hardware_gpio
    )

    # specify USB port as default serial communication's interface (not UART RX/TX pins)
    pico_enable_stdio_usb(${example} 1)
    pico_enable_stdio_uart(${example} 0)

    # create map/bin/hex file etc.
    pico_add_extra_outputs(${example})
endforeach()
