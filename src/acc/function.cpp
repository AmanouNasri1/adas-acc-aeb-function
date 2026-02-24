#include "acc/function.hpp"
#include "acc/limiters.hpp"
#include "acc/plausibility.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace acc {

static double compute_ttc(const Input& in) {
  if (!in.lead_valid) return std::numeric_limits<double>::infinity();
  if (!std::isfinite(in.lead_distance_m) || !std::isfinite(in.lead_rel_speed_mps))
    return std::numeric_limits<double>::infinity();
  if (in.lead_distance_m <= 0.0) return 0.0;
  if (in.lead_rel_speed_mps >= 0.0) return std::numeric_limits<double>::infinity();
  return in.lead_distance_m / (-in.lead_rel_speed_mps);
}

Output Function::step(const Input& in) {
  Output out{};
  out.ttc_s = compute_ttc(in);

  const bool ok = plausible(cfg_, in);
  out.mode = fsm_.update(cfg_, in, out.ttc_s, ok);

  // OFF/FAULT
  if (out.mode == Mode::OFF || out.mode == Mode::FAULT) {
    cruise_i_ = 0.0;
    prev_out_ = out;
    return out;
  }

  // AEB overrides
  if (out.mode == Mode::AEB) {
    out.a_aeb_mps2 = cfg_.a_min_mps2;
    out.a_cmd_mps2 = clamp(cfg_.a_min_mps2, cfg_.a_min_mps2, cfg_.a_max_mps2);
    prev_out_ = out;
    return out;
  }

  // CRUISE PI with anti-windup (prevents oscillation from saturation)
  const double e_v = in.v_set_mps - in.ego_speed_mps;

  // candidate integrator update
  const double i_candidate = clamp(
      cruise_i_ + cfg_.cruise_ki * e_v * cfg_.Ts_s,
      cfg_.cruise_i_min, cfg_.cruise_i_max);

  // compute unsaturated PI output using candidate integrator
  const double a_pi_unsat = cfg_.cruise_kp * e_v + i_candidate;

  // check saturation (relative to accel limits)
  const bool sat_high = (a_pi_unsat > cfg_.a_max_mps2);
  const bool sat_low  = (a_pi_unsat < cfg_.a_min_mps2);

  // integrate only if not saturating in the same direction as the error
  if (!((sat_high && e_v > 0.0) || (sat_low && e_v < 0.0))) {
    cruise_i_ = i_candidate;
  }

  out.a_cruise_mps2 = cfg_.cruise_kp * e_v + cruise_i_;

  // FOLLOW PD
  double a_raw = out.a_cruise_mps2;
  if (out.mode == Mode::FOLLOW && in.lead_valid && std::isfinite(in.lead_distance_m)) {
    out.d_des_m = cfg_.standstill_offset_m + cfg_.time_gap_s * in.ego_speed_mps;
    out.distance_error_m = in.lead_distance_m - out.d_des_m;

    out.a_follow_mps2 =
        cfg_.follow_kp_dist * out.distance_error_m + cfg_.follow_kd_rel * in.lead_rel_speed_mps;

    a_raw = std::min(out.a_cruise_mps2, out.a_follow_mps2);
  }

  a_raw = clamp(a_raw, cfg_.a_min_mps2, cfg_.a_max_mps2);
  double jerk = cfg_.jerk_max_mps3;
  if (in.lead_valid && std::isfinite(out.ttc_s) && out.ttc_s < cfg_.ttc_warn_s) {
    jerk = cfg_.jerk_max_emergency_mps3;
  }
  out.a_cmd_mps2 = jerk_limit(prev_out_.a_cmd_mps2, a_raw, cfg_.Ts_s, jerk);
    prev_out_ = out;
    return out;
}

}  // namespace acc