#include <SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <exception>
#include <iostream>
#include "rendering.hpp"

#pragma comment(lib, "opengl32.lib")

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

GLuint compileShader(const char* vertexShader, const char* fragmentShader);

namespace application {

class Camera {
 public:
  Camera();
  ~Camera();
 private:
  t::position position;
  t::rotation rotation;
};

class Shaders {
 public:
  Shaders();
  ~Shaders();
  GLuint sceneShaderID = 0;
};

class VRRendering {
  VRRendering();
  ~VRRendering();
 private:
  Camera* pLeftEyeCamera = NULL;
  Camera* pRightEyeCamera = NULL;
};

class HMDCamera : public Camera {
 public:
  explicit HMDCamera(VRRendering* pVRRendering);
  ~HMDCamera();
};

class Rendering: public IRendering {
 public:
  Rendering();
  ~Rendering();
  void renderFrame();
 private:
  void prepareObject();
  SDL_Window* pSDLWindow;
  SDL_GLContext pGLContext = NULL;
  Shaders* pShaders = NULL;
  Camera* pScreenCamera = NULL;
  VRRendering* pVRRendering = NULL;
  GLuint VBO, VAO;
};

Rendering::Rendering() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    throw(std::runtime_error("SDL could not initialize"));

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

  pSDLWindow = SDL_CreateWindow("hellovr", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (pSDLWindow == NULL)
    throw(std::runtime_error("SDL Window could not be created"));

  SDL_SetWindowTitle(pSDLWindow, "HelloVR");

  pGLContext = SDL_GL_CreateContext(pSDLWindow);
  if (pGLContext == NULL)
    throw std::runtime_error("OpenGL context could not be created");

  if (SDL_GL_SetSwapInterval(1) < 0)
    throw std::runtime_error("Unable to set VSync");

  if (glewInit() != GLEW_OK)
    throw std::runtime_error("Error initializing GLEW");
  glGetError();

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  pShaders = new Shaders();
  pScreenCamera = new Camera();

  prepareObject();
}

IRendering* IRendering::create() {
  return new Rendering();
}

void Rendering::prepareObject() {
  GLfloat vertices[] = {
      -0.5f, -0.5f, 0.0f,  // Left
       0.5f, -0.5f, 0.0f,  // Right
       0.0f,  0.5f, 0.0f   // Top
  };
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

Rendering::~Rendering() {
  delete(pShaders);
  delete(pScreenCamera);
  pGLContext = NULL;

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  if (pSDLWindow != NULL)
    SDL_DestroyWindow(pSDLWindow);

  SDL_Quit();
}

void Rendering::renderFrame() {
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(pShaders->sceneShaderID);

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);

  SDL_GL_SwapWindow(pSDLWindow);
}

VRRendering::VRRendering() {
  pLeftEyeCamera = new HMDCamera(this);
  pRightEyeCamera = new HMDCamera(this);
}

VRRendering::~VRRendering() {
  delete(pRightEyeCamera);
  delete(pLeftEyeCamera);
}

Shaders::Shaders() {
  sceneShaderID = compileShader(
    R"SHADER(
      #version 410 core
      layout(location = 0) in vec4 position;
      layout(location = 1) in vec2 v2UVIn;
      noperspective out vec2 v2UV;
      void main() {
        v2UV = v2UVIn;
        gl_Position = position;
      }
    )SHADER", R"SHADER(
      #version 410 core
      uniform sampler2D mytexture;
      noperspective in vec2 v2UV;
      out vec4 outputColor;
      void main() {
        outputColor = texture(mytexture, v2UV);
      }
    )SHADER");
}

Shaders::~Shaders() {
  if (sceneShaderID)
    glDeleteProgram(sceneShaderID);
}

Camera::Camera() {
}

Camera::~Camera() {
}

HMDCamera::HMDCamera(VRRendering* pVRRendering) {
  vr::EVREye eye;
  eye = vr::Eye_Left;
  eye = vr::Eye_Right;
  /*
  vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(nEye, 0.1f, 30.0f);

  projection = return Matrix4(
    mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
    mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
    mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
    mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
  );
  */

  /*
  vr::HmdMatrix34_t matEye = m_pHMD->GetEyeToHeadTransform(nEye);
  Matrix4 matrixObj(
    matEye.m[0][0], matEye.m[1][0], matEye.m[2][0], 0.0,
    matEye.m[0][1], matEye.m[1][1], matEye.m[2][1], 0.0,
    matEye.m[0][2], matEye.m[1][2], matEye.m[2][2], 0.0,
    matEye.m[0][3], matEye.m[1][3], matEye.m[2][3], 1.0f
  );

  eyePos = return matrixObj.invert();
  */
}

HMDCamera::~HMDCamera() {
}

}  // namespace application

GLuint compileShader(const char* vertexShader, const char* fragmentShader) {
  GLuint unProgramID = glCreateProgram();

  GLuint nSceneVertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(nSceneVertexShader, 1, &vertexShader, NULL);
  glCompileShader(nSceneVertexShader);

  GLint vShaderCompiled = GL_FALSE;
  glGetShaderiv(nSceneVertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
  if (vShaderCompiled != GL_TRUE) {
    glDeleteProgram(unProgramID);
    glDeleteShader(nSceneVertexShader);
    throw(std::runtime_error("Unable to compile vertex shader"));
  }
  glAttachShader(unProgramID, nSceneVertexShader);
  glDeleteShader(nSceneVertexShader);  // the program hangs onto this once it's attached

  GLuint nSceneFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(nSceneFragmentShader, 1, &fragmentShader, NULL);
  glCompileShader(nSceneFragmentShader);

  GLint fShaderCompiled = GL_FALSE;
  glGetShaderiv(nSceneFragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
  if (fShaderCompiled != GL_TRUE) {
    glDeleteProgram(unProgramID);
    glDeleteShader(nSceneFragmentShader);
    throw(std::runtime_error("Unable to compile fragment shader"));
  }

  glAttachShader(unProgramID, nSceneFragmentShader);
  glDeleteShader(nSceneFragmentShader);  // the program hangs onto this once it's attached

  glLinkProgram(unProgramID);

  GLint programSuccess = GL_TRUE;
  glGetProgramiv(unProgramID, GL_LINK_STATUS, &programSuccess);
  if (programSuccess != GL_TRUE) {
    // dprintf("%s - Error linking program %d!\n", pchShaderName, unProgramID);
    glDeleteProgram(unProgramID);
    return 0;
  }

  return unProgramID;
}
