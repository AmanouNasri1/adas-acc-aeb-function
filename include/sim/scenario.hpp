#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace sim {

struct Meta {
  double Ts_s{0.02};
  double init_ego_speed_mps{20.0};
  double init_lead_distance_m{40.0};
};

struct Row {
  double t_s{0.0};
  bool lead_valid{true};
  double v_lead_mps{20.0};
  double v_set_mps{25.0};
  double lead_distance_m_override{0.0};
  bool has_distance_override{false};
};

struct Scenario {
  Meta meta;
  std::vector<Row> rows;

  double duration_s() const;
  Row sample(double t_s) const;  // piecewise-linear for speeds, stepwise for lead_valid
};

Scenario load_csv(const std::string& path);

}  // namespace sim