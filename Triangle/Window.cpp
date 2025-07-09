#include "Window.h" // Include the class declaration

// Default constructor: sets a standard window size and initializes internal
// state. These values are later used during GLFW window creation in
// initialize().
Window::Window() {
  width = 800;  // Default window width in screen pixels
  height = 600; // Default window height in screen pixels

  bufferWidth =
      0; // Will be filled by glfwGetFramebufferSize() after window creation
  bufferHeight = 0;

  mainWindow = 0; // Null pointer — window will be created later by initialize()
}

// Parameterized constructor: allows the user to define a custom window size.
// Like the default constructor, this doesn't create the window yet — it just
// stores the configuration.
Window::Window(GLint windowWidth, GLint windowHeight) {
  width = windowWidth;
  height = windowHeight;

  bufferWidth = 0; // Again, real values set after GLFW creates framebuffer
  bufferHeight = 0;

  mainWindow = 0; // Will point to GLFWwindow once it's created
}

int Window::initialize() {
  // Step 1: Initialize GLFW — this sets up OS-level window/context creation.
  // Required before calling any GLFW functions.
  if (!glfwInit()) {
    std::cerr << "GLFW library load failed...\n";
    glfwTerminate(); // Clean up if init fails
    return 1;        // Non-zero = failure
  }

  // ------------------------------------------------------------
  // Step 2: GLFW Window Configuration
  // ------------------------------------------------------------

  // Request OpenGL 3.3 core profile
  // Core profile means no deprecated functionality (e.g., fixed-function
  // pipeline is removed)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  // Use the Core Profile — required for modern OpenGL on macOS and Linux
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Forward compatibility: allows future OpenGL versions by removing legacy
  // features Required on macOS and good for clean modern context
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Step 3: Create the actual window and OpenGL context
  // Parameters:
  // - width, height: window dimensions
  // - "GL Test Window": window title
  // - NULL, NULL: not using fullscreen or shared context
  mainWindow = glfwCreateWindow(width, height, "GL Test Window", NULL, NULL);
  if (!mainWindow) {
    std::cerr << "GLFW window creation failed...\n";
    glfwTerminate();
    return 1;
  }

  // Step 4: Retrieve actual framebuffer size (in pixels, not screen coords)
  // This is necessary because high-DPI displays may scale the window
  glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

  // Step 5: Make this window the current OpenGL context
  // All subsequent OpenGL commands will affect this context
  glfwMakeContextCurrent(mainWindow);

  // Step 6: Prepare GLEW to initialize OpenGL function pointers
  // This must be set to TRUE to get access to newer OpenGL functions
  glewExperimental = GLU_TRUE;

  // Initialize GLEW — this loads all available OpenGL extensions for this
  // context
  GLenum error = glewInit();
  if (error != GLEW_OK) {
    std::cerr << "GLEW initialization failed...\n";
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    return 1;
  }

  // Step 7: Enable depth testing in OpenGL
  // This allows 3D objects to be correctly occluded based on depth values
  // (Z-buffering)
  glEnable(GL_DEPTH_TEST);

  // Step 8: Configure OpenGL's viewport to match framebuffer size
  // Maps normalized device coordinates [-1, 1] to the actual pixel region
  glViewport(0, 0, bufferWidth, bufferHeight);

  return 0; // Success
}
// Destructor: called automatically when a `Window` object goes out of scope.
//
// This handles resource cleanup by:
// - destroying the GLFW window (releases the OpenGL context and window memory)
// - terminating the GLFW library (frees all associated internal resources)
//
// In graphics applications, it's important to clean up to avoid memory/resource
// leaks especially when debugging GPU usage, or on platforms with limited
// resources (e.g., mobile, embedded).
Window::~Window() {
  glfwDestroyWindow(mainWindow); // Destroys the window and its OpenGL context

  glfwTerminate(); // Cleans up GLFW itself (you can only call this once per
                   // program)
}
