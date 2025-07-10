#include <GL/glew.h> // Gives access to modern OpenGL functions
#include <GLFW/glfw3.h> // Cross-platform library to create OpenGL windows and receive input events
#include <iostream> // For logging and debugging

#pragma once // Prevents this header from being included multiple times in a
             // single translation unit

// Window is a utility class that encapsulates:
// - Creation of an OpenGL-capable GLFW window
// - Initialization of the OpenGL context
// - Access to framebuffer dimensions (needed for setting up the viewport)
// - Management of the window’s lifecycle (creation, destruction, buffer swap)
class Window {
public:
  // Default constructor: creates a window with default resolution (e.g.,
  // 800x600)
  Window();

  // Parameterized constructor: creates a window with user-defined resolution
  Window(GLint windowWidth, GLint windowHeight);

  // Initializes GLFW, creates the window, sets the OpenGL context, and
  // initializes GLEW. Must be called before any OpenGL calls.
  //
  // Returns:
  //   0 on failure, 1 on success
  //
  // Side effects:
  // - Initializes GLFW
  // - Creates a GLFWwindow
  // - Makes the context current
  // - Initializes GLEW
  int initialize();

  // Accessor for framebuffer width (not window width — actual OpenGL rendering
  // size in pixels) Needed for configuring the OpenGL viewport (glViewport).
  GLint getBufferWidth() { return bufferWidth; }

  // Accessor for framebuffer height
  GLint getBufferHeight() { return bufferHeight; }

  // Returns true if the window should close (e.g., user clicked the close
  // button or pressed ESC). Typically used in the main render loop.
  bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

  bool *getsKeys() { return keys; }
  GLfloat getXChange();
  GLfloat getYChange();

  // Swaps the front and back buffers — this displays the next rendered frame on
  // the screen. Should be called once per frame at the end of the render loop.
  void swapBuffers() { glfwSwapBuffers(mainWindow); }

  // Destructor: should clean up GLFW resources (e.g., destroy the window).
  ~Window();

private:
  // Pointer to the GLFW window instance.
  // This object owns the OpenGL context and handles window events.
  GLFWwindow *mainWindow;

  // Requested window width/height (in screen pixels)
  GLint width, height;

  // Actual framebuffer dimensions.
  // On high-DPI displays, this can differ from width/height (e.g., 800x600
  // window → 1600x1200 framebuffer).
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
