#pragma once
#include "acc/config.hpp"
#include "acc/types.hpp"

namespace acc {

class Function {
 public:
  explicit Function(Config cfg) : cfg_(cfg) {}

  void reset() { prev_out_ = Output{}; }

  // Step the function one cycle (Ts fixed in cfg)
  Output step(const Input& in);

 private:
  Config cfg_;
  Output prev_out_{};
};

}  // namespace acc