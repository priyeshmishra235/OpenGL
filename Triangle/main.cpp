#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"

// Constants
const float toRadians = 3.14159265f / 180.0f;
float curAngle = 0.0f;
GLfloat deltaTime{0.0f};
GLfloat lastTime{0.0f};

// scene object creation
Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader> shaderList;
Camera camera;
Texture brickTexture;
Texture dirtTexture;
Light mainLight;

// shader location define
static const char *vShader = "Shaders/shader.vert";
static const char *fShader = "Shaders/shader.frag";

void CreateObjects() {
  // triangle
  unsigned int triIndices[] = {0, 3, 1, 1, 3, 2, 2, 3, 0, 0, 1, 2};
  GLfloat triVertices[] = {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                           1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                           1.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                           -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
  GLfloat cubeVertices[] = {
      -1, -1, 1,  0, 0, 0, 0, 1,  -1, 1,  1,  0, 1, 0, 0, 1,
      1,  1,  1,  1, 1, 0, 0, 1,  1,  -1, 1,  1, 0, 0, 0, 1,
      -1, -1, -1, 1, 0, 0, 0, -1, -1, 1,  -1, 1, 1, 0, 0, -1,
      1,  1,  -1, 0, 1, 0, 0, -1, 1,  -1, -1, 0, 0, 0, 0, -1,
  };
  unsigned int cubeIndices[] = {0, 1, 2, 0, 2, 3, 5, 4, 7, 5, 7, 6,
                                4, 0, 3, 4, 3, 7, 1, 5, 6, 1, 6, 2,
                                3, 2, 6, 3, 6, 7, 4, 5, 1, 4, 1, 0};
  // assign vertices and indices
  // for triangle
  Mesh *obj1 = new Mesh();
  obj1->CreateMesh(triVertices, triIndices, 20, 12);
  meshList.push_back(obj1);
  // for cube
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
  // initialization
  mainWindow.initialize();
  CreateObjects();
  CreateShaders();

  // camera initialization
  camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  -90.0f, 0.0f, 5.0f, 0.5f);

  // texture
  brickTexture = Texture("Textures/brick.png");
  brickTexture.LoadTexture();
  dirtTexture = Texture("Textures/dirt.png");
  dirtTexture.LoadTexture();

  // Light
  mainLight = Light(1.0f, 1.0f, 1.0f,   // RGB
                    0.3f,               // ambientIntensity
                    0.0f, -1.0f, -1.0f, // direction
                    0.8f);              // diffuseIntensity

  GLuint uniformModel{0}, uniformProjection{0}, uniformView{0},
      uniformAmbientIntensity{0}, uniformAmbientColor{0};

  // get perspective right
  glm::mat4 projection =
      glm::perspective(45.0f,
                       (GLfloat)mainWindow.getBufferWidth() /
                           (GLfloat)mainWindow.getBufferHeight(),
                       0.1f, 100.0f);

  // run till window is not closed
  while (!mainWindow.getShouldClose()) {
    // delta time
    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    lastTime = now;

    glfwPollEvents();

    // keyboard and mouse
    camera.keyControl(mainWindow.getsKeys(), deltaTime);
    camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

    // screen skybox color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // apply shaders
    shaderList[0].UseShader();
    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    uniformAmbientColor = shaderList[0].GetAmbientColorLocation();
    uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
    uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();
    uniformDirection = shaderList[0].GetDirectionLocation();

    mainLight.UseLight(uniformAmbientIntensity, // float
                       uniformAmbientColor,     // vec3
                       uniformDiffuseIntensity, // float
                       uniformDirection);       // vec3

    // for rotation
    curAngle += 1.0f;
    if (curAngle >= 360) {
      curAngle -= 360;
    }

    // triangle render
    // this will initialize model to identity matirx
    glm::mat4 model = glm::identity<glm::mat4>();
    // now scale, rotate then translate
    model = glm::translate(model, glm::vec3(-0.8f, 0.0f, -2.5f));
    // model =
    // glm::rotate(model, curAngle * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    // assign those changes to model matrix
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    // apply projection to it
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
                       glm::value_ptr(projection));
    // and camera
    glUniformMatrix4fv(uniformView, 1, GL_FALSE,
                       glm::value_ptr(camera.calculateViewMatrix()));
    // then apply the texture
    brickTexture.UseTexture();
    // now ready for rendering
    meshList[0]->RenderMesh();
    // cube render
    model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(1.5f, 0.0f, -5.0f));
    model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
    // model = glm::rotate(model, 2 * curAngle * toRadians, glm::vec3(1, -1,
    // -1));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
                       glm::value_ptr(projection));
    dirtTexture.UseTexture();
    meshList[1]->RenderMesh();
    // stop the program and redo the while
    glUseProgram(0);
    // swap with the buffer window
    mainWindow.swapBuffers();
  }
  return 0;
}
