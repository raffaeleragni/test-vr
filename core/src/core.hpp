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
#ifndef CORE_SRC_CORE_HPP_
#define CORE_SRC_CORE_HPP_

#include <memory>
#include <string>
#include <vector>

#include "types.hpp"

using std::string;
using std::vector;
using std::shared_ptr;

namespace core {

class IWorld;
class IObject;
class IPlugin;

class Worlds {
 public:
  static shared_ptr<IWorld> createNew(const string& id);
  static shared_ptr<IWorld> load(const string& id, const string& path);
};

class IPlugin {
 public:
  enum Type {SCRIPT};
  virtual ~IPlugin() {}
  virtual const string& getId() = 0;
  virtual Type getType() = 0;
  virtual void saveToFile(const string& path) = 0;
  virtual bool execute(IWorld* world, IObject* object) = 0;
};

class IObject {
 public:
  virtual ~IObject() {}
  virtual const string& getId() = 0;
  virtual void setPosition(const t::position& pos) = 0;
  virtual const t::position& getPosition() = 0;
  virtual void setRotation(const t::rotation& pos) = 0;
  virtual const t::rotation& getRotation() = 0;
  virtual void setScale(const t::scale& pos) = 0;
  virtual const t::scale& getScale() = 0;
  virtual vector<string> listPluginIds() = 0;
};

class IWorld {
 public:
  virtual ~IWorld() {}
  virtual const string& getId() = 0;
  virtual size_t objectCount() = 0;
  virtual shared_ptr<IObject> newObject(const string& id) = 0;
  virtual shared_ptr<IObject> getObject(const string& id) = 0;
  virtual vector<string> listObjectIds() = 0;
  virtual void deleteObject(const string& key) = 0;
  virtual void saveScriptToObject(const string& objectId, const string& pluginId, const string& code) = 0;
  virtual void round() = 0;
  virtual void save(const string& path) = 0;
};

}  // namespace core

#endif  // CORE_SRC_CORE_HPP_
