# ogurec

A Terraria server written from scratch in C++26, based on reverse engineering
the network protocol and the world file format. Packets and file structures
are plain structs; a serializer built on C++26 static reflection reads and
writes them by walking their members at compile time, with annotations for
per-field encoders, compression and flags.

Work in progress: the server accepts one client and goes through the start of
the join handshake (version check, player slot, world metadata) with mock
world data. The serializer in `src/io.hpp` is the prototype of
[lzfmt](https://github.com/1zcnt/lzfmt) and is being replaced by it.

## Build

Needs GCC 16 (`-freflection`), CMake 4.3 and zlib.

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
```

## Usage

```
ogurec [host] [port] [password]      # defaults: 0.0.0.0 7777, no password
```

```sh
$ build/ogurec 127.0.0.1 7777
listening on 127.0.0.1:7777
```
