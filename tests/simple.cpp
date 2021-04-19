#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "mopt/moptional.hpp"

int main() {
  using namespace mopt;

  // TODO decide on testing framework

  Moptional<int> int_opt(4);

  Moptional<std::vector<int>> set_opt(std::vector<int>{1, 2, 3, 4, 1, 2, 3});

  std::cout << *set_opt.begin() << "\n";
  std::cout << *(--set_opt.end()) << "\n";

  std::cout << details::IsContainerLike<int>() << "\n";
  Moptional<int> single_opt(4);

  std::cout << *single_opt.begin() << "\n";

  return 0;
}
