#include "sim/scenario.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace sim {

static std::string trim(const std::string& s) {
  const auto b = s.find_first_not_of(" \t\r\n");
  if (b == std::string::npos) return "";
  const auto e = s.find_last_not_of(" \t\r\n");
  return s.substr(b, e - b + 1);
}

static std::vector<std::string> split_csv(const std::string& line) {
  std::vector<std::string> out;
  std::stringstream ss(line);
  std::string cell;
  while (std::getline(ss, cell, ',')) out.push_back(trim(cell));
  return out;
}

double Scenario::duration_s() const {
  if (rows.empty()) return 0.0;
  return rows.back().t_s;
}

Row Scenario::sample(double t) const {
  if (rows.empty()) return Row{};
  if (t <= rows.front().t_s) return rows.front();
  if (t >= rows.back().t_s) return rows.back();

  // find interval [i, i+1]
  auto it = std::upper_bound(rows.begin(), rows.end(), t,
                             [](double val, const Row& r) { return val < r.t_s; });
  size_t i1 = static_cast<size_t>(std::distance(rows.begin(), it));
  size_t i0 = i1 - 1;

  const Row& a = rows[i0];
  const Row& b = rows[i1];

  const double dt = b.t_s - a.t_s;
  const double alpha = (dt > 0.0) ? (t - a.t_s) / dt : 0.0;

  Row r{};
  r.t_s = t;
  r.lead_valid = a.lead_valid;  // stepwise
  r.v_lead_mps = a.v_lead_mps + alpha * (b.v_lead_mps - a.v_lead_mps);
  r.v_set_mps  = a.v_set_mps  + alpha * (b.v_set_mps  - a.v_set_mps);

  // distance override: if either endpoint has it, prefer latest <= t
  if (a.has_distance_override) {
    r.has_distance_override = true;
    r.lead_distance_m_override = a.lead_distance_m_override;
  }
  return r;
}

Scenario load_csv(const std::string& path) {
  std::ifstream f(path);
  if (!f) throw std::runtime_error("Cannot open scenario: " + path);

  Scenario sc{};
  std::string line;

  // read metadata + header
  std::vector<std::string> header;
  while (std::getline(f, line)) {
    line = trim(line);
    if (line.empty()) continue;

    if (line.rfind("#", 0) == 0) {
      // # key=value
      const auto kv = trim(line.substr(1));
      const auto eq = kv.find('=');
      if (eq != std::string::npos) {
        const auto key = trim(kv.substr(0, eq));
        const auto val = trim(kv.substr(eq + 1));
        const double d = std::stod(val);
        if (key == "Ts_s") sc.meta.Ts_s = d;
        else if (key == "init_ego_speed_mps") sc.meta.init_ego_speed_mps = d;
        else if (key == "init_lead_distance_m") sc.meta.init_lead_distance_m = d;
      }
      continue;
    }

    // first non-comment non-empty is header
    header = split_csv(line);
    break;
  }

  if (header.empty()) throw std::runtime_error("Scenario missing header row: " + path);

  auto col_index = [&](const std::string& name) -> int {
    for (size_t i = 0; i < header.size(); ++i) if (header[i] == name) return (int)i;
    return -1;
  };

  const int c_t = col_index("t_s");
  const int c_valid = col_index("lead_valid");
  const int c_vlead = col_index("v_lead_mps");
  const int c_vset = col_index("v_set_mps");
  const int c_d = col_index("lead_distance_m");

  if (c_t < 0 || c_valid < 0 || c_vlead < 0) {
    throw std::runtime_error("Scenario must include columns: t_s, lead_valid, v_lead_mps");
  }

  while (std::getline(f, line)) {
    line = trim(line);
    if (line.empty() || line.rfind("#", 0) == 0) continue;

    const auto cells = split_csv(line);
    Row r{};
    r.t_s = std::stod(cells.at(c_t));
    r.lead_valid = (std::stoi(cells.at(c_valid)) != 0);
    r.v_lead_mps = std::stod(cells.at(c_vlead));
    r.v_set_mps = (c_vset >= 0 && c_vset < (int)cells.size()) ? std::stod(cells.at(c_vset)) : 25.0;

    if (c_d >= 0 && c_d < (int)cells.size()) {
      const std::string& s = cells.at(c_d);
      if (!s.empty()) {
        r.has_distance_override = true;
        r.lead_distance_m_override = std::stod(s);
      }
    }

    sc.rows.push_back(r);
  }

  if (sc.rows.size() < 2) throw std::runtime_error("Scenario needs at least 2 rows: " + path);
  std::sort(sc.rows.begin(), sc.rows.end(), [](const Row& a, const Row& b){ return a.t_s < b.t_s; });

  return sc;
}

}  // namespace sim