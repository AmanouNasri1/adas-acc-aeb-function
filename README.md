# ACC/AEB Function (C++)

Repo skeleton: CMake + GoogleTest + CI.

## Build
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure