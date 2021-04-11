## Include this file if you want to use the RF24Network library
## in YOUR (Pico) project.
## See examples_pico/CMakeLists.txt to see how it could be done

cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Define the RF24Network library
add_library(RF24Network INTERFACE)

target_sources(RF24Network INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/RF24Network.cpp
    )

target_include_directories(RF24Network INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/
    )
