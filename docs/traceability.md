# Requirements Traceability

| Req | Description (short) | Verification |
|-----|----------------------|--------------|
| R1  | ACC disabled -> OFF + a_cmd=0 | Unit test (FSM OffWhenDisabled) |
| R2  | a_cmd limited to [a_min,a_max] | KPI: a_cmd_range within limits (all scenarios) |
| R3  | CRUISE steady-state error <= 0.5 m/s | Scenario: cruise_step.csv + KPI cruise_ss_speed_err_mps |
| R4  | FOLLOW spacing policy d_des = d0 + T*v | Logged d_des_m + distance_error_m |
| R5  | FOLLOW time-gap error <= 0.2 s | Scenario: follow_constant_lead.csv + KPI follow_ss_tgap_err_s |
| R6  | Comfort jerk <= jerk_max outside emergency | KPI max_jerk_comfort_mps3 |
| R7  | lead_valid false->true -> FOLLOW quickly | Unit test to add (TODO) |
| R8  | lead_valid true->false -> CRUISE quickly | Unit test to add (TODO) |
| R9  | TTC definition | Unit test to add (TODO) |
| R10 | AEB trigger when TTC < threshold | Unit test (FSM Aeb latch) + lead_brake scenario |
| R11 | AEB overrides comfort | Observed: AEB bypasses jerk limiter (design) |
| R12 | driver brake cancels | Unit test to add (TODO) |
| R13 | implausible -> FAULT | Unit test (FaultWhenImplausible) |
| R14 | no chatter (hysteresis) | Unit test (Aeb latch + release) |
| R15 | all verified in CI | GitHub Actions CI green |