#pragma once
#include <cstdint>

namespace acc {

struct Config {
  // sample time
  double Ts_s{0.02};

  // speed setpoint (for early phases; later scenario can provide it)
  double v_set_mps{25.0}; // 90 km/h

  // spacing policy
  double time_gap_s{1.5};
  double standstill_offset_m{3.0};

  // accel limits
  double a_max_mps2{2.0};
  double a_min_mps2{-6.0};

  // comfort (ACC/FOLLOW only)
  double jerk_max_mps3{2.0};

  // AEB trigger thresholds
  double ttc_warn_s{2.5};
  double ttc_aeb_s{1.5};

  // plausibility limits (simple first pass)
  double max_distance_m{300.0};
  double max_abs_rel_speed_mps{80.0};

  // --- Cruise controller (PI) ---
  double cruise_kp{0.5};
  double cruise_ki{0.10};
  double cruise_i_min{-3.0};
  double cruise_i_max{3.0};

  // --- Follow controller (PD on distance + relative speed) ---
  double follow_kp_dist{0.2};   // [m/s^2] per meter error
  double follow_kd_rel{0.6};    // [m/s^2] per (m/s)
};

}  // namespace acc