#pragma once
#include "acc/config.hpp"
#include "acc/types.hpp"

namespace acc {

struct FsmState {
  Mode mode{Mode::OFF};
  bool aeb_latched{false};
};

class Fsm {
 public:
  void reset() { state_ = FsmState{}; }

  Mode update(const Config& cfg, const Input& in, double ttc_s, bool plausible);

  const FsmState& state() const { return state_; }

 private:
  FsmState state_{};
};

}  // namespace acc