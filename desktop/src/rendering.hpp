#ifndef DESKTOP_SRC_RENDERING_HPP_
#define DESKTOP_SRC_RENDERING_HPP_

#include <SDL.h>
#include <SDL_opengl.h>
#include <openvr.h>
#include "core.hpp"

namespace application {

class IRendering {
 public:
  static IRendering* create();
  virtual ~IRendering() {}
  virtual void renderFrame() = 0;
};

}  // namespace application

#endif  // DESKTOP_SRC_RENDERING_HPP_
