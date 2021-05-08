#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "mopt/moptional.hpp"

TEST_CASE("STL algorithms") {
  using namespace mopt;

  Moptional<int> int_mopt(3);

  uint8_t count = 0;
  std::for_each(int_mopt.begin(), int_mopt.end(), [&count](int _val) {
    ++count;
    CHECK(_val == 3);
  });

  CHECK(count == 1);

  Moptional<int> default_int_mopt;

  std::for_each(default_int_mopt.begin(), default_int_mopt.end(),
                [](int _val) mutable { REQUIRE(0); });

  Moptional<std::vector<int>> vec_mopt(std::vector<int>{0, 1, 2, 3, 4, 5});

  auto total = std::accumulate(vec_mopt.begin(), vec_mopt.end(), 0);

  CHECK(15 == total);

  Moptional<std::set<std::string>> str_mopt;

  std::for_each(str_mopt.begin(), str_mopt.end(), [](auto&) { REQUIRE(0); });

  str_mopt.emplace(std::set<std::string>{});

  std::for_each(str_mopt.begin(), str_mopt.end(), [](auto&) { REQUIRE(0); });
}
