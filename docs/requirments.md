# Requirements (ACC/AEB)

R1  When acc_enable=false -> mode=OFF and a_cmd=0.
R2  Output a_cmd shall be limited to [a_min, a_max] at all times.
R3  In CRUISE, controller shall track v_set with steady-state error <= 0.5 m/s within 10 s (nominal plant).
R4  In FOLLOW, desired distance shall be d_des = d0 + T * v_ego.
R5  In FOLLOW, time-gap error shall be <= 0.2 s in steady-state (nominal plant, constant lead).
R6  Jerk limiting: |a_cmd[k]-a_cmd[k-1]|/Ts <= jerk_max in CRUISE/FOLLOW.
R7  If lead_valid transitions false->true, mode shall enter FOLLOW within 2 cycles.
R8  If lead_valid transitions true->false, mode shall enter CRUISE within 2 cycles (if acc_enable=true).
R9  TTC shall be computed as distance/(-rel_speed) when rel_speed<0, else inf.
R10 AEB shall trigger when (lead_valid=true) AND (rel_speed<0) AND (TTC < ttc_aeb).
R11 In AEB, a_cmd shall be <= min(a_cmd_acc, a_aeb) and never exceed comfort jerk limits requirement is waived (safety first).
R12 Driver brake shall force mode=OFF (or FAULT-safe) and a_cmd=0 within 1 cycle.
R13 Implausible inputs (negative distance, NaN, distance>max_distance, |rel_speed|>max_abs_rel_speed) -> mode=FAULT and a_cmd=0.
R14 Mode shall not chatter: transitions into/out of AEB shall include hysteresis of at least 0.2 s TTC margin (to be implemented).
R15 All requirements shall be verifiable by automated unit and scenario tests in CI.