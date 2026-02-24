#include <gtest/gtest.h>
#include <limits>
#include "acc/fsm.hpp"

TEST(Fsm, OffWhenDisabled) {
  acc::Config cfg{};
  acc::Fsm fsm{};
  acc::Input in{};
  in.acc_enable = false;
  EXPECT_EQ(fsm.update(cfg, in, 1.0, true), acc::Mode::OFF);
}

TEST(Fsm, FaultWhenImplausible) {
  acc::Config cfg{};
  acc::Fsm fsm{};
  acc::Input in{};
  in.acc_enable = true;
  EXPECT_EQ(fsm.update(cfg, in, 1.0, false), acc::Mode::FAULT);
}

TEST(Fsm, FollowWhenLeadValid) {
  acc::Config cfg{};
  acc::Fsm fsm{};
  acc::Input in{};
  in.acc_enable = true;
  in.lead_valid = true;
  EXPECT_EQ(fsm.update(cfg, in, std::numeric_limits<double>::infinity(), true), acc::Mode::FOLLOW);
}

TEST(Fsm, AebLatchesAndReleasesWithHysteresis) {
  acc::Config cfg{};
  cfg.ttc_aeb_s = 1.5;

  acc::Fsm fsm{};
  acc::Input in{};
  in.acc_enable = true;
  in.aeb_enable = true;
  in.lead_valid = true;
  in.lead_distance_m = 10.0;
  in.lead_rel_speed_mps = -10.0; // closing

  // trigger
  EXPECT_EQ(fsm.update(cfg, in, 1.0, true), acc::Mode::AEB);

  // still latched above trigger but below release (1.7)
  EXPECT_EQ(fsm.update(cfg, in, 1.6, true), acc::Mode::AEB);

  // release (> 1.7)
  EXPECT_NE(fsm.update(cfg, in, 1.8, true), acc::Mode::AEB);
}