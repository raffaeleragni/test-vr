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
#ifndef CORE_SRC_TYPES_HPP_
#define CORE_SRC_TYPES_HPP_

namespace t {

class double2 {
 public:
  explicit double2(double x = 0, double y = 0) : x{x}, y{y} {}
  ~double2() {}

  double x, y;
};

class double3 : public double2 {
 public:
  explicit double3(double x = 0, double y = 0, double z = 0) : double2(x, y), z{z} {}
  virtual ~double3() {}

  double z;
};

class double4 : public double3 {
 public:
  explicit double4(double x = 0, double y = 0, double z = 0, double w = 0) : double3(x, y, z), w{w} {}
  virtual ~double4() {}

  double w;
};

class position : public double3 {
 public:
  explicit position(double x = 0, double y = 0, double z = 0) : double3(x, y, z) {}
  ~position() {}

  bool operator==(const double3& other) const {
    if (this->x != other.x)
      return false;
    if (this->y != other.y)
      return false;
    return this->z == other.z;
  }

  position& operator=(double3 other) {
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    return *this;
  }
};

class rotation : public double4 {
 public:
  explicit rotation(double x = 0, double y = 0, double z = 0, double w = 0) : double4(x, y, z, w) {}
  ~rotation() {}

  bool operator==(const double4& other) const {
    if (this->x != other.x)
      return false;
    if (this->y != other.y)
      return false;
    if (this->z != other.z)
      return false;
    return this->w == other.w;
  }

  rotation& operator=(double3 other) {
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->w = 0;
    return *this;
  }

  rotation& operator=(double4 other) {
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->w = other.w;
    return *this;
  }
};

class scale : public double3 {
 public:
  explicit scale(double x = 1, double y = 1, double z = 1) : double3(x, y, z) {}
  ~scale() {}

  bool operator==(const double3& other) const {
    if (this->x != other.x)
      return false;
    if (this->y != other.y)
      return false;
    return this->z == other.z;
  }

  scale& operator=(double3 other) {
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    return *this;
  }
};

}  // namespace t

#endif  // CORE_SRC_TYPES_HPP_
