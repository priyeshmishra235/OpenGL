#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#pragma once

class Window {
public:
  Window();
  Window(GLint windowWidth, GLint windowHeight);

  int initialize();

  GLint getBufferWidth() { return bufferWidth; }
  GLint getBufferHeight() { return bufferHeight; }

  bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

  bool *getsKeys() { return keys; }
  GLfloat getXChange();
  GLfloat getYChange();

  void swapBuffers() { glfwSwapBuffers(mainWindow); }

  ~Window();

private:
  GLFWwindow *mainWindow;

  GLint width, height;
  GLint bufferWidth, bufferHeight;

  bool keys[1024];
  GLfloat lastx, lasty;
  GLfloat xChange, yChange;
  bool mouseFirstMoved;

  void createCallbacks();
  static void handleKeys(GLFWwindow *window, int key, int code, int action,
                         int mode);
  static void handleMouse(GLFWwindow *window, double xPos, double yPos);
};
