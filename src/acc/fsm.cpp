#include "acc/fsm.hpp"
#include <cmath>
#include <limits>

namespace acc {

Mode Fsm::update(const Config& cfg, const Input& in, double ttc_s, bool plausible) {
  // Highest priority: disabled or driver brake
  if (!in.acc_enable || in.driver_brake) {
    state_ = FsmState{};
    state_.mode = Mode::OFF;
    return state_.mode;
  }

  // Fault
  if (!plausible) {
    state_ = FsmState{};
    state_.mode = Mode::FAULT;
    return state_.mode;
  }

  // AEB latch + hysteresis
  const bool closing = in.lead_valid && std::isfinite(in.lead_distance_m) &&
                       std::isfinite(in.lead_rel_speed_mps) && (in.lead_rel_speed_mps < 0.0);

  const bool aeb_trigger =
      in.aeb_enable && closing && std::isfinite(ttc_s) && (ttc_s < cfg.ttc_aeb_s);

  const double release_ttc = cfg.ttc_aeb_s + 0.2;  // hysteresis
  const bool aeb_release = !closing || !std::isfinite(ttc_s) || (ttc_s > release_ttc);

  if (state_.aeb_latched) {
    if (aeb_release) state_.aeb_latched = false;
  } else {
    if (aeb_trigger) state_.aeb_latched = true;
  }

  if (state_.aeb_latched) {
    state_.mode = Mode::AEB;
    return state_.mode;
  }

  state_.mode = in.lead_valid ? Mode::FOLLOW : Mode::CRUISE;
  return state_.mode;
}

}  // namespace acc