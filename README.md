# ACC/AEB Longitudinal Function (C++)

[![CI](https://github.com/AmanouNasri1/adas-acc-aeb-function/actions/workflows/ci.yml/badge.svg)](https://github.com/AmanouNasri1/adas-acc-aeb-function/actions/workflows/ci.yml)

Minimal **Software-in-the-Loop (SiL)** ACC + AEB demonstrator with:
- FSM modes: **OFF / CRUISE / FOLLOW / AEB / FAULT**
- Closed-loop longitudinal plant
- Scenario replay from CSV + logging
- KPI evaluation + automated regression tests (GoogleTest + GitHub Actions)

## Quickstart (Ubuntu)
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
Run scenarios + KPIs
mkdir -p results

./build/sim_runner --scenario scenarios/lead_brake.csv --out results/lead_brake.csv
python3 tools/evaluate_kpis.py results/lead_brake.csv 0.02 3.0 1.5 3.0

./build/sim_runner --scenario scenarios/cruise_step.csv --out results/cruise.csv
python3 tools/evaluate_kpis.py results/cruise.csv 0.02 3.0 1.5 3.0

./build/sim_runner --scenario scenarios/follow_constant_lead.csv --out results/follow.csv
python3 tools/evaluate_kpis.py results/follow.csv 0.02 3.0 1.5 3.0
Results snapshot (SiL)

From automated KPI evaluation:

Scenario	Key outcome
lead_brake.csv	AEB time 0.0 s, min TTC 2.65 s, emergency jerk used when TTC < 3.0 s
cruise_step.csv	steady-state speed error 0.07 m/s, comfort jerk ≤ 2.0 m/s³
follow_constant_lead.csv	steady-state time-gap error 0.00 s, comfort jerk ≤ 2.0 m/s³
Verification

Unit tests: FSM behavior, plausibility handling, TTC correctness

Scenario tests: KPI thresholds enforced in CI

Requirements traceability: see docs/traceability.md

Repository layout

include/acc/ interfaces + config

src/acc/ FSM, plausibility, controllers, limiters

src/sim/ scenario loader + sim runner

scenarios/ input CSV scenarios

tools/ KPI evaluation scripts

tests/ unit + scenario tests

docs/ requirements + traceability

Limitations

This is a minimal SiL demonstrator (not production ECU code). Comfort constraints are overridden in risk region (TTC-based emergency jerk), and AEB overrides everything.