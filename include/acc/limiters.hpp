#pragma once
#include <algorithm>

namespace acc {

inline double clamp(double x, double lo, double hi) {
  return std::max(lo, std::min(x, hi));
}

inline double jerk_limit(double a_prev, double a_raw, double Ts, double jerk_max) {
  const double max_da = jerk_max * Ts;
  return clamp(a_raw, a_prev - max_da, a_prev + max_da);
}

}  // namespace acc