# Requirements Traceability

| Req | Description (short) | Verification |
|-----|----------------------|--------------|
| R1  | ACC disabled -> OFF + a_cmd=0 | Unit test: `Fsm.OffWhenDisabled` |
| R2  | a_cmd limited to [a_min,a_max] | KPI: `a_cmd_range_mps2` within limits (scenarios) |
| R3  | CRUISE steady-state error <= 0.5 m/s | Scenario: `cruise_step.csv` + KPI `cruise_ss_speed_err_mps` |
| R4  | FOLLOW spacing policy d_des = d0 + T*v | Logged signals: `d_des_m`, `distance_error_m` |
| R5  | FOLLOW time-gap error <= 0.2 s | Scenario: `follow_constant_lead.csv` + KPI `follow_ss_tgap_err_s` |
| R6  | Comfort jerk <= jerk_max outside emergency | KPI: `max_jerk_comfort_mps3` |
| R7  | lead_valid false->true -> FOLLOW quickly | Unit test: `FsmTransitions.LeadAcquiredGoesToFollow` |
| R8  | lead_valid true->false -> CRUISE quickly | Unit test: `FsmTransitions.LeadLostGoesToCruise` |
| R9  | TTC definition correctness | Unit test: `TtcComputation.*` (Function output `ttc_s`) |
| R10 | AEB trigger when TTC < threshold | Unit test: `Fsm.AebLatchesAndReleasesWithHysteresis` |
| R11 | AEB overrides comfort | Design: AEB sets `a_cmd = a_min` directly (bypasses comfort jerk) |
| R12 | driver brake cancels | Unit test: `FsmTransitions.DriverBrakeForcesOff` |
| R13 | implausible -> FAULT | Unit test: `Fsm.FaultWhenImplausible` |
| R14 | no chatter (hysteresis) | Unit test: `Fsm.AebLatchesAndReleasesWithHysteresis` |
| R15 | all verified in CI | GitHub Actions workflow `ci.yml` |