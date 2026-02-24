#include <gtest/gtest.h>
#include <limits>
#include "acc/function.hpp"
#include "acc/fsm.hpp"

// R7/R8/R12: FSM transition and override behavior
TEST(FsmTransitions, LeadAcquiredGoesToFollow) {
  acc::Config cfg{};
  acc::Fsm fsm{};
  acc::Input in{};
  in.acc_enable = true;

  in.lead_valid = false;
  EXPECT_EQ(fsm.update(cfg, in, std::numeric_limits<double>::infinity(), true), acc::Mode::CRUISE);

  in.lead_valid = true;
  EXPECT_EQ(fsm.update(cfg, in, std::numeric_limits<double>::infinity(), true), acc::Mode::FOLLOW);
}

TEST(FsmTransitions, LeadLostGoesToCruise) {
  acc::Config cfg{};
  acc::Fsm fsm{};
  acc::Input in{};
  in.acc_enable = true;

  in.lead_valid = true;
  EXPECT_EQ(fsm.update(cfg, in, std::numeric_limits<double>::infinity(), true), acc::Mode::FOLLOW);

  in.lead_valid = false;
  EXPECT_EQ(fsm.update(cfg, in, std::numeric_limits<double>::infinity(), true), acc::Mode::CRUISE);
}

TEST(FsmTransitions, DriverBrakeForcesOff) {
  acc::Config cfg{};
  acc::Fsm fsm{};
  acc::Input in{};
  in.acc_enable = true;
  in.driver_brake = true;
  EXPECT_EQ(fsm.update(cfg, in, 1.0, true), acc::Mode::OFF);
}

// R9: TTC computation correctness (via Function output ttc_s)
TEST(TtcComputation, ClosingInComputesFiniteTtc) {
  acc::Config cfg{};
  acc::Function fn(cfg);

  acc::Input in{};
  in.acc_enable = true;
  in.ego_speed_mps = 10.0;
  in.lead_valid = true;
  in.lead_distance_m = 20.0;
  in.lead_rel_speed_mps = -5.0; // closing

  const auto out = fn.step(in);
  EXPECT_NEAR(out.ttc_s, 4.0, 1e-6);
}

TEST(TtcComputation, NotClosingGivesInf) {
  acc::Config cfg{};
  acc::Function fn(cfg);

  acc::Input in{};
  in.acc_enable = true;
  in.ego_speed_mps = 10.0;
  in.lead_valid = true;
  in.lead_distance_m = 20.0;
  in.lead_rel_speed_mps = +1.0; // pulling away

  const auto out = fn.step(in);
  EXPECT_TRUE(std::isinf(out.ttc_s));
}

TEST(TtcComputation, NoLeadGivesInf) {
  acc::Config cfg{};
  acc::Function fn(cfg);

  acc::Input in{};
  in.acc_enable = true;
  in.lead_valid = false;

  const auto out = fn.step(in);
  EXPECT_TRUE(std::isinf(out.ttc_s));
}