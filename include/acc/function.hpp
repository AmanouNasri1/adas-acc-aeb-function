#pragma once
#include "acc/config.hpp"
#include "acc/fsm.hpp"
#include "acc/types.hpp"

namespace acc {

class Function {
 public:
  explicit Function(Config cfg) : cfg_(cfg) {}

  void reset() {
    fsm_.reset();
    prev_out_ = Output{};
    cruise_i_ = 0.0;
  }

  Output step(const Input& in);

 private:
  Config cfg_;
  Fsm fsm_;
  Output prev_out_{};
  double cruise_i_{0.0};
};

}  // namespace acc