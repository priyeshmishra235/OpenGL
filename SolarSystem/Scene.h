#include "Planet.h"
#include <vector>

class Scene {
public:
  std::vector<Planet> planets;
  Scene();
  void Update(float time);
  void Render(Shader &shader, unsigned int VAO);
};
