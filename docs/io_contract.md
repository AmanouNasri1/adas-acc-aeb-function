# I/O Contract (ACC/AEB)

## Inputs
- ego_speed_mps [m/s]
- lead_valid [bool]
- lead_distance_m [m] (bumper-to-bumper, >= 0)
- lead_rel_speed_mps [m/s] = v_lead - v_ego (closing -> negative)
- acc_enable [bool]
- aeb_enable [bool]
- driver_brake / driver_throttle [bool]

## Outputs
- a_cmd_mps2 [m/s^2] (negative = braking request)
- mode: OFF / CRUISE / FOLLOW / AEB / FAULT
- debug: ttc_s, d_des_m, distance_error_m, component accelerations

## Assumptions
- fixed timestep Ts (Config.Ts_s)
- function is deterministic: same inputs -> same outputs