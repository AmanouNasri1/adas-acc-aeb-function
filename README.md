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

## Results (SiL)

From automated KPI evaluation:

- Lead brake scenario (`lead_brake.csv`): AEB time **0.0 s**, min TTC **2.65 s**, emergency jerk used when TTC < 3.0 s.
- Cruise step (`cruise_step.csv`): steady-state speed error **0.07 m/s**, comfort jerk <= **2.0 m/s³**.
- Follow constant lead (`follow_constant_lead.csv`): steady-state time-gap error **0.00 s**, comfort jerk <= **2.0 m/s³**.