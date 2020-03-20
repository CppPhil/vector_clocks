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

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

add_subdirectory(external/googletest)
add_subdirectory(external/fmtlib)

execute_process(
    COMMAND bash ${vector_clocks_SOURCE_DIR}/scripts/build_jaeger_client_cpp.sh
)

set(LIB_NAME vector_clocks_lib)

set(
    LIB_HEADERS
    include/actor_id.hpp
    include/error.hpp
    include/hton.hpp
    include/ntoh.hpp
    include/source_location.hpp
    include/vector_timestamp.hpp
    include/packet.hpp
    include/logger.hpp
    include/log_level.hpp
    include/server.hpp
    include/server_port.hpp
    include/client.hpp
)

set(
    LIB_SOURCES
    src/actor_id.cpp
    src/error.cpp
    src/vector_timestamp.cpp
    src/packet.cpp
    src/logger.cpp
    src/log_level.cpp
    src/server.cpp
    src/client.cpp
)

add_library(
    ${LIB_NAME}
    OBJECT
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_link_libraries(
    ${LIB_NAME}
    PUBLIC
    Threads::Threads
)

target_link_libraries(
    ${LIB_NAME}
    PUBLIC
    fmt::fmt
)

target_link_libraries(${LIB_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

target_link_libraries(${LIB_NAME} PUBLIC ${vector_clocks_SOURCE_DIR}/external/jaeger-client-cpp/build/libjaegertracing.a)

target_include_directories(
    ${LIB_NAME} 
    PUBLIC 
    ${vector_clocks_SOURCE_DIR}/include
    ${vector_clocks_SOURCE_DIR}/external/philslib/include
    ${vector_clocks_SOURCE_DIR}/external/optional/include
    ${vector_clocks_SOURCE_DIR}/external/expected/include
    ${vector_clocks_SOURCE_DIR}/external/jaeger-client-cpp/src
    ${vector_clocks_SOURCE_DIR}/external/jaeger-client-cpp/build/src
)

set(EXE_NAME vector_clocks_app)

add_executable(
    ${EXE_NAME}
    include/main_window.hpp
    src/main.cpp
    src/main_window.cpp
)

target_link_libraries(
    ${EXE_NAME}
    PRIVATE
    ${LIB_NAME}
)

set(TEST_NAME vector_clocks_tests)

set(
    TEST_SOURCES
    tests/src/main.cpp
    tests/src/ntoh.cpp
    tests/src/hton.cpp
    tests/src/vector_timestamp.cpp
    tests/src/packet.cpp
)

add_executable(
    ${TEST_NAME}
    ${TEST_HEADERS}
    ${TEST_SOURCES}
)

target_link_libraries(${TEST_NAME} PRIVATE ${LIB_NAME} gtest)
