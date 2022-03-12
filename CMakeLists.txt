# Check if we are building a pico-sdk based project
# (or more exactly: if we just got included in a pico-sdk based project)
if (PICO_SDK_PATH)
    # If so, load the relevant CMakeLists-file but don't do anything else
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/usePicoSDK.cmake)
    return()
endif()


############################
# for non-RPi-Pico platforms
############################
cmake_minimum_required(VERSION 3.15)

# Set the project name to your project name
project(RF24Network C CXX)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/StandardProjectSettings.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/PreventInSourceBuilds.cmake)

# get library info from Arduino IDE's required library.properties file
include(${CMAKE_CURRENT_LIST_DIR}/cmake/GetLibInfo.cmake) # sets the variable LibTargetName

# detect any applicable external libs (like pigpio)
include(cmake/AutoConfig_RF24_DRIVER.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(${LibTargetName}_project_options INTERFACE)
target_compile_features(${LibTargetName}_project_options INTERFACE cxx_std_17)
add_compile_options(-Ofast -Wall)

# allow using CMake options to adjust RF24Network_config.h without modiying source code
option(SERIAL_DEBUG "enable/disable general debugging output" OFF)
option(SERIAL_DEBUG_MINIMAL "enable/disable minimal debugging output" OFF)
option(SERIAL_DEBUG_ROUTING
    "enable/disable debugging output related to transmission routing"
    OFF
)
option(SERIAL_DEBUG_FRAGMENTATION
    "enable/disable debugging output related to message fragmentation"
    OFF
)
option(SERIAL_DEBUG_FRAGMENTATION_L2
    "enable/disable debugging output related to fragmented messages' transmission success"
    OFF
)
option(DISABLE_FRAGMENTATION "disable message fragmentation" OFF)
option(DISABLE_DYNAMIC_PAYLOADS "force usage of static payload size" OFF)

# detect CPU and add compiler flags accordingly
include(cmake/detectCPU.cmake)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
    if(ENABLE_BUILD_WITH_TIME_TRACE)
        add_compile_definitions(project_options INTERFACE -ftime-trace)
    endif()
endif()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(${LibTargetName}_project_warnings INTERFACE)

# enable cache system
include(${CMAKE_CURRENT_LIST_DIR}/cmake/Cache.cmake)

# standard compiler warnings
include(${CMAKE_CURRENT_LIST_DIR}/cmake/CompilerWarnings.cmake)
set_project_warnings(${LibTargetName}_project_warnings)

# setup CPack options
include(${CMAKE_CURRENT_LIST_DIR}/cmake/CPackInfo.cmake)

if(NOT DEFINED USE_RF24_LIB_SRC)
    find_library(RF24 rf24 REQUIRED)
    message(STATUS "using RF24 library: ${RF24}")
endif()

###########################
# create target for bulding the RF24Network lib
###########################
add_library(${LibTargetName} SHARED
    RF24Network.cpp
)

target_include_directories(${LibTargetName} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

if(DEFINED USE_RF24_LIB_SRC OR pybind11_FOUND OR SKBUILD)
    message(STATUS "Building lib from RF24 source")
    target_compile_definitions(${LibTargetName} PUBLIC USE_RF24_LIB_SRC)
    if(NOT DEFINED RF24_LIB_PATH)
        target_include_directories(${LibTargetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../RF24)
    else()
        target_include_directories(${LibTargetName} PUBLIC ${RF24_LIB_PATH})
    endif()
    target_link_libraries(${LibTargetName}
        INTERFACE ${LibTargetName}_project_options
        INTERFACE ${LibTargetName}_project_warnings
    )

else()
    target_link_libraries(${LibTargetName}
        INTERFACE ${LibTargetName}_project_options
        INTERFACE ${LibTargetName}_project_warnings
        SHARED ${RF24}
    )
endif()

set_target_properties(
    ${LibTargetName}
    PROPERTIES
    SOVERSION ${${LibName}_VERSION_MAJOR}
    VERSION ${${LibName}_VERSION_STRING}
)

# assert the appropriate preprocessor macros for RF24Network_config.h
if(SERIAL_DEBUG)
    message(STATUS "SERIAL_DEBUG asserted")
    target_compile_definitions(${LibTargetName} PUBLIC SERIAL_DEBUG)
endif()
if(SERIAL_DEBUG_MINIMAL)
    message(STATUS "SERIAL_DEBUG_MINIMAL asserted")
    target_compile_definitions(${LibTargetName} PUBLIC SERIAL_DEBUG_MINIMAL)
endif()
if(SERIAL_DEBUG_ROUTING)
    message(STATUS "SERIAL_DEBUG_ROUTING asserted")
    target_compile_definitions(${LibTargetName} PUBLIC SERIAL_DEBUG_ROUTING)
endif()
if(SERIAL_DEBUG_FRAGMENTATION)
    message(STATUS "SERIAL_DEBUG_FRAGMENTATION asserted")
    target_compile_definitions(${LibTargetName} PUBLIC SERIAL_DEBUG_FRAGMENTATION)
endif()
if(SERIAL_DEBUG_FRAGMENTATION_L2)
    message(STATUS "SERIAL_DEBUG_FRAGMENTATION_L2 asserted")
    target_compile_definitions(${LibTargetName} PUBLIC SERIAL_DEBUG_FRAGMENTATION_L2)
endif()
if(DISABLE_FRAGMENTATION)
    message(STATUS "DISABLE_FRAGMENTATION asserted")
    target_compile_definitions(${LibTargetName} PUBLIC DISABLE_FRAGMENTATION)
endif()
if(DISABLE_DYNAMIC_PAYLOADS)
    message(STATUS "DISABLE_DYNAMIC_PAYLOADS asserted")
    target_compile_definitions(${LibTargetName} PUBLIC DISABLE_DYNAMIC_PAYLOADS)
endif()
# for MAX_PAYLOAD_SIZE, we let the default be configured in source code
if(DEFINED MAX_PAYLOAD_SIZE) # don't use CMake's `option()` for this one
    message(STATUS "MAX_PAYLOAD_SIZE set to ${MAX_PAYLOAD_SIZE}")
    target_compile_definitions(${LibTargetName} PUBLIC MAX_PAYLOAD_SIZE=${MAX_PAYLOAD_SIZE})
endif()
if(DEFINED SLOW_ADDR_POLL_RESPONSE)
    message(STATUS "SLOW_ADDR_POLL_RESPONSE set to ${SLOW_ADDR_POLL_RESPONSE}")
    target_compile_definitions(${LibTargetName} PUBLIC SLOW_ADDR_POLL_RESPONSE=${SLOW_ADDR_POLL_RESPONSE})
endif()
# conditionally disable interruot support (a pigpio specific feature)
if("${LibPIGPIO}" STREQUAL "LibPIGPIO-NOTFOUND" OR DEFINED RF24_NO_INTERRUPT)
    message(STATUS "Disabling IRQ pin support")
    target_compile_definitions(${LibTargetName} PUBLIC RF24_NO_INTERRUPT)
endif()

###########################
# target install rules for the RF24Network lib
###########################
install(TARGETS ${LibTargetName}
    DESTINATION lib
)

install(FILES
        RF24Network.h
        RF24Network_config.h
    DESTINATION include/RF24Network
)

# CMAKE_CROSSCOMPILING is only TRUE when CMAKE_TOOLCHAIN_FILE is specified via CLI
if("${CMAKE_CROSSCOMPILING}" STREQUAL "FALSE")
    install(CODE "message(STATUS \"Updating ldconfig\")")
    install(CODE "execute_process(COMMAND ldconfig)")
endif()
