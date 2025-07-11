#define STB_IMAGE_IMPLEMENTATION
#include <glm/ext/matrix_transform.hpp>
#include <vector>

#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
const float toRadians = 3.14159265f / 180.0f;
float curAngle = 0.0f;
Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;

GLfloat deltaTime{0.0f};
GLfloat lastTime{0.0f};
static const char *vShader = "Shaders/shader.vert";
static const char *fShader = "Shaders/shader.frag";
void CreateObjects() {
  unsigned int indices[] = {0, 3, 1, 1, 3, 2, 2, 3, 0, 0, 1, 2};
  GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
                        1.0f,  0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
                        0.0f,  0.0f,  1.0f, 0.0f, 0.5f,  1.0f};
  GLfloat cubeVertices[] = {-1, -1, 1,  0, 0, 1,  -1, 1,  0.5f, 1.0f,
                            1,  1,  1,  1, 0, -1, 1,  1,  0.5f, 1,
                            -1, -1, -1, 0, 0, 1,  -1, -1, 0.5f, 1,
                            1,  1,  -1, 1, 0, -1, 1,  -1, 0.5f, 1};
  unsigned int cubeIndices[] = {0, 1, 2, 0, 2, 3, 5, 4, 7, 5, 7, 6,
                                4, 0, 3, 4, 3, 7, 1, 5, 6, 1, 6, 2,
                                3, 2, 6, 3, 6, 7, 4, 5, 1, 4, 1, 0};
  Mesh *obj1 = new Mesh();
  obj1->CreateMesh(vertices, indices, 20, 12);
  meshList.push_back(obj1);
  Mesh *cube = new Mesh();
  cube->CreateMesh(cubeVertices, cubeIndices, 40, 36);
  meshList.push_back(cube);
}
void CreateShaders() {
  Shader *shader1 = new Shader();
  shader1->CreateFromFiles(vShader, fShader);
  shaderList.push_back(*shader1);
}

int main() {
  mainWindow.initialize();
  CreateObjects();
  CreateShaders();

  camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  -90.0f, 0.0f, 5.0f, 0.5f);
  brickTexture = Texture("Textures/brick.png");
  brickTexture.LoadTexture();
  dirtTexture = Texture("Textures/dirt.png");
  brickTexture.LoadTexture();
  GLuint uniformModel{0}, uniformProjection{0}, uniformView{0};
  glm::mat4 projection =
      glm::perspective(45.0f,
                       (GLfloat)mainWindow.getBufferWidth() /
                           (GLfloat)mainWindow.getBufferHeight(),
                       0.1f, 100.0f);
  while (!mainWindow.getShouldClose()) {
    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    lastTime = now;
    glfwPollEvents();
    camera.keyControl(mainWindow.getsKeys(), deltaTime);
    camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderList[0].UseShader();
    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    curAngle += 1.0f;
    if (curAngle >= 360) {
      curAngle -= 360;
    }
    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(-0.8f, 0.0f, -2.5f));
    model =
        glm::rotate(model, curAngle * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
                       glm::value_ptr(projection));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE,
                       glm::value_ptr(camera.calculateViewMatrix()));
    brickTexture.UseTexture();
    meshList[0]->RenderMesh();

    model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(1.5f, 0.0f, -5.0f));
    model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
    model = glm::rotate(model, 2 * curAngle * toRadians, glm::vec3(1, -1, -1));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
                       glm::value_ptr(projection));
    meshList[1]->RenderMesh();
    glUseProgram(0);
    mainWindow.swapBuffers();
  }
  return 0;
}
