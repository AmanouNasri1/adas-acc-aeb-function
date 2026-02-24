#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

#include "acc/function.hpp"
#include "acc/types.hpp"
#include "sim/scenario.hpp"

static std::string get_arg(int argc, char** argv, const std::string& key, const std::string& def) {
  for (int i = 1; i + 1 < argc; ++i) {
    if (std::string(argv[i]) == key) return std::string(argv[i + 1]);
  }
  return def;
}

static bool has_flag(int argc, char** argv, const std::string& key) {
  for (int i = 1; i < argc; ++i) if (std::string(argv[i]) == key) return true;
  return false;
}

static int mode_to_int(acc::Mode m) { return static_cast<int>(m); }

int main(int argc, char** argv) {
  const std::string scenario_path = get_arg(argc, argv, "--scenario", "scenarios/lead_brake.csv");
  const std::string out_path      = get_arg(argc, argv, "--out", "results/out.csv");
  const bool aeb_off              = has_flag(argc, argv, "--no-aeb");

  sim::Scenario sc;
  try {
    sc = sim::load_csv(scenario_path);
  } catch (const std::exception& e) {
    std::cerr << "Error loading scenario: " << e.what() << "\n";
    return 1;
  }

  acc::Config cfg;
  cfg.Ts_s = sc.meta.Ts_s;

  acc::Function fn(cfg);

  // Closed-loop "plant"
  double v_ego = sc.meta.init_ego_speed_mps;
  double d = sc.meta.init_lead_distance_m;

  std::ofstream out(out_path);
  if (!out) {
    std::cerr << "Cannot open output file: " << out_path << "\n";
    return 1;
  }

  out << "t_s,mode,ego_speed_mps,lead_valid,lead_distance_m,lead_rel_speed_mps,a_cmd_mps2,ttc_s\n";

  const double t_end = sc.duration_s();
  for (double t = 0.0; t <= t_end + 1e-9; t += cfg.Ts_s) {
    const auto row = sc.sample(t);

    // Lead state from scenario
    const bool lead_valid = row.lead_valid;
    const double v_lead = row.v_lead_mps;

    if (row.has_distance_override) d = row.lead_distance_m_override;
    if (!lead_valid) d = std::numeric_limits<double>::infinity();

    // Compute relative speed (v_lead - v_ego)
    const double v_rel = lead_valid ? (v_lead - v_ego) : 0.0;

    // Build function input
    acc::Input in;
    in.t_s = t;
    in.acc_enable = true;
    in.aeb_enable = !aeb_off;
    in.driver_brake = false;
    in.driver_throttle = false;
    in.ego_speed_mps = v_ego;
    in.lead_valid = lead_valid;
    in.lead_distance_m = d;
    in.lead_rel_speed_mps = v_rel;

    // Run function
    const auto y = fn.step(in);

    // Plant update (very simple)
    // v_ego[k+1] = max(0, v_ego + a_cmd*Ts)
    v_ego = std::max(0.0, v_ego + y.a_cmd_mps2 * cfg.Ts_s);

    // d[k+1] = d + (v_lead - v_ego)*Ts  (only if lead exists)
    if (lead_valid && std::isfinite(d)) {
      d = std::max(0.0, d + (v_lead - v_ego) * cfg.Ts_s);
    }

    out << t << "," << mode_to_int(y.mode) << "," << v_ego << ","
        << (lead_valid ? 1 : 0) << "," << d << "," << v_rel << ","
        << y.a_cmd_mps2 << "," << y.ttc_s << "\n";
  }

  std::cout << "Wrote: " << out_path << "\n";
  return 0;
}