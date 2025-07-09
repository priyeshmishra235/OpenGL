// Include OpenGL Extension Wrangler - gives access to modern OpenGL functions.
// This must be included before any OpenGL calls.
// GLEW loads function pointers for OpenGL at runtime, since modern OpenGL is
// not exposed directly by system headers.
#include <GL/glew.h>

#include <fstream> // Used to open and read shader files from disk
#include <iostream> // Used for debug logging, especially useful during shader compilation
#include <sstream> // Used to convert file stream to string (shader code)
#include <string> // Used to represent file paths and shader source code as strings

#pragma once // Ensures this header is only included once during the build,
             // avoids duplicate definition errors

// The Shader class is a wrapper around the process of loading, compiling,
// and using GPU shader programs in OpenGL. It abstracts low-level API calls
// and provides a clean interface for managing shaders in a rendering pipeline.
class Shader {
public:
  // Default constructor: initializes internal OpenGL shader program ID.
  // Actual creation of the shader happens later via CreateFromFiles().
  Shader();

  // Loads shader source code from the provided vertex and fragment shader
  // files, compiles them, links them into a shader program, and stores the
  // resulting program ID. This must be called before using the shader.
  //
  // Arguments:
  // - vShader: path to vertex shader file (.vert)
  // - fShader: path to fragment shader file (.frag)
  //
  // Example use: shader.CreateFromFiles("Shaders/shader.vert",
  // "Shaders/shader.frag");
  void CreateFromFiles(const char *vShader, const char *fShader);

  // Activates (binds) the compiled shader program so that subsequent OpenGL
  // draw calls will use it. This should be called before rendering any mesh
  // that uses this shader.
  //
  // In OpenGL, only one shader program can be active at a time. This ensures
  // the GPU uses the correct shaders for transforming vertices and shading
  // fragments.
  void UseShader() { glUseProgram(this->shader); }

  // Returns the uniform location of the "model" matrix in the linked shader
  // program. This is used to send the model matrix (object-to-world transform)
  // from CPU to GPU.
  //
  // This uniform is typically used in the vertex shader to transform
  // object-space vertices into world space or view space using a model matrix
  // (mat4).
  GLuint GetModelLocation() { return this->uniformModel; }

  // Returns the uniform location of the "projection" matrix in the shader
  // program. This allows the application to pass the projection matrix
  // (perspective or orthographic) to the GPU, which is then used to project 3D
  // coordinates into screen space.
  //
  // The projection matrix is applied *after* the view and model
  // transformations.
  GLuint GetProjectionLocation() { return this->uniformProjection; }

  // Destructor: used to free GPU resources associated with this shader program.
  // OpenGL programs are stored on the GPU, so they must be explicitly deleted
  // when no longer in use to avoid memory/resource leaks.
  ~Shader();

private:
  // This is the OpenGL handle (ID) to the compiled shader program created by
  // glCreateProgram. It's used to reference and bind the program when rendering
  // or setting uniforms.
  GLuint shader;

  // These store the uniform location IDs for the `model` and `projection`
  // matrices. These are retrieved after linking the shader program using
  // glGetUniformLocation. They let the application send matrices to the shader
  // via glUniformMatrix4fv.
  GLuint uniformModel;
  GLuint uniformProjection;

  // Reads a text file (typically GLSL shader source) from the given path and
  // returns it as a string. This helper is used internally by CreateFromFiles
  // to load .vert and .frag files.
  std::string readShaderCodeFromFile(const char *shaderPath);

  // Adds and compiles a shader of a given type (vertex or fragment) to a shader
  // program. Internally uses glCreateShader, glShaderSource, glCompileShader,
  // and glAttachShader. If there's a compile error, logs it to stderr.
  void addShader(GLuint theProgram, const char *shaderCode, GLenum shaderType);

  // Compiles and links a vertex shader and a fragment shader into a complete
  // shader program. Handles all OpenGL boilerplate, including shader creation,
  // error checking, and linking.
  //
  // Arguments:
  // - vShaderCode: source code for vertex shader (as a C string)
  // - fShaderCode: source code for fragment shader (as a C string)
  //
  // This function internally:
  //  - creates a program object
  //  - compiles the vertex and fragment shaders
  //  - attaches them to the program
  //  - links the program
  //  - gets the locations of `model` and `projection` uniforms
  void compileShaders(const char *vShaderCode, const char *fShaderCode);
};
