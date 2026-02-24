#pragma once
#include <cstdint>
#include <limits>

namespace acc {

enum class Mode : std::uint8_t {
  OFF = 0,
  CRUISE = 1,
  FOLLOW = 2,
  AEB = 3,
  FAULT = 4
};

struct Input {
  // time
  double t_s{0.0};

  // enable / driver override
  bool acc_enable{false};
  bool aeb_enable{true};
  bool driver_brake{false};
  bool driver_throttle{false};

  // ego signals
  double ego_speed_mps{0.0};

  // lead object
  bool lead_valid{false};
  double lead_distance_m{std::numeric_limits<double>::infinity()};
  double lead_rel_speed_mps{0.0}; // v_lead - v_ego (closing -> negative)
};

struct Output {
  Mode mode{Mode::OFF};
  double a_cmd_mps2{0.0};

  // debug
  double d_des_m{0.0};
  double ttc_s{std::numeric_limits<double>::infinity()};
  double distance_error_m{0.0};

  double a_cruise_mps2{0.0};
  double a_follow_mps2{0.0};
  double a_aeb_mps2{0.0};
};

}  // namespace acc