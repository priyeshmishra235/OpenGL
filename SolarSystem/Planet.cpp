#include "Planet.h"
#include "glad/glad.h"
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>

Planet::Planet(float orbitRadius, float orbitSpeed, float size, glm::vec3 color)
    : orbitRadius(orbitRadius), orbitSpeed(orbitSpeed), size(size),
      color(color) {}

void Planet::Update(float time) {
  float angle = orbitSpeed * time;
  position =
      glm::vec3(orbitRadius * cos(angle), 0.0f, orbitRadius * sin(angle));
}

void Planet::Draw(Shader &shader, unsigned int VAO) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
  model = glm::scale(model, glm::vec3(size));
  shader.setMat4("model", model);
  shader.setVec3("color", color);
  shader.setVec3("objectColor", color);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, (36 * 18 * 6), GL_UNSIGNED_INT, 0);
}
