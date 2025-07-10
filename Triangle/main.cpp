// main.cpp : Entry point of the program.
// This is where the entire OpenGL application is initialized, shaders and
// meshes are created, and the render loop is run until the window is closed.

#include <glm/ext/matrix_transform.hpp>
#include <vector> // Used to store multiple Mesh* and Shader objects dynamically

// GLEW and GLFW are core libraries for OpenGL setup and function management
#include <GL/glew.h> // Loads OpenGL function pointers (modern OpenGL doesn’t expose core functions by default)
#include <GLFW/glfw3.h> // Handles window creation, input handling, and OpenGL context

// GLM is the mathematics library used to create and manipulate transformation
// matrices
#include <glm/glm.hpp>                  // Base vector/matrix types
#include <glm/gtc/matrix_transform.hpp> // Provides glm::translate, glm::rotate, glm::scale, etc.
#include <glm/gtc/type_ptr.hpp> // Allows sending glm matrices to shaders via glUniformMatrix4fv

// Our own engine modules that abstract OpenGL setup
#include "Camera.h"
#include "Mesh.h" // Handles creation and rendering of geometry (VAO, VBO, IBO)
#include "Shader.h" // Compiles vertex + fragment shaders and manages uniforms
#include "Window.h" // Manages GLFW window, context, and framebuffer size

// Constant to convert degrees to radians (required by glm::rotate and others)
const float toRadians = 3.14159265f / 180.0f;

// Global variable to store current rotation angle (used for
// animation/transform)
float curAngle = 0.0f;

// Main window object — wraps the GLFW window + OpenGL context
Window mainWindow;

// Lists of meshes and shaders used in the scene
// These store dynamically allocated Mesh* and Shader objects to be drawn
// per-frame
std::vector<Mesh *> meshList;
std::vector<Shader> shaderList;

Camera camera;

GLfloat deltaTime{0.0f};
GLfloat lastTime{0.0f};
// Paths to vertex and fragment shader source files
// These are relative paths pointing to the Shaders/ directory in your project
// structure
static const char *vShader =
    "Shaders/shader.vert"; // Vertex shader: transforms vertices, outputs color
static const char *fShader = "Shaders/shader.frag"; // Fragment shader: receives
                                                    // color, writes pixel color

// CreateObjects() sets up the vertex and index data for one or more meshes.
// This populates the meshList vector with Mesh objects that contain VAO, VBO,
// and IBO.
//
// The geometry defined here represents a triangle-based shape composed of 4
// vertices (though only 3 are unique in position), and 12 indices connecting
// them.
void CreateObjects() {
  // Index buffer: defines how to connect vertices into triangles
  // Each group of 3 indices forms one triangle (0-based index into the vertex
  // array) Example:
  //   Triangle 1: vertex 0 → vertex 3 → vertex 1
  //   Triangle 2: vertex 1 → vertex 3 → vertex 2
  //   Triangle 3: vertex 2 → vertex 3 → vertex 0
  //   Triangle 4: vertex 0 → vertex 1 → vertex 2
  unsigned int indices[] = {0, 3, 1, 1, 3, 2, 2, 3, 0, 0, 1, 2};

  // Vertex buffer: raw XYZ positions of 4 vertices
  // Each group of 3 floats is a vertex (x, y, z)
  // Total: 4 vertices × 3 floats = 12 floats
  // These define a 2D diamond shape in the XY plane
  GLfloat vertices[] = {
      -1.0f, -1.0f, 0.0f, // Vertex 0
      0.0f,  -1.0f, 1.0f, // Vertex 1
      1.0f,  -1.0f, 0.0f, // Vertex 2
      0.0f,  1.0f,  0.0f  // Vertex 3
  };
  // cube
  GLfloat cubeVertices[] = {
      // Front face
      -1, -1, 1, // 0
      1, -1, 1,  // 1
      1, 1, 1,   // 2
      -1, 1, 1,  // 3
      // Back face
      -1, -1, -1, // 4
      1, -1, -1,  // 5
      1, 1, -1,   // 6
      -1, 1, -1   // 7
  };
  // Winding order: OpenGL by default assumes counter-clockwise (CCW) vertex
  // order for front-facing triangles.
  // Each triangle is defined in CCW order when looking at the face from outside
  // the cube.
  //
  // Front face
  // 0, 1, 2,  // bottom-left → bottom-right → top-right (CCW)
  // 0, 2, 3   // bottom-left → top-right → top-left (CCW)
  //
  //   7--------6
  //  /|       /|
  // 3--------2 |
  // | |      | |
  // | 4------|-5
  // |/       |/
  // 0--------1
  unsigned int cubeIndices[] = {// Front
                                0, 1, 2, 0, 2, 3,
                                // Back
                                5, 4, 7, 5, 7, 6,
                                // Left
                                4, 0, 3, 4, 3, 7,
                                // Right
                                1, 5, 6, 1, 6, 2,
                                // Top
                                3, 2, 6, 3, 6, 7,
                                // Bottom
                                4, 5, 1, 4, 1, 0};

  // Allocate and construct a Mesh object dynamically on the heap
  // Uploads vertex and index data to the GPU
  Mesh *obj1 = new Mesh();
  obj1->CreateMesh(vertices, indices, 12,
                   12);     // 12 floats (4 vertices), 12 indices
  meshList.push_back(obj1); // Store pointer in mesh list for later drawing

  // create a 2d rectangle
  Mesh *cube = new Mesh();
  cube->CreateMesh(cubeVertices, cubeIndices, 24, 36);
  meshList.push_back(cube);

  // Create another mesh using the same geometry (e.g., to draw twice at
  // different positions)
  // Mesh *obj2 = new Mesh();
  // obj2->CreateMesh(vertices, indices, 12, 12);
  // meshList.push_back(obj2);
}

// CreateShaders() compiles vertex + fragment shaders from source files
// and links them into a complete GPU shader program (GLSL).
// The resulting program object is stored in shaderList and used during
// rendering.
void CreateShaders() {
  Shader *shader1 = new Shader(); // Dynamically allocate a Shader object

  // Loads source code from file, compiles vertex and fragment shaders,
  // links them into a single shader program, and stores its handle.
  shader1->CreateFromFiles(vShader, fShader);

  // Note: shaderList stores Shader objects (not pointers), so we dereference
  // and copy This is fine for small projects, but in real engines you'd store
  // pointers or use smart pointers.
  shaderList.push_back(*shader1);
}

int main() {
  // === 1. Initialize window and OpenGL context ===
  // This sets up everything required to render: a window, OpenGL context, GLEW
  // extensions, depth testing, etc. Without this, you can't issue any OpenGL
  // draw calls.
  mainWindow.initialize();

  // === 2. Create GPU-side mesh data ===
  // This uploads vertex and index buffers to GPU memory (VBO, IBO),
  // and configures VAOs (vertex attribute objects) which describe input
  // layouts.
  CreateObjects();

  // === 3. Compile shaders and create a shader program ===
  // This reads the shader source code from .vert and .frag files,
  // compiles them individually, and links them into a complete GPU shader
  // pipeline.
  CreateShaders();

  camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  -90.0f, 0.0f, 5.0f, 0.5f);

  // === 4. Declare uniform variable locations ===
  // These will store GPU-side locations of the 'model' and 'projection'
  // matrices in your vertex shader. These locations are retrieved by name and
  // used to send transformation data to the shader.
  GLuint uniformModel{0}, uniformProjection{0}, uniformView{0};

  // === 5. Setup projection matrix ===
  // This projection matrix converts world-space coordinates into clip space
  // using perspective projection (like a real camera).
  //
  // Arguments:
  // - 45.0f: Field of View in degrees
  // - Aspect Ratio: width / height of framebuffer (not screen resolution!)
  // - Near/Far: distances at which rendering starts/stops
  //
  // The resulting 4x4 matrix will later be passed to the vertex shader uniform
  // called "projection"
  glm::mat4 projection =
      glm::perspective(45.0f,
                       (GLfloat)mainWindow.getBufferWidth() /
                           (GLfloat)mainWindow.getBufferHeight(),
                       0.1f, 100.0f);
  // Loop until window is closed
  while (!mainWindow.getShouldClose()) {

    // delta time calculation
    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    lastTime = now;
    // === 1. Poll events from window system ===
    // This checks for events like key presses, mouse input, window close, etc.
    // It must be called every frame to keep the window responsive.
    glfwPollEvents();
    camera.keyControl(mainWindow.getsKeys(), deltaTime);
    camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

    // === 2. Clear the color and depth buffers ===
    // glClearColor sets the background color of the screen.
    // The glClear call resets both the color buffer (pixels) and the depth
    // buffer (used for depth testing).
    //
    // Without clearing, you'd render on top of the previous frame, causing
    // artifacts.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // === 3. Bind the active shader program ===
    // All vertex and fragment shader operations that follow will use this
    // program. It must be bound before setting uniforms or drawing geometry.
    shaderList[0].UseShader();

    // === 4. Get uniform locations from the shader ===
    // These are required to update uniform variables in the GPU
    // (model/projection matrices). Uniforms are shader-wide global variables —
    // each draw call must set them correctly.
    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();

    // === 5. Update rotation angle (for animation) ===
    // This simulates rotation over time by increasing the angle every frame.
    // `curAngle` is used in the model matrix to rotate the object around an
    // arbitrary axis.
    curAngle += 1.0f;
    if (curAngle >= 360) {
      curAngle -= 360; // Wrap around to prevent float overflow in long sessions
    }

    // === 6. Construct the model matrix ===
    // The model matrix transforms an object from local space → world space.
    // This matrix stack applies:
    // - Identity: base matrix (no transform)
    // - Translation: move the object away from camera (along -Z axis)
    // - Rotation: rotate around the axis (1, 1, 1)
    // - Scale: shrink object uniformly in X and Y
    glm::mat4 model = glm::identity<glm::mat4>();

    // Move object away from camera so it's visible in view frustum
    model = glm::translate(model, glm::vec3(-0.8f, 0.0f, -2.5f));

    // Apply rotation around arbitrary axis (1,1,1) using current angle (in
    // radians)
    model =
        glm::rotate(model, curAngle * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));

    // Scale down uniformly in X and Y, no scale on Z (1.0 means unchanged)
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

    // Upload model matrix to GPU as uniform (will be used in vertex shader to
    // transform vertices)
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    // Upload projection matrix to GPU (used to convert to clip space)
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
                       glm::value_ptr(projection));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE,
                       glm::value_ptr(camera.calculateViewMatrix()));
    // === 7. Draw first mesh (rotating diamond) ===
    // This object is animated — rotation is applied each frame via the model
    // matrix above. RenderMesh() binds its VAO and draws it using
    // glDrawElements().
    meshList[0]->RenderMesh();

    // === 8. Prepare model matrix for second object (static) ===
    // This object will not rotate — just translated upward and scaled.
    // Matrix stack:
    // - Identity → Translate (up 1.0 in Y) → Scale
    // model = glm::identity<glm::mat4>();
    // model = glm::translate(
    //     model, glm::vec3(0.0f, 1.0f,
    //                      -2.5f)); // Positioned slightly above the first mesh
    // model = glm::scale(
    //     model, glm::vec3(0.4f, 0.4f, 1.0f)); // Same scale but no rotation
    //
    // // === 9. Upload updated model matrix for second object ===
    // // You must update uniforms every time they change before a draw call —
    // // they're not cached across draws.
    // glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    // glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
    //                    glm::value_ptr(projection));

    // === 10. Draw second mesh ===
    // Uses the same shader program and geometry as the first mesh, but
    // transformed differently.
    // meshList[1]->RenderMesh();

    // === 11. Optional cleanup: unbind current shader ===
    // This deactivates the current program (no program will be active after
    // this). Technically optional in small apps, but good practice for state
    // management in engines.

    model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(1.5f, 0.0f, -5.0f));
    model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
    model = glm::rotate(model, 2 * curAngle * toRadians, glm::vec3(1, -1, -1));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
                       glm::value_ptr(projection));
    meshList[1]->RenderMesh();
    glUseProgram(0);

    // === 12. Swap front and back buffers ===
    // This presents the rendered image to the screen.
    // Double-buffering is used to avoid flickering and tearing.
    mainWindow.swapBuffers();
  }

  // === 13. Application finished ===
  // GLFW will automatically clean up the window and terminate the context via
  // Window destructor.
  return 0;
}
