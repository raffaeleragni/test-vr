#include <exception>
#include <iostream>

#include "application.hpp"
#include "rendering.hpp"

using std::cout;
using std::exception;
using std::make_shared;
using std::shared_ptr;
using application::IApplication;

void start() {
  try {
    printf("Starting application...\n");
    auto app = IApplication::create();
    printf("Starting game loop...\n");
    app->gameLoop();
    printf("End of application.\n");
  } catch (const std::exception& e) {
    cout << e.what() << std::endl;
  }
}

namespace application {

class VRHeadset : public IVRHeadset {
 public:
  VRHeadset();
  ~VRHeadset();
 private:
  vr::IVRSystem* pHMD = NULL;
  t::position position = t::position{ 0, 0, 0 };
  t::rotation rotation = t::rotation{ 0, 0, 0, 0 };
};

VRHeadset::VRHeadset() {
  vr::EVRInitError eError = vr::VRInitError_None;
  pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
  if (eError != vr::VRInitError_None)
    throw std::runtime_error("Unable to init VR runtime");

  if (!vr::VRCompositor())
    throw std::runtime_error("Compositor initialization failed");
}

VRHeadset::~VRHeadset() {
  vr::VR_Shutdown();
  pHMD = NULL;
}

class Application : public IApplication {
 public:
  Application() {
    pRendering = IRendering::create();
  }
  ~Application() {
    delete(pRendering);
    delete(pVRHeadset);
  }
  void gameLoop();
  void toggleVR();
  void processSDLEvent(const SDL_Event& sdlEvent);
  void processSDLKeydown(const SDL_Event& sdlEvent);
  bool initVR();
  void closeVR();
 private:
  IRendering* pRendering = NULL;
  IVRHeadset* pVRHeadset = NULL;
  bool vrON = false;
};

shared_ptr<IApplication> IApplication::create() {
  return make_shared<Application>();
}

void Application::gameLoop() {
  SDL_Event sdlEvent = SDL_Event();
  while (sdlEvent.type != SDL_QUIT) {
    SDL_PollEvent(&sdlEvent);
    processSDLEvent(sdlEvent);
    pRendering->renderFrame();
    SDL_Delay(10);
  }
}

void Application::processSDLEvent(const SDL_Event & sdlEvent) {
  if (sdlEvent.type == SDL_KEYDOWN)
    processSDLKeydown(sdlEvent);
}

void Application::processSDLKeydown(const SDL_Event& sdlEvent) {
  if (sdlEvent.key.keysym.sym == SDLK_F8)
    toggleVR();
}

void Application::toggleVR() {
  vrON = !vrON;
  std::cout << "Changing VR to " << vrON << std::endl;
  if (vrON)
    vrON = initVR();
  else
    closeVR();
}

bool Application::initVR() {
  try {
    if (pVRHeadset == NULL)
      pVRHeadset = new VRHeadset();

    return true;
  }
  catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

void Application::closeVR() {
  delete pVRHeadset;
  pVRHeadset = NULL;
}

}  // namespace application
