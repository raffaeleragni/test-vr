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
#include <iostream>

#include "test.hpp"
#include "../src/core.hpp"
#include "../src/scripting.hpp"

using std::string;
using core::IWorld;
using core::IObject;
using core::Scripts;

class MockWorld: public IWorld {
 public:
    explicit MockWorld(const string& id) : id{id} {}

    const string& getId()  { return id; }
    size_t objectCount() override { return 0; };
    shared_ptr<IObject> newObject(const string& id) { return NULL; }
    shared_ptr<IObject> getObject(const string& id) { return NULL; }
    vector<string> listObjectIds() { return vector<string>(); }
    void deleteObject(const string& key) {}
    void saveScriptToObject(const string& objectId, const string& pluginId, const string& code) {}
    void round() {}
    void save(const string& path) {}

 private:
    string id;
};

class MockObject: public IObject {
 public:
    explicit MockObject(const string& id) : id{id} {}
    const string& getId() override { return id; };
    void setPosition(const t::position& pos) {}
    const t::position& getPosition() { return position; }
    void setRotation(const t::rotation& pos) {}
    const t::rotation& getRotation() { return rotation; }
    void setScale(const t::scale& pos) {}
    const t::scale& getScale() { return scale; }
    vector<string> listPluginIds() { return vector<string>(); }

 private:
    string id;
    t::position position = t::position{0, 0, 0};
    t::rotation rotation = t::rotation{0, 0, 0, 0};
    t::scale scale = t::scale{1, 1, 1};
};

bool runScript(const char* script) {
  MockWorld world("world");
  MockObject object("object");
  try {
    auto scriptPlugin = Scripts::asPlugin("plugin", script);
    return scriptPlugin->execute(&world, &object);
  } catch (...) {
    return false;
  }
}

TEST_CASE("Run plugin with parameters") {
  bool success = runScript(R"script(
local world, object, plugin = ...
print('Hello ' .. world:getId() .. '-' .. object:getId() .. '-' .. plugin:getId())
  )script");

  REQUIRE(success);
}

TEST_CASE("Run with script with compile errors") {
  bool success = runScript("a == 2");

  REQUIRE(success == false);
}

TEST_CASE("Check included library: base") {
  bool success = runScript("if error ~=nil then print('baselib OK') end");
  REQUIRE(success);
}

TEST_CASE("Check included library: coroutine") {
  bool success = runScript("if coroutine ~= nil then print('coroutine OK') end");
  REQUIRE(success);
}

TEST_CASE("Check included library: table") {
  bool success = runScript("if table ~= nil then print('table OK') end");
  REQUIRE(success);
}

TEST_CASE("Check included library: math") {
  bool success = runScript("if math ~= nil then print('math OK') end");
  REQUIRE(success);
}

TEST_CASE("Check included library: utf8") {
  bool success = runScript("if utf8 ~= nil then print('utf8 OK') end");
  REQUIRE(success);
}
