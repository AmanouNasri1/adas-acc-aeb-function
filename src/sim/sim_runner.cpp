#include "acc/dummy.hpp"
#include <iostream>

int main() {
  std::cout << "sim_runner boot OK. add(2,3)=" << acc::add(2, 3) << "\n";
  return 0;
}