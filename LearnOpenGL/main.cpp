#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include "Shader.h"
#include "glad.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// ─── globals ───────────────────────────────────────────────────────────────
int width{800}, height{600};
GLuint VBO{}, VAO{}, EBO{}, fragmentShader{}, vertexShader{}, shaderProgram{};
GLuint textures[2];
int iwidth, iheight, nrChannels;

float mixFactor = 0.0f;
unsigned int texture;
// ----Helpers ---------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    mixFactor += 0.01f;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    mixFactor -= 0.01f;
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
    // positions vec3    // colors vec3   // texture coords vec2
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
};
unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle

};
void cleanUp() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);
  glfwTerminate();
}
void textureData() {
  unsigned char *data =
      stbi_load("container.jpg", &iwidth, &iheight, &nrChannels, 0);
  if (!data) {
    std::cout << "Failed to find container.png " << std::endl;
    exit(1);
  }
  unsigned char *data2 =
      stbi_load("awesomeface.png", &iwidth, &iheight, &nrChannels, 0);

  if (!data2) {
    std::cout << "Failed to find smily.png " << std::endl;
    exit(1);
  }

  glGenTextures(2, textures);
  // load and genglBindTexture(GL_TEXTURE_2D, textures[0]);

  // set the sampler behaviour once
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_set_flip_vertically_on_load(true);
  // image 1
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  int w0, h0, nc0;
  unsigned char *img0 = stbi_load("container.jpg", &w0, &h0, &nc0, 0);
  GLenum fmt0 = (nc0 == 4) ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, fmt0, w0, h0, 0, fmt0, GL_UNSIGNED_BYTE, img0);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(img0);
  // image 2
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  int w1, h1, nc1;
  unsigned char *img1 = stbi_load("awesomeface.png", &w1, &h1, &nc1, 0);
  GLenum fmt1 = (nc1 == 4) ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, fmt1, w1, h1, 0, fmt1, GL_UNSIGNED_BYTE, img1);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(img1);
}

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
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // 4. Set vertex attribute layout (stored in VAO)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  textureData();

  ourShader.use();
  glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // unit 0
  glUniform1i(glGetUniformLocation(ourShader.ID, "texture2"), 1); // unit 1
  //
  //
  int mixFactorID = glGetUniformLocation(ourShader.ID, "mixFactor");
  // 5. VAO now fully configured
  glBindVertexArray(0); // optional unbind for safety

  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    // rendering commands

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform1f(mixFactorID, mixFactor);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    ourShader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  cleanUp();
  return 0;
}
