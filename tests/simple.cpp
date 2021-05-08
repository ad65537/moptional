#include <iostream>
#include <set>
#include <string>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "mopt/moptional.hpp"

TEST_CASE("Basic usage") {
  using namespace mopt;

  REQUIRE(!details::IsContainerLike<int>());
  REQUIRE(details::IsContainerLike<std::vector<int>>());

  Moptional<int> int_mopt(4);
  Moptional<std::vector<int>> set_mopt(std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7});

  CHECK(*int_mopt.begin() == 4);
  CHECK(set_mopt->size() == 8);
  CHECK(*set_mopt.begin() == 0);
  CHECK(*--set_mopt.end() == 7);

  auto start_it = int_mopt.begin();
  auto end_it = int_mopt.end();
  
  REQUIRE(start_it != end_it);

  CHECK(*start_it == 4);

  ++start_it;

  CHECK(start_it == end_it);

  Moptional<std::set<std::string>> str_set_mopt;

  SUBCASE("Check both empty cases") {
    CHECK(str_set_mopt.begin() == str_set_mopt.end());

    str_set_mopt.emplace<std::string>({});

    CHECK(str_set_mopt.begin() == str_set_mopt.end());
  }
  
  static constexpr auto kFirst = "monadic";
  static constexpr auto kSecond = "opt";

  str_set_mopt.emplace<std::string>({kFirst, kSecond});

  auto str_it = str_set_mopt.begin();
  auto str_end_it = str_set_mopt.end();

  REQUIRE(str_it != str_end_it); 
  CHECK(*str_it == kFirst); 

  str_it++;

  REQUIRE(str_it != str_end_it);
  CHECK(*str_it == kSecond);

  str_it++;
  CHECK(str_it == str_end_it);  
}
