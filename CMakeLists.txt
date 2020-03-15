cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(vector_clocks VERSION 1.0.0)

string(APPEND CMAKE_CXX_FLAGS " -Werror -Wall -Wextra -Wcast-align -Wcast-qual -Wfloat-equal -Wmissing-field-initializers -Wmissing-include-dirs -Wpointer-arith -Wstack-protector -Wstrict-aliasing=2 -Wunreachable-code -Wlogical-op -Wformat=2 -Wduplicated-cond -Wnull-dereference -Wrestrict -Wno-unknown-pragmas -Wno-unused-function -Wno-deprecated-copy -Wno-pessimizing-move")
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -g -O0")
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O3 -march=native -funroll-loops -flto -fwhole-program -DNDEBUG")

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

set(EXE_NAME vector_clocks_app)

set(
    EXE_UIS
    include/main_window.ui
)

set(
    EXE_HEADERS
    include/actor_id.hpp
    include/error.hpp
    include/hton.hpp
    include/main_window.hpp
    include/ntoh.hpp
    include/source_location.hpp
    include/vector_timestamp.hpp
)

set(
    EXE_SOURCES
    src/actor_id.cpp
    src/error.cpp
    src/main.cpp
    src/main_window.cpp
    src/vector_timestamp.cpp
)

add_executable(
    ${EXE_NAME}
    ${EXE_UIS}
    ${EXE_HEADERS}
    ${EXE_SOURCES}
)

target_link_libraries(${EXE_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

target_include_directories(
    ${EXE_NAME} 
    PRIVATE 
    ${vector_clocks_SOURCE_DIR}/include
    ${vector_clocks_SOURCE_DIR}/external/philslib/include
    ${vector_clocks_SOURCE_DIR}/external/optional/include
    ${vector_clocks_SOURCE_DIR}/external/expected/include
)

