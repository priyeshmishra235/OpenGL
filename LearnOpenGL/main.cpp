#include "Shader.h"
#include "glad.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <iostream>

// ─── globals ───────────────────────────────────────────────────────────────
int width{800}, height{600};
GLuint VBO{}, VAO{}, EBO{}, fragmentShader{}, vertexShader{}, shaderProgram{};

// ----Helpers ---------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
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

float vertices[] = {
    // Triangle 0  ↑
    -0.950f, -0.200f, 0.0f, -0.570f, -0.200f, 0.0f, -0.760f, 0.200f, 0.0f,

    // Triangle 1  ↓   (0.005 gap from previous)
    -0.565f, 0.200f, 0.0f, -0.185f, 0.200f, 0.0f, -0.375f, -0.200f, 0.0f,

    // Triangle 2  ↑
    -0.180f, -0.200f, 0.0f, 0.200f, -0.200f, 0.0f, 0.010f, 0.200f, 0.0f,

    // Triangle 3  ↓
    0.205f, 0.200f, 0.0f, 0.585f, 0.200f, 0.0f, 0.395f, -0.200f, 0.0f,

    // Triangle 4  ↑
    0.590f, -0.200f, 0.0f, 0.970f, -0.200f, 0.0f, 0.780f, 0.200f, 0.0f};
unsigned int indices[] = {
    0, 1, 2,  2,  1, 3, // quad 1
    2, 3, 4,  4,  3, 5, // quad 2
    4, 5, 6,  6,  5, 7, // quad 3
    6, 7, 8,  8,  7, 9, // quad 4
    8, 9, 10, 10, 9, 11 // quad 5
};
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
  Shader ourShader("shader.vert", "shader.frag");
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // 1. Create and bind VAO first — it must store attribute state
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // 2. Create and bind VBO
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // 3. Create and bind EBO
  // glGenBuffers(1, &EBO);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //              GL_STATIC_DRAW);

  // 4. Set vertex attribute layout (stored in VAO)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
  //                       (void *)(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);

  // 5. VAO now fully configured
  glBindVertexArray(0); // optional unbind for safety

  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    // rendering commands

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ourShader.use();
    // after the shader program in started we set the

    glBindVertexArray(VAO); // binds VBO + EBO + attribs as stored
    glDrawArrays(GL_TRIANGLES, 0, 15);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0;
}
