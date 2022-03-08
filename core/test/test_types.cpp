/*
    Core engine
    Copyright (C) 2022  Raffaele Ragni

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "test.hpp"
#include "../src/types.hpp"

TEST_CASE("Position equality", "") {
  auto pos = t::position{0, 0, 0};
  REQUIRE(pos == t::position{0, 0, 0});

  pos.x = 1;
  REQUIRE(pos == t::position{1, 0, 0});

  pos.y = 2;
  REQUIRE(pos == t::position{1, 2, 0});

  pos.z = 3;
  REQUIRE(pos == t::position{1, 2, 3});
}

TEST_CASE("Position assignment", "") {
  auto pos = t::position{0, 0, 0};
  pos = t::position{1, 2, 3};
  REQUIRE(pos == t::position{1, 2, 3});
}

TEST_CASE("Rotation equality", "") {
  auto rot = t::rotation{0, 0, 0, 0};
  REQUIRE(rot == t::rotation{0, 0, 0, 0});

  rot.x = 1;
  REQUIRE(rot == t::rotation{1, 0, 0, 0});

  rot.y = 2;
  REQUIRE(rot == t::rotation{1, 2, 0, 0});

  rot.z = 3;
  REQUIRE(rot == t::rotation{1, 2, 3, 0});

  rot.w = 4;
  REQUIRE(rot == t::rotation{1, 2, 3, 4});

  rot = t::position{4, 5, 6};
  REQUIRE(rot == t::rotation{4, 5, 6, 0});
}

TEST_CASE("Rotation assignment", "") {
  auto rot = t::rotation{0, 0, 0};
  rot = t::rotation{1, 2, 3, 4};
  REQUIRE(rot == t::rotation{1, 2, 3, 4});
}

TEST_CASE("Scale equality", "") {
  auto s = t::scale{0, 0, 0};
  REQUIRE(s == t::scale{0, 0, 0});

  s.x = 1;
  REQUIRE(s == t::scale{1, 0, 0});

  s.y = 2;
  REQUIRE(s == t::scale{1, 2, 0});

  s.z = 3;
  REQUIRE(s == t::scale{1, 2, 3});
}

TEST_CASE("Scale assignment", "") {
  auto s = t::scale{0, 0, 0};
  s = t::scale{1, 2, 3};
  REQUIRE(s == t::scale{1, 2, 3});
}
