#ifndef PLANET_H
#define PLANET_H

#include "Shader.h"
#include <glm/glm.hpp>

class Planet {
public:
  float orbitRadius, orbitSpeed, size;
  glm::vec3 color;
  glm::vec3 position;

  Planet(float orbitRadius, float orbitSpeed, float size, glm::vec3 color);
  void Update(float time);
  void Draw(Shader &shader, unsigned int VAO);
};
#endif
