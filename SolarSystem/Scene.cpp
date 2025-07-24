#include "Scene.h"

Scene::Scene() {
  // Sun
  planets.emplace_back(0.0f, 0.0f, 2.5f, glm::vec3(1.0f, 1.0f, 0.0f));
  // Mercury
  planets.emplace_back(3.5f, 1.6f, 0.2f, glm::vec3(0.5f, 0.5f, 0.5f));
  // Venus
  planets.emplace_back(5.0f, 1.2f, 0.4f, glm::vec3(1.0f, 0.8f, 0.4f));
  // Earth
  planets.emplace_back(7.0f, 1.0f, 0.5f, glm::vec3(0.2f, 0.5f, 1.0f));
  // Mars
  planets.emplace_back(9.5f, 0.8f, 0.4f, glm::vec3(1.0f, 0.3f, 0.3f));
  // Jupiter
  planets.emplace_back(13.0f, 0.4f, 1.2f, glm::vec3(0.9f, 0.7f, 0.5f));
  // Saturn
  planets.emplace_back(17.0f, 0.3f, 1.0f, glm::vec3(0.9f, 0.8f, 0.6f));
  // Uranus
  planets.emplace_back(21.0f, 0.25f, 0.8f, glm::vec3(0.5f, 1.0f, 1.0f));
  // Neptune
  planets.emplace_back(25.0f, 0.2f, 0.7f, glm::vec3(0.3f, 0.5f, 1.0f));
}

void Scene::Update(float time) {
  for (auto &p : planets)
    p.Update(time);
}

void Scene::Render(Shader &shader, unsigned int VAO) {
  for (auto &p : planets)
    p.Draw(shader, VAO);
}
