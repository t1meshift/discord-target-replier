# Discord Target Replier

A simple Discord bot for replying messages from a given user.

## Requirements

* C++17 compiler
* CMake 3.22+
* vcpkg

## Building

1. Install vcpkg (https://vcpkg.io/en/getting-started.html)
2. Generate the project with
`-DCMAKE_TOOLCHAIN_FILE=<vcpkg_absolute_path>\vcpkg\scripts\buildsystems\vcpkg.cmake`
3. Create `config.json` file ([example](config.example.json))
4. Build and run it