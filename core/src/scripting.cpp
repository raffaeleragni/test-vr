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
#include "scripting.hpp"
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

using std::string;
using std::vector;
using std::ofstream;

namespace core {

const auto LUA_whitelistedFunctions = vector<string>{
  "assert",
  "error",
  "ipairs",
  "next",
  "pairs",
  "pcall",
  "print",
  "select",
  "tonumber",
  "tostring",
  "type",
  "unpack",
  "_VERSION",
  "xpcall"
};

const auto LUA_whitelistedLibraries = vector<string>{
  "coroutine",
  "table",
  "string",
  "math",
  "utf8",
};

class ScriptEnvironment {
 public:
  sol::state lua;
  sol::environment env;
  ScriptEnvironment() {
    env = sol::environment(lua, sol::create);
    loadLibraries();
    sandbox(LUA_whitelistedFunctions, LUA_whitelistedLibraries);
    registerCustomTypes();
  }
  ~ScriptEnvironment() {}

 private:
  void loadLibraries() {
    lua.open_libraries(sol::lib::base);
    lua.open_libraries(sol::lib::coroutine);
    lua.open_libraries(sol::lib::table);
    lua.open_libraries(sol::lib::string);
    lua.open_libraries(sol::lib::utf8);
    lua.open_libraries(sol::lib::math);
  }
  void sandbox(const vector<string>& libraries, const vector<string>& functions) {
    env["_G"] = env;

    for (const auto &name : functions)
      env[name] = lua[name];

    for (const auto &name : libraries) {
      sol::table copy(lua, sol::create);
      sol::table original = lua[name].tbl;
      for (auto const& [key, val] : original)
        copy[key] = val;
      env[name] = copy;
    }

    env["loadstring"] = NULL;
    env["loadfile"] = NULL;
    env["dofile"] = NULL;
  }
  void registerCustomTypes() {
    sol::usertype<IWorld> c_iworld = lua.new_usertype<IWorld>("c_iworld");
    c_iworld["getId"] = &IWorld::getId;
    sol::usertype<IObject> c_iobject = lua.new_usertype<IObject>("c_iobject");
    c_iobject["getId"] = &IObject::getId;
    sol::usertype<IPlugin> c_iplugin = lua.new_usertype<IPlugin>("c_iplugin");
    c_iplugin["getId"] = &IPlugin::getId;
  }
};

class ScriptPlugin : public IPlugin {
 public:
  ScriptPlugin(const string& id, ScriptEnvironment* env, const string& data)
    : id{id}, source{data} {

    script = env->lua.load(data);
    // env->env.set_on(script);
  }
  ~ScriptPlugin() {}
  const string& getId() { return id; }
  Type getType() { return SCRIPT; }
  const string& getSource() { return source; }
  bool execute(IWorld* world, IObject* object) {
    try {
      script(world, object, reinterpret_cast<IPlugin*>(this));
      return true;
    } catch(const std::exception& ex) {
      printf("%s\n", ex.what());
      return false;
    } catch(...) {
      return false;
    }
  }
  void saveToFile(const string& path) {
    ofstream scriptOut(path);
    scriptOut << getSource();
    scriptOut.close();
  }

 private:
  string id;
  string source;
  sol::function script;
};

static ScriptEnvironment scriptEnvironment;

shared_ptr<IPlugin> Scripts::asPlugin(const string& id, const string& data) {
  return make_shared<ScriptPlugin>(id, &scriptEnvironment, data);
}

}  // namespace core
