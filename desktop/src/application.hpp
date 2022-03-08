#ifndef APPLICATION_CLASS_H
#define APPLICATION_CLASS_H

#include "core.hpp"

void start();

namespace application {

  class IVRHeadset {
  public:
    virtual ~IVRHeadset() {};
  };

  class IApplication {
  public:
    virtual ~IApplication() {};
    virtual void gameLoop() = 0;
    static shared_ptr<IApplication> create();
  };
}
#endif
