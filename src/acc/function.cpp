#include "acc/function.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace acc {

static double clamp(double x, double lo, double hi) {
  return std::max(lo, std::min(x, hi));
}

Output Function::step(const Input& in) {
  Output out{};
  out.mode = Mode::OFF;
  out.a_cmd_mps2 = 0.0;

  // R1-ish: if not enabled or driver brake -> OFF
  if (!in.acc_enable || in.driver_brake) {
    out.mode = Mode::OFF;
    return out;
  }

  // Compute TTC (R9-ish)
  out.ttc_s = std::numeric_limits<double>::infinity();
  if (in.lead_valid && std::isfinite(in.lead_distance_m) && in.lead_distance_m > 0.0 &&
      std::isfinite(in.lead_rel_speed_mps) && in.lead_rel_speed_mps < 0.0) {
    out.ttc_s = in.lead_distance_m / (-in.lead_rel_speed_mps);
  }

  // Very simple placeholder behavior (Phase 2):
  // - If AEB triggers, command hard braking
  // - Else command 0 accel (controllers come later)
  if (in.aeb_enable && in.lead_valid && out.ttc_s < cfg_.ttc_aeb_s) {
    out.mode = Mode::AEB;
    out.a_aeb_mps2 = cfg_.a_min_mps2;
    out.a_cmd_mps2 = cfg_.a_min_mps2;
  } else {
    out.mode = in.lead_valid ? Mode::FOLLOW : Mode::CRUISE;
    out.a_cmd_mps2 = 0.0;
  }

  // Clamp (R2-ish)
  out.a_cmd_mps2 = clamp(out.a_cmd_mps2, cfg_.a_min_mps2, cfg_.a_max_mps2);

  prev_out_ = out;
  return out;
}

}  // namespace acc