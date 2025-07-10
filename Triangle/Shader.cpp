#include "Shader.h" // Includes the declaration of the Shader class
#include <cstring>  // included for C-style string operations

// Constructor: initialize all internal handles to 0 (which is an
// invalid/uninitialized OpenGL ID). This is a good practice so you can check
// later if the shader was successfully compiled.
Shader::Shader() {
  shader = 0;
  uniformModel = 0;
  uniformProjection = 0;
}

// High-level interface to build a shader program from GLSL source files on
// disk.
//
// This is usually called once at startup per shader pipeline (e.g., standard
// lighting shader). It reads the vertex and fragment shader source code from
// disk and passes them to the compiler/linker function to create a complete
// shader program.
void Shader::CreateFromFiles(const char *vShader, const char *fShader) {
  // Read the source code from the vertex shader file (.vert)
  std::string vShaderCode = readShaderCodeFromFile(vShader);

  // Read the source code from the fragment shader file (.frag)
  std::string fShaderCode = readShaderCodeFromFile(fShader);

  // Now compile and link both shader source strings into a complete shader
  // program.
  compileShaders(vShaderCode.c_str(), fShaderCode.c_str());
}

// Destructor: currently empty, but in a complete implementation you would call
// glDeleteProgram(shader) to free GPU resources.
Shader::~Shader() {}

// Reads the entire content of a text file (GLSL shader) into a std::string.
// This is needed because OpenGL expects shader source to be passed as a
// C-string (const char*), but you can't feed it a file stream directly.
//
// Returns:
//   A string containing the GLSL shader code from disk, or an empty string on
//   error.
std::string Shader::readShaderCodeFromFile(const char *shaderPath) {
  std::string code;         // Will hold the contents of the shader file
  std::ifstream shaderFile; // File input stream object

  // Enable exception throwing on file errors (e.g. disk issues)
  shaderFile.exceptions(std::ifstream::badbit);

  try {
    // Open the file from given path
    shaderFile.open(shaderPath);

    // Create a string stream buffer to read entire file content at once
    std::stringstream shaderStream;

    // Read the file's entire contents into the stream buffer
    shaderStream << shaderFile.rdbuf();

    // Close the file after reading (good practice to free OS-level file
    // handles)
    shaderFile.close();

    // Convert the stream to a standard string (std::string)
    code = shaderStream.str();
  } catch (std::ifstream::failure e) {
    // If there's an error (e.g. file missing, corrupted, permission denied),
    // log a message.
    std::cout << "Shader file " << shaderPath << " cannot be read" << std::endl;
  }

  // Return shader source code as string to be compiled by OpenGL later
  return code;
}

// This function compiles a shader (vertex or fragment) from raw GLSL source
// code and attaches it to the given shader program.
//
// Parameters:
// - theProgram: OpenGL shader program ID to which this shader will be attached
// - shaderCode: C-string of GLSL source code (read earlier from a file)
// - shaderType: GLenum indicating shader stage (e.g., GL_VERTEX_SHADER,
// GL_FRAGMENT_SHADER)
void Shader::addShader(GLuint theProgram, const char *shaderCode,
                       GLenum shaderType) {
  // Step 1: Create an empty shader object of the specified type.
  // This is an OpenGL handle to GPU-side shader memory.
  GLuint theShader = glCreateShader(shaderType);

  // Step 2: Prepare pointers and lengths for shader source.
  // `glShaderSource` expects an array of strings (so we pass an array of 1
  // here).
  const GLchar *theCode[1];
  theCode[0] = shaderCode; // Pointer to actual shader code

  // Pass the length of the shader source to OpenGL
  GLint codeLength[1];
  codeLength[0] = strlen(shaderCode); // Calculate string length

  // Step 3: Send source code to OpenGL and compile it.
  // This copies the source into the shader object.
  glShaderSource(theShader, 1, theCode, codeLength);

  // Now compile the shader into machine code on the GPU.
  glCompileShader(theShader);

  // Step 4: Check compilation status for errors.
  GLint result = 0;
  GLchar errLog[1024] = {0}; // Buffer for error message

  // Query the compilation status
  glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

  if (!result) {
    // Compilation failed — retrieve and log the error message
    glGetShaderInfoLog(theShader, sizeof(errLog), NULL, errLog);
    std::cerr << "Error compiling the " << shaderType << " shader: '" << errLog
              << "'\n";
    return; // Do not attach if compilation failed
  }

  // Step 5: Attach compiled shader to the program object.
  // This doesn't link yet — linking happens in compileShaders().
  glAttachShader(theProgram, theShader);
}
// This function compiles and links both vertex and fragment shaders into a
// complete shader program. It also retrieves the locations of uniform variables
// (like model/projection matrices) so the CPU can send data to the GPU before
// rendering.
void Shader::compileShaders(const char *vShaderCode, const char *fShaderCode) {
  // Step 1: Create a new shader program object.
  // This represents the full pipeline configuration (vertex + fragment stages).
  shader = glCreateProgram();

  if (!shader) {
    std::cerr << "Error creating shader program\n";
    return;
  }

  // Step 2: Compile and attach both shader stages.
  // This function will log and exit early if either compilation fails.
  addShader(shader, vShaderCode, GL_VERTEX_SHADER);
  addShader(shader, fShaderCode, GL_FRAGMENT_SHADER);

  // Step 3: Link the shader program.
  // This resolves all input/output variables across stages (e.g. `out` from
  // vertex must match `in` of fragment).
  GLint result = 0;
  GLchar errLog[1024] = {0};

  glLinkProgram(shader);

  // Step 4: Check if linking succeeded.
  glGetProgramiv(shader, GL_LINK_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(shader, sizeof(errLog), NULL, errLog);
    std::cerr << "Error linking program: '" << errLog << "'\n";
    return;
  }

  // Step 5: Validate the program.
  // This ensures that the program is usable in the current OpenGL context.
  // It checks if the program is consistent with the pipeline state.
  glValidateProgram(shader);
  glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(shader, sizeof(errLog), NULL, errLog);
    std::cerr << "Error validating program: '" << errLog << "'\n";
    return;
  }

  // Step 6: Query the locations of uniform variables from the shader.
  // These allow the CPU to set the value of `uniform mat4 model` and `uniform
  // mat4 projection` declared in the GLSL shader code. Uniforms are global
  // shader variables set per-draw call.
  uniformModel = glGetUniformLocation(shader, "model");
  uniformProjection = glGetUniformLocation(shader, "projection");
  uniformView = glGetUniformLocation(shader, "view");
  // Note: if `model` or `projection` is not used in your GLSL code, the
  // compiler may optimize it away. In that case, glGetUniformLocation will
  // return -1.
}
