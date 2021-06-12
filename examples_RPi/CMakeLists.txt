cmake_minimum_required(VERSION 3.12)

# iterate over a list of examples by filename
set(EXAMPLES_LIST
    helloworld_rx
    helloworld_tx
    rx-test
)

project(RF24LogExamples)
add_compile_options(-Ofast -Wall) # passing the compiler a `-pthread` flag doesn't work here

# detect the CPU make and type
include(../cmake/detectCPU.cmake) # sets the variable SOC accordingly

find_library(RF24 rf24 REQUIRED)
message(STATUS "using RF24 library: ${RF24}")

find_library(RF24Network rf24network REQUIRED)
message(STATUS "using RF24Network library: ${RF24Network}")

foreach(example ${EXAMPLES_LIST})
    # make a target
    add_executable(${example} ${example}.cpp)

    # link the RF24 lib to the target. Notice we specify pthread as a linked lib here
    target_link_libraries(${example} PUBLIC ${RF24} pthread ${RF24Network})
endforeach()
