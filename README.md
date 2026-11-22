# visual-novel-video-generator
C++ application to generate visual novel style videos.


## Building the project

### Linux (GCC / Clang)

```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### Windows (Visual Studio / MSVC)

```
mkdir build && cd build
cmake .. -A x64
cmake --build . --config Debug
```

### VS Code

Use **Run Build Task** to select one of the four platform and configuration tasks:

- `Debug Build for windows`
- `Debug Build for linux`
- `Release Build for windows`
- `Release Build for linux`

`Build: select target and platform` provides prompts for the target and platform.
Windows tasks require Visual Studio/MSVC, while Linux tasks must run from a
Linux host (including VS Code connected to WSL).

## External Dependencies

- [kuba--/zip](https://github.com/kuba--/zip) - A portable, simple zip library written in C.
