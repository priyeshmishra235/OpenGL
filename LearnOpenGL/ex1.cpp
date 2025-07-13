#include "glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

// Exercise
// 1.:Done Try to draw 2 triangles next to each other using glDrawArrays by
// adding more vertices to your data: solution.
//
// 2.:Done Now create the same 2 triangles using two different VAOs and VBOs for
// their data: solution.
//
// 3. Create two shader programs where the second program uses a different
// fragment shader that outputs the color yellow; draw both triangles again
// where one outputs the color yellow: solution.
//
// ─── globals───────────────────────────────────────────────────────────────
int width{800}, height{600};
GLuint VBO[2], VAO[2], EBO{}, fragmentShader{}, fragmentShader2{},
    vertexShader{}, shaderProgram{}, shaderProgram2{};

//----Shader code ------------------------------------------------------------
const char *vertexShaderSource = R"(#version 330 core
layout(location = 0) in vec3 aPos;
void main() { gl_Position = vec4(aPos, 1.0);})";

const char *fragmentShaderSource = R"(#version 330 core
out vec4 FragColor;
void main() { FragColor = vec4(0.5f,0.5f,0.2f,1.0f);})";

const char *fragmentShaderSource2 = R"(#version 330 core
out vec4 FragColor2;
void main() { FragColor2 = vec4(0.0f,0.5f,0.0f,1.0f);})";
// ----Helpers ---------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
GLuint compile(GLenum type, const char *src);

// float vertices[] = {
//     // triangle A (left)
//     -0.9f, -0.5f, 0.0f, // v0
//     -0.1f, -0.5f, 0.0f, // v1
//     -0.5f, 0.5f,  0.0f, // v2
//
// };
// float vertices2[] = {
//     // triangle B (right)
//     0.1f, -0.5f, 0.0f, // v3
//     0.9f, -0.5f, 0.0f, // v4
//     0.5f, 0.5f,  0.0f  // v5
// };

const float vertices[] = {
    /* left vertical bar (red) — 4 × triangles */
    -0.35f, 0.50f, 0.0f, -0.25f, 0.50f, 0.0f, -0.35f, -0.50f, 0.0f,  // 1
    -0.35f, -0.50f, 0.0f, -0.25f, 0.50f, 0.0f, -0.25f, -0.50f, 0.0f, // 2

    /* top horizontal bar (red) — 2 × triangles */
    -0.25f, 0.50f, 0.0f, 0.25f, 0.50f, 0.0f, -0.25f, 0.35f, 0.0f, // 2
    -0.25f, 0.35f, 0.0f, 0.25f, 0.50f, 0.0f, 0.25f, 0.35f, 0.0f,  // 3

    /* middle horizontal bar (red) — 2 × triangles */
    -0.25f, 0.05f, 0.0f, 0.15f, 0.05f, 0.0f, -0.25f, -0.10f, 0.0f, // 4
    -0.25f, -0.10f, 0.0f, 0.15f, 0.05f, 0.0f, 0.15f, -0.10f, 0.0f, // 5

    /* four blue diagonal trims (one at each bar end) — 4 × triangles */
    -0.35f, 0.50f, 0.0f, -0.25f, 0.50f, 0.0f, -0.25f, 0.35f, 0.0f,   // 6
    -0.25f, 0.50f, 0.0f, 0.25f, 0.50f, 0.0f, 0.25f, 0.35f, 0.0f,     // 7
    -0.35f, -0.10f, 0.0f, -0.25f, 0.05f, 0.0f, -0.25f, -0.10f, 0.0f, // 8
    -0.25f, 0.05f, 0.0f, 0.15f, 0.05f, 0.0f, 0.15f, -0.10f, 0.0f,    // 9
};
// unsigned int indices[] = {
//
// };
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(width, height, "GL Test Window", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // compile the vertex and frag shader and then unlink them , so later we can
  // use in while render loop when needed // this should be done outside while
  vertexShader = compile(GL_VERTEX_SHADER, vertexShaderSource);
  fragmentShader = compile(GL_FRAGMENT_SHADER, fragmentShaderSource);
  fragmentShader2 = compile(GL_FRAGMENT_SHADER, fragmentShaderSource2);

  shaderProgram = glCreateProgram();
  shaderProgram2 = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glAttachShader(shaderProgram2, vertexShader);
  glAttachShader(shaderProgram2, fragmentShader2);
  glLinkProgram(shaderProgram2);
  //
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteShader(fragmentShader2);

  // 1. Create and bind VAO first — it must store attribute state
  glGenVertexArrays(2, VAO);
  glGenBuffers(2, VBO);

  glBindVertexArray(VAO[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // glBindVertexArray(VAO[1]);
  // glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2,
  // GL_STATIC_DRAW); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 *
  // sizeof(float), (void *)0); glEnableVertexAttribArray(0); // either
  // 0->enable 1->disable

  // 3. Create and bind EBO
  // glGenBuffers(1, &EBO);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //              GL_STATIC_DRAW);

  // 4. Set vertex attribute layout (stored in VAO)

  // 5. VAO now fully configured
  glBindVertexArray(0); // optional unbind for safety

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    // rendering commands

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO[0]); // binds VBO + EBO + attribs as stored
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    glUseProgram(shaderProgram2);
    glDrawArrays(GL_TRIANGLES, 18, 12);

    // glUseProgram(shaderProgram2);
    // glBindVertexArray(VAO[1]); // binds VBO + EBO + attribs as stored
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteBuffers(2, VBO);
  glDeleteVertexArrays(2, VAO);
  glDeleteProgram(shaderProgram);
  glDeleteProgram(shaderProgram2);
  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

GLuint compile(GLenum type, const char *src) {
  GLuint sh = glCreateShader(type);
  glShaderSource(sh, 1, &src, NULL);
  glCompileShader(sh);
  GLint ok{};
  glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetShaderInfoLog(sh, 512, nullptr, log);
    std::cerr << "Shader compile failed:\n" << log << '\n';
    std::exit(EXIT_FAILURE);
  }
  return sh;
}
