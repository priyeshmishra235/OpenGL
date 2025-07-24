#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera {
public:
  glm::vec3 Position, Front, Up, Right, WorldUp;
  float Yaw, Pitch, MovementSpeed, MouseSensitivity;

  Camera(glm::vec3 position);
  glm::mat4 GetViewMatrix();
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);
  void ProcessMouseMovement(float xoffset, float yoffset);
  void UpdateCameraVectors();
};
#endif
