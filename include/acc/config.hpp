#pragma once
#include <cstdint>

namespace acc {

struct Config {
  // sample time
  double Ts_s{0.02};

  // default speed setpoint (scenario provides v_set via Input)
  double v_set_mps{25.0}; // ~90 km/h

  // spacing policy
  double time_gap_s{1.5};
  double standstill_offset_m{3.0};

  // accel limits
  double a_max_mps2{2.0};
  double a_min_mps2{-6.0};

  // comfort jerk limit (ACC/FOLLOW normal operation)
  double jerk_max_mps3{2.0};

  // emergency jerk limit (used when TTC < ttc_warn_s to avoid AEB)
  double jerk_max_emergency_mps3{12.0};

  // AEB thresholds
  double ttc_warn_s{3.0};
  double ttc_aeb_s{1.5};

  // plausibility limits
  double max_distance_m{300.0};
  double max_abs_rel_speed_mps{80.0};

  // --- Cruise controller (PI) ---
  double cruise_kp{0.5};
  double cruise_ki{0.10};
  double cruise_i_min{-3.0};
  double cruise_i_max{3.0};

  // --- Follow controller (PD on distance + relative speed) ---
  double follow_kp_dist{0.3};   // [m/s^2] per meter distance error
  double follow_kd_rel{1.2};    // [m/s^2] per (m/s) relative speed
};

}  // namespace acc