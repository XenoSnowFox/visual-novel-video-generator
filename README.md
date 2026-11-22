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

## External Dependencies

- [kuba--/zip](https://github.com/kuba--/zip) - A portable, simple zip library written in C.
