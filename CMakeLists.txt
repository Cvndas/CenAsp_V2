cmake_minimum_required(VERSION 3.16)
project(asp_v8 C CXX)

# Set C standard to C11 for modern C features
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard for V8 wrapper
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# CUSTOM: Required for clangd to work (in vscode, probably elsewhere too)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ASAN is WIP
set (CMAKE_C_FLAGS "-g -Wall -fsanitize=address,leak")
set (CMAKE_CXX_FLAGS "-g -Wall -fsanitize=address,leak")

# set (CMAKE_C_FLAGS "-g -Wall")
# set (CMAKE_CXX_FLAGS "-g")


# Adjust these paths if needed
set(V8_ROOT "/home/asp/v8")
set(V8_INCLUDE_DIR "${V8_ROOT}/include")
set(V8_LIB_DIR "${V8_ROOT}/out.gn/x64.release.sample/obj")

# Add include directories
include_directories(${V8_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link directories
link_directories(${V8_LIB_DIR})

# Create the V8 wrapper library (only the C++ wrapper)
add_library(v8wrapper
        src/v8_api_access.cc
        include/v8_api_access.h
)

# Add V8 specific compile definitions
target_compile_definitions(v8wrapper PRIVATE
    V8_COMPRESS_POINTERS
    V8_TARGET_ARCH_X64
)

# If you're on a 64-bit system, add these as well
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(v8wrapper PRIVATE
        V8_ENABLE_SANDBOX
        V8_31BIT_SMIS_ON_64BIT_ARCH
        V8_ENABLE_POINTER_COMPRESSION
    )
endif()

target_link_libraries(v8wrapper PRIVATE v8_monolith pthread dl)

include(sourcefiles.cmake)

# Create the main application
add_executable(asp_v8
    ${SOURCE_FILES}
)

# Link the main application with our v8wrapper
target_link_libraries(asp_v8 PRIVATE v8wrapper)

# Add -rdynamic to linker flags for USDT/bpftrace support
target_link_libraries(asp_v8 PRIVATE -rdynamic)

add_custom_target(codegrade_tests COMMAND
    ${CMAKE_COMMAND} -E tar "cfv" "codegrade_tests.zip" --format=zip
       "${CMAKE_CURRENT_SOURCE_DIR}/runtests.py"
       "${CMAKE_CURRENT_SOURCE_DIR}/tests/"
       "${CMAKE_CURRENT_SOURCE_DIR}/apps/"
       "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

add_custom_target(submission COMMAND
    ${CMAKE_COMMAND} -E tar "cfv" "submission.zip" --format=zip
       "${CMAKE_CURRENT_SOURCE_DIR}/src/*"
       "${CMAKE_CURRENT_SOURCE_DIR}/include/*"
       "${CMAKE_CURRENT_SOURCE_DIR}/sourcefiles.cmake")