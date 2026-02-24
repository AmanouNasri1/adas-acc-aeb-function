#include "acc/plausibility.hpp"
#include <cmath>

namespace acc {

static bool finite(double x) { return std::isfinite(x); }

bool plausible(const Config& cfg, const Input& in) {
  if (!finite(in.ego_speed_mps) || in.ego_speed_mps < 0.0) return false;

  if (in.lead_valid) {
    if (!finite(in.lead_distance_m) || in.lead_distance_m < 0.0 ||
        in.lead_distance_m > cfg.max_distance_m)
      return false;

    if (!finite(in.lead_rel_speed_mps) ||
        std::abs(in.lead_rel_speed_mps) > cfg.max_abs_rel_speed_mps)
      return false;
  }

  return true;
}

}  // namespace acc