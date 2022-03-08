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

#ifndef CORE_SRC_SCRIPTING_HPP_
#define CORE_SRC_SCRIPTING_HPP_

#include <memory>
#include <string>

#include "core.hpp"

using std::string;
using std::shared_ptr;

namespace core {
class Scripts {
 public:
  static shared_ptr<IPlugin> asPlugin(const string& id, const string& data);
};
}  // namespace core

#endif  // CORE_SRC_SCRIPTING_HPP_
