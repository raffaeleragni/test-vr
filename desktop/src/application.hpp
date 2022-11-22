#ifndef DESKTOP_SRC_APPLICATION_HPP_
#define DESKTOP_SRC_APPLICATION_HPP_

#include <memory>

#include "core.hpp"

void start();

namespace application {

class IVRHeadset {
 public:
  virtual ~IVRHeadset() {}
};

class IApplication {
 public:
  virtual ~IApplication() {}
  virtual void gameLoop() = 0;
  static shared_ptr<IApplication> create();
};

}  // namespace application
#endif  // DESKTOP_SRC_APPLICATION_HPP_
