#include "Camera.h"

Camera::Camera(glm::vec3 position)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(15.5f),
      MouseSensitivity(0.1f), Yaw(-90.0f), Pitch(0.0f), Position(position),
      WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)) {
  UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
  return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement dir, float deltaTime) {
  float velocity = MovementSpeed * deltaTime;
  if (dir == FORWARD)
    Position += Front * velocity;
  if (dir == BACKWARD)
    Position -= Front * velocity;
  if (dir == LEFT)
    Position -= Right * velocity;
  if (dir == RIGHT)
    Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;
  Yaw += xoffset;
  Pitch += yoffset;
  Pitch = glm::clamp(Pitch, -89.0f, 89.0f);
  UpdateCameraVectors();
}

void Camera::UpdateCameraVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  front.y = sin(glm::radians(Pitch));
  front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  Front = glm::normalize(front);
  Right = glm::normalize(glm::cross(Front, WorldUp));
  Up = glm::normalize(glm::cross(Right, Front));
}
