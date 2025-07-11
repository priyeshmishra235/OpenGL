#include "Window.h"
#include <GLFW/glfw3.h>
Window::Window() {
  width = 800;
  height = 600;

  bufferWidth = 0;
  bufferHeight = 0;

  mainWindow = 0;

  for (size_t i{0}; i < 1024; i++) {
    keys[i] = 0;
  }
}
Window::Window(GLint windowWidth, GLint windowHeight) {
  width = windowWidth;
  height = windowHeight;

  bufferWidth = 0;
  bufferHeight = 0;

  mainWindow = 0;
  for (size_t i{0}; i < 1024; i++) {
    keys[i] = 0;
  }
}

int Window::initialize() {
  if (!glfwInit()) {
    std::cerr << "GLFW library load failed...\n";
    glfwTerminate();
    return 1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  mainWindow = glfwCreateWindow(width, height, "GL Test Window", NULL, NULL);
  if (!mainWindow) {
    std::cerr << "GLFW window creation failed...\n";
    glfwTerminate();
    return 1;
  }
  glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
  glfwMakeContextCurrent(mainWindow);
  createCallbacks();
  glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glewExperimental = GLU_TRUE;
  GLenum error = glewInit();
  if (error != GLEW_OK) {
    std::cerr << "GLEW initialization failed...\n";
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    return 1;
  }
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, bufferWidth * 1.5, bufferHeight * 1.5);

  glfwSetWindowUserPointer(mainWindow, this);
  return 0;
}
void Window::createCallbacks() {
  glfwSetKeyCallback(mainWindow, handleKeys);
  glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getXChange() {
  GLfloat theChange = xChange;
  xChange = 0.0f;
  return theChange;
}
GLfloat Window::getYChange() {
  GLfloat theChange = yChange;
  yChange = 0.0f;
  return theChange;
}

void Window::handleKeys(GLFWwindow *window, int key, int code, int action,
                        int mode) {
  Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
  if (key >= 0 && key <= 1024) {
    if (action == GLFW_PRESS) {
      theWindow->keys[key] = true;
    } else if (action == GLFW_RELEASE) {
      theWindow->keys[key] = false;
    }
  }
}
void Window::handleMouse(GLFWwindow *window, double xPos, double yPos) {
  Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
  if (theWindow->mouseFirstMoved) {
    theWindow->lastx = xPos;
    theWindow->lasty = yPos;
    theWindow->mouseFirstMoved = false;
  }
  theWindow->xChange = xPos - theWindow->lastx;
  theWindow->yChange = theWindow->lasty - yPos;

  theWindow->lastx = xPos;
  theWindow->lasty = yPos;
}
Window::~Window() {
  glfwDestroyWindow(mainWindow);

  glfwTerminate();
}
