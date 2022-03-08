#ifndef APPLICATION_CLASS_RENDERING_H
#define APPLICATION_CLASS_RENDERING_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <openvr.h>
#include "core.hpp"

namespace application {

  class IRendering {
  public:
    static IRendering* create();
    virtual ~IRendering() {};
    virtual void renderFrame() = 0;
  };
}

#endif
