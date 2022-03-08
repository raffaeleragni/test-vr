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
#include <iostream>
#include <map>
#include <filesystem>
#include <fstream>

#include "test.hpp"
#include "../src/core.hpp"

using std::string;
using std::ifstream;
using std::stringstream;
namespace fs = std::filesystem;

TEST_CASE("Empty world") {
  auto world = core::Worlds::createNew("id");

  REQUIRE(world != nullptr);
  REQUIRE(world->getId() == "id");
  REQUIRE(world->objectCount() == 0);
}

TEST_CASE("World with one object") {
  auto world = core::Worlds::createNew("id");

  auto object = world->newObject("id");
  REQUIRE(object != nullptr);
  REQUIRE(world->objectCount() == 1);
  REQUIRE(world->listObjectIds() == vector<string>{"id"});

  object = world->getObject("id");
  REQUIRE(object != nullptr);

  world->deleteObject("id");
  REQUIRE(world->objectCount() == 0);
}

TEST_CASE("Object change positions") {
  auto world = core::Worlds::createNew("id");
  auto object = world->newObject("id");

  REQUIRE(object->getPosition() == t::position{0, 0, 0});

  auto pos = t::position{1, 2, 3};
  object->setPosition(pos);

  REQUIRE(object->getPosition() == pos);
}

TEST_CASE("Object change rotations") {
  auto world = core::Worlds::createNew("id");
  auto object = world->newObject("id");

  REQUIRE(object->getRotation() == t::rotation{0, 0, 0, 0});

  auto rot = t::rotation{1, 2, 3, 4};
  object->setRotation(rot);

  REQUIRE(object->getRotation() == rot);
}

TEST_CASE("Object change scales") {
  auto world = core::Worlds::createNew("id");
  auto object = world->newObject("id");

  REQUIRE(object->getScale() == t::scale{1, 1, 1});

  auto s = t::scale{2, 3, 4};
  object->setScale(s);

  REQUIRE(object->getScale() == s);
}

TEST_CASE("Load plugins to non existent object") {
  auto world = core::Worlds::createNew("id");

  REQUIRE(world->objectCount() == 0);

  world->saveScriptToObject("id", "id", "print('Hello injected')");

  REQUIRE(world->objectCount() == 0);
}

TEST_CASE("Load plugin via code") {
  auto world = core::Worlds::createNew("id");
  auto object = world->newObject("id");
  world->saveScriptToObject("id", "id", "print('Hello injected')");

  REQUIRE(object->listPluginIds() == vector<string>{"id"});
  world->round();
}

TEST_CASE("Load world") {
  auto world = core::Worlds::load("id", "sample_worlds/simple_case");

  REQUIRE(world != nullptr);
  REQUIRE(world->getId() == "id");
  REQUIRE(world->objectCount() == 1);

  auto object = world->getObject("id");
  REQUIRE(object != nullptr);
  REQUIRE(object->getId() == "id");

  REQUIRE(object->getPosition() == t::position{1, 2, 3});
  REQUIRE(object->getRotation() == t::rotation{4, 5, 6, 7});
  REQUIRE(object->getScale() == t::scale{8, 9, 10});

  world->round();
}

TEST_CASE("Save world") {
  auto path = fs::path("sample_world_saved");
  auto configPath = fs::path("sample_world_saved/world.yaml");
  auto world = core::Worlds::load("id", "sample_worlds/simple_case");

  fs::remove_all(path);

  world->save("sample_world_saved");

  REQUIRE(fs::exists(path));
  REQUIRE(fs::exists(configPath));

  auto yaml = YAML::LoadFile("sample_world_saved/world.yaml");
  REQUIRE(yaml["objects"]);
  REQUIRE(yaml["objects"].IsSequence());
  REQUIRE(yaml["objects"].size() == 1);
  REQUIRE(yaml["objects"][0]["id"].as<string>() == "id");
  REQUIRE(yaml["objects"][0]["position"]["x"].as<double>() == 1);
  REQUIRE(yaml["objects"][0]["position"]["y"].as<double>() == 2);
  REQUIRE(yaml["objects"][0]["position"]["z"].as<double>() == 3);
  REQUIRE(yaml["objects"][0]["rotation"]["x"].as<double>() == 4);
  REQUIRE(yaml["objects"][0]["rotation"]["y"].as<double>() == 5);
  REQUIRE(yaml["objects"][0]["rotation"]["z"].as<double>() == 6);
  REQUIRE(yaml["objects"][0]["rotation"]["w"].as<double>() == 7);
  REQUIRE(yaml["objects"][0]["scale"]["x"].as<double>() == 8);
  REQUIRE(yaml["objects"][0]["scale"]["y"].as<double>() == 9);
  REQUIRE(yaml["objects"][0]["scale"]["z"].as<double>() == 10);
  REQUIRE(yaml["objects"][0]["plugins"]);
  REQUIRE(yaml["objects"][0]["plugins"].IsSequence());
  REQUIRE(yaml["objects"][0]["plugins"].size() == 1);
  REQUIRE(yaml["objects"][0]["plugins"][0]["id"].as<string>() == "hello-world");
  REQUIRE(yaml["objects"][0]["plugins"][0]["type"].as<string>() == "script");

  ifstream scriptFile("sample_world_saved/scripts/id_hello-world");
  stringstream buffer;
  buffer << scriptFile.rdbuf();
  scriptFile.close();
  REQUIRE(buffer.str() == "print(\"Hello VR\")\n");

  fs::remove_all(path);
}
