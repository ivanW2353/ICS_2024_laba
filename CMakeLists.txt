cmake_minimum_required(VERSION 3.15)
project(lc3-assembler)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the language students will use for the lab assignments. C++ is used by default.
option(USE_CPP "Set to ON if the student is using C++, otherwise set to OFF for C" ON)

if (USE_CPP)
    add_compile_definitions(USE_CPP)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(AddCLI11)

add_library(assembler
    src/token.cpp
    src/parser.cpp
    src/operand.cpp
    src/instruction.cpp
    src/assembler.cpp
)

# Add the solution file to the target. If the student is using C++, the solution file is
# `solution/solution.cpp`, otherwise it is `solution/solution.c`.
if (USE_CPP)
    target_sources(assembler PRIVATE solution/solution.cpp)
else()
    # Set `solution/solution.c` to the C programming language, so that CMake will use a C compiler
    # to compile it.
    set_source_files_properties(solution/solution.c PROPERTIES LANGUAGE C)
    target_sources(assembler PRIVATE solution/solution.c)
endif()

target_include_directories(assembler PUBLIC include)
target_compile_options(assembler
    PUBLIC
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /Zc:preprocessor>
)

add_executable(lc3-assembler src/main.cpp)
target_link_libraries(lc3-assembler PRIVATE assembler CLI11::CLI11)

enable_testing()

include(AddGoogleTest)
add_subdirectory(unittest)
add_subdirectory(test)

if (USE_CPP)
    message("")
    message("==========================================================")
    message("  You are currently using C++ to complete the lab.")
    message("  You should finish the lab in `solution/solution.cpp`.")
    message("  To switch to C, rerun cmake with the definition:")
    message("      -DUSE_CPP=OFF")
    message("==========================================================")
    message("")
else()
    message("")
    message("==========================================================")
    message("  You are currently using C to complete the lab.")
    message("  You should finish the lab in `solution/solution.c`.")
    message("  To switch to C++, rerun cmake with the definition:")
    message("      -DUSE_CPP=ON")
    message("==========================================================")
    message("")
endif()
