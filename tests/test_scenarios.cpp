#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

static double find_value_in_kpi(const std::string& kpi_file, const std::string& key) {
  std::ifstream f(kpi_file);
  if (!f) throw std::runtime_error("Cannot open KPI file: " + kpi_file);
  std::string line;
  while (std::getline(f, line)) {
    if (line.rfind(key, 0) == 0) {
      // key: value
      auto pos = line.find(':');
      if (pos == std::string::npos) continue;
      return std::stod(line.substr(pos + 1));
    }
  }
  throw std::runtime_error("Key not found in KPI file: " + key);
}

static int run(const std::string& cmd) {
  const int rc = std::system(cmd.c_str());
  return rc;
}

TEST(Scenarios, CruiseStep_MeetsTrackingAndComfort) {
  ASSERT_EQ(run("./sim_runner --scenario ../scenarios/cruise_step.csv --out ../results/ci_cruise.csv"), 0);
  ASSERT_EQ(run("python3 ../tools/evaluate_kpis.py ../results/ci_cruise.csv 0.02 3.0 1.5 3.0 > ../results/ci_cruise_kpi.txt"), 0);

  const double cruise_err = find_value_in_kpi("../results/ci_cruise_kpi.txt", "cruise_ss_speed_err_mps");
  const double jerk_comf  = find_value_in_kpi("../results/ci_cruise_kpi.txt", "max_jerk_comfort_mps3");
  const double aeb_time   = find_value_in_kpi("../results/ci_cruise_kpi.txt", "aeb_time_s");

  EXPECT_LE(cruise_err, 0.5);
  EXPECT_LE(jerk_comf,  2.0);
  EXPECT_LE(aeb_time,   0.0);
}

TEST(Scenarios, FollowConstantLead_MeetsTimeGapAndComfort) {
  ASSERT_EQ(run("./sim_runner --scenario ../scenarios/follow_constant_lead.csv --out ../results/ci_follow.csv"), 0);
  ASSERT_EQ(run("python3 ../tools/evaluate_kpis.py ../results/ci_follow.csv 0.02 3.0 1.5 3.0 > ../results/ci_follow_kpi.txt"), 0);

  const double tgap_err  = find_value_in_kpi("../results/ci_follow_kpi.txt", "follow_ss_tgap_err_s");
  const double jerk_comf = find_value_in_kpi("../results/ci_follow_kpi.txt", "max_jerk_comfort_mps3");
  const double aeb_time  = find_value_in_kpi("../results/ci_follow_kpi.txt", "aeb_time_s");

  EXPECT_LE(tgap_err, 0.2);
  EXPECT_LE(jerk_comf, 2.0);
  EXPECT_LE(aeb_time, 0.0);
}