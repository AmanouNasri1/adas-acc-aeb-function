# ACC/AEB Function (C++)

A minimal **Software-in-the-Loop (SiL)** project for an ACC + AEB longitudinal function:
- finite-state mode logic (OFF / CRUISE / FOLLOW / AEB / FAULT)
- simple closed-loop longitudinal plant
- scenario replay from CSV + CSV logging

## Build (Ubuntu)
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure