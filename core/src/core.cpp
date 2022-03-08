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
#include <yaml-cpp/yaml.h>
#include <map>
#include <queue>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "core.hpp"
#include "scripting.hpp"

using std::string;
using std::map;
using std::queue;
using std::vector;
using std::ofstream;
using std::ifstream;
using std::stringstream;
namespace fs = std::filesystem;

namespace core {

class UpdateCommand {
 public:
  void execute() {}
  void operator()() { execute(); }
};

class UpdateQueue {
 public:
  void enqueue(shared_ptr<UpdateCommand> command) { updateQueue.push(command); }
  bool empty() { return updateQueue.empty(); }
  size_t size() { return updateQueue.size(); }
  bool processNext() {
    if (updateQueue.empty())
      return false;
    auto entry = updateQueue.front();
    entry->execute();
    updateQueue.pop();
    return true;
  }
 private:
  queue<shared_ptr<UpdateCommand>> updateQueue;
};

class Object : public IObject {
 public:
  explicit Object(const string& id) : id{id} {}

  const string& getId() override { return id; }

  void setPosition(const t::position& pos) { this->position = pos; }
  const t::position& getPosition() { return this->position; }

  void setRotation(const t::rotation& rot) { this->rotation = rot; }
  const t::rotation& getRotation() { return this->rotation; }

  void setScale(const t::scale& s) { this->scale = s; }
  const t::scale& getScale() { return this->scale; }

  void addPlugin(const string& id, shared_ptr<IPlugin> plugin) { plugins[id] = plugin; }
  shared_ptr<IPlugin> getPlugin(const string& id) { return plugins[id]; }
  vector<string> listPluginIds() {
    vector<string> result;
    for (auto const& [key, val] : plugins)
      result.push_back(key);
    return result;
  }
  void removePlugin(const string& pluginId) {
    if (plugins.count(pluginId) == 0)
      return;
    plugins.erase(pluginId);
  }
  void replacePlugin(const string& id, shared_ptr<IPlugin> plugin) {
    removePlugin(id);
    addPlugin(id, plugin);
  }

  void runPlugins(IWorld* world) {
    for (auto const& [key, val] : plugins)
      val->execute(world, this);
  }

 private:
  string id;
  t::position position = t::position{0, 0, 0};
  t::rotation rotation = t::rotation{0, 0, 0, 0};
  t::scale scale = t::scale{1, 1, 1};
  map<string, shared_ptr<IPlugin>> plugins;
};

class World : public IWorld {
 public:
  explicit World(const string& id) : id{id} {}

  const string& getId() { return id; }

  size_t objectCount() { return objects.size(); }
  shared_ptr<IObject> newObject(const string& id) {
    auto object = make_shared<Object>(id);
    objects[id] = object;
    return object;
  }
  shared_ptr<IObject> getObject(const string& id) { return objects[id]; }
  void deleteObject(const string& id) { objects.erase(id); }
  vector<string> listObjectIds() {
    vector<string> result;
    for (auto const& [key, val] : objects)
      result.push_back(key);
    return result;
  }

  void saveScriptToObject(const string& objectId, const string& pluginId, const string& code) {
    if (objects.count(objectId) == 0)
      return;
    auto object = objects[objectId];
    auto plugin = Scripts::asPlugin(pluginId, code);
    object->replacePlugin(pluginId, plugin);
  }

  void round() {
    for (auto const& [key, val] : objects)
      val->runPlugins(this);
    while (updateQueue.processNext()) {}
  }

  void save(const string& path);

 private:
  string id;
  map<string, shared_ptr<Object>> objects;
  UpdateQueue updateQueue;
};

shared_ptr<IWorld> Worlds::createNew(const string& id) {
  return make_shared<World>(id);
}

// World loading and saving

void loadObjectFromYaml(shared_ptr<IWorld> world, const string& path, YAML::Node objectConf);
void loadPluginFromYalm(shared_ptr<IWorld> world, const string& objectId, const string& path, YAML::Node pluginConf);

shared_ptr<IWorld> Worlds::load(const string& id, const string& path) {
  auto world = make_shared<World>(id);
  auto config = YAML::LoadFile(path + "/world.yaml");
  for (auto const& objectConf : config["objects"])
    loadObjectFromYaml(world, path, objectConf);

  return world;
}

void loadObjectFromYaml(shared_ptr<IWorld> world, const string& path, YAML::Node objectConf) {
  auto objectId = objectConf["id"].as<string>();
  auto object = world->newObject(objectId);
  object->setPosition(t::position{
    objectConf["position"]["x"].as<double>(),
    objectConf["position"]["y"].as<double>(),
    objectConf["position"]["z"].as<double>()
  });
  object->setRotation(t::rotation{
    objectConf["rotation"]["x"].as<double>(),
    objectConf["rotation"]["y"].as<double>(),
    objectConf["rotation"]["z"].as<double>(),
    objectConf["rotation"]["w"].as<double>()
  });
  object->setScale(t::scale{
    objectConf["scale"]["x"].as<double>(),
    objectConf["scale"]["y"].as<double>(),
    objectConf["scale"]["z"].as<double>()
  });
  for (auto const& pluginConf : objectConf["plugins"])
    loadPluginFromYalm(world, objectId, path, pluginConf);
}

void loadPluginFromYalm(shared_ptr<IWorld> world, const string& objectId, const string& path, YAML::Node pluginConf) {
  auto pluginId = pluginConf["id"].as<string>();
  if (pluginConf["type"].as<string>() == "script") {
    ifstream scriptFile(path + "/scripts/" + objectId +"_" + pluginId);
    stringstream buffer;
    buffer << scriptFile.rdbuf();
    scriptFile.close();
    world->saveScriptToObject(objectId, pluginId, buffer.str());
  }
}

void World::save(const string& path)  {
  auto worldPath = fs::path(path);
  if (!fs::exists(path))
    fs::create_directories(path);

  auto scriptsPath = fs::path(path + "/scripts");
  if (!fs::exists(scriptsPath))
    fs::create_directories(scriptsPath);

  YAML::Emitter yaml;
  yaml.SetIndent(2);

  yaml << YAML::BeginMap;
  yaml << YAML::Key << "objects";
  yaml << YAML::Value << YAML::BeginSeq;

  for (auto const& [objectId, object] : objects) {
    yaml << YAML::BeginMap;
    yaml << YAML::Key << "id";
    yaml << YAML::Value << objectId;

    yaml << YAML::Key << "position" << YAML::Value << YAML::BeginMap;
    yaml << YAML::Key << "x" << YAML::Value << object->getPosition().x;
    yaml << YAML::Key << "y" << YAML::Value << object->getPosition().y;
    yaml << YAML::Key << "z" << YAML::Value << object->getPosition().z;
    yaml << YAML::EndMap;

    yaml << YAML::Key << "rotation" << YAML::Value << YAML::BeginMap;
    yaml << YAML::Key << "x" << YAML::Value << object->getRotation().x;
    yaml << YAML::Key << "y" << YAML::Value << object->getRotation().y;
    yaml << YAML::Key << "z" << YAML::Value << object->getRotation().z;
    yaml << YAML::Key << "w" << YAML::Value << object->getRotation().w;
    yaml << YAML::EndMap;

    yaml << YAML::Key << "scale" << YAML::Value << YAML::BeginMap;
    yaml << YAML::Key << "x" << YAML::Value << object->getScale().x;
    yaml << YAML::Key << "y" << YAML::Value << object->getScale().y;
    yaml << YAML::Key << "z" << YAML::Value << object->getScale().z;
    yaml << YAML::EndMap;

    yaml << YAML::Key << "plugins" << YAML::Value << YAML::BeginSeq;
    for (auto const& pluginId : object->listPluginIds()) {
      auto plugin = object->getPlugin(pluginId);
      yaml << YAML::BeginMap;
      yaml << YAML::Key << "id" << YAML::Value << pluginId;
      if (plugin->getType() == IPlugin::Type::SCRIPT)
        yaml << YAML::Key << "type" << YAML::Value << "script";
      yaml << YAML::EndMap;

      plugin->saveToFile(path + "/scripts/" + objectId + "_" + pluginId);
    }
    yaml << YAML::EndSeq;

    yaml << YAML::EndMap;
  }

  yaml << YAML::EndSeq;
  yaml << YAML::EndMap;

  ofstream yamlOut(path + "/world.yaml");
  yamlOut << yaml.c_str();
  yamlOut.close();
}

}  // namespace core
