cmake_minimum_required(VERSION 4.3)
project(ogurec)

set(CMAKE_CXX_STANDARD 26)
set(CXX_EXTENSIONS ON)
set(CMAKE_CXX_FLAGS "-freflection -fconcepts-diagnostics-depth=4")

set(SRCS
        src/main.cpp
        src/io.hpp
        src/conn.hpp
        src/proto.hpp
        src/server.hpp
        src/client.hpp
        src/proxy.hpp
        src/game/server.hpp
        src/bitset.hpp
        src/game/player.hpp
        src/game/inventory.hpp
        src/game/buff.hpp
        src/types.hpp
        src/game/world.hpp
        src/misc.hpp
        src/test.hpp
        src/game/file.hpp
        src/meta.hpp
)

find_package(ZLIB REQUIRED)

add_executable(ogurec ${SRCS})
target_link_libraries(ogurec PUBLIC ZLIB::ZLIB)
