#include "camera.hpp"

#include <fmt/core.h>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

void Camera::computeProjectionMatrix(int width, int height) {
  m_projMatrix = glm::mat4(1.0f);
  auto aspect{static_cast<float>(width) / static_cast<float>(height)};
  m_projMatrix = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 20.0f);
}

void Camera::computeViewMatrix() {
  if(m_eye.z<-1.15f)
    m_eye.z = -1.15f;
  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}

void Camera::dolly(float speed) {
  speed *= m_dollySpeed;
  // Compute forward vector (view direction)
  glm::vec3 forward = glm::normalize(m_at - m_eye);

  // Move eye and center forward (speed > 0) or backward (speed < 0)
  m_eye += forward * speed;
  m_at += forward * speed;

  computeViewMatrix();
}

void Camera::truck(float speed) {
  speed *= m_truckSpeed;
  // Compute forward vector (view direction)
  glm::vec3 forward = glm::normalize(m_at - m_eye);
  // Compute vector to the left
  glm::vec3 left = glm::cross(m_up, forward);

  // Move eye and center to the left (speed < 0) or to the right (speed > 0)
  m_at -= left * speed;
  m_eye -= left * speed;

  computeViewMatrix();
}

void Camera::pan(float speed) {
  speed *= m_panSpeed;

  glm::mat4 transform{glm::mat4(1.0f)};

  // Rotate camera around its local y axis
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, -speed, m_up);
  transform = glm::translate(transform, -m_eye);

  m_at = transform * glm::vec4(m_at, 1.0f);

  computeViewMatrix();
}

glm::vec3 Camera::getPosition(){
  return m_eye;
}

glm::vec3 Camera::getM_at(){
  return m_at;
}

float Camera::getAng(){
  glm::vec3 vetor = m_eye - m_at;

  float ang = atan(vetor.x/vetor.z);

  // fmt::print(stdout, "Ang---> {} Vec---> x:{} z:{}\r", ang, vetor.x, vetor.z);

  return -(ang+3.0f*M_PI_2+M_PI/4);
}

float Camera::getDollySpeed(){
  return m_dollySpeed;
}
float Camera::getPanSpeed(){
  return m_panSpeed;
}
float Camera::getTruckSpeed(){
  return m_truckSpeed;
}

void Camera::setDollySpeed(float n){
  m_dollySpeed = n;
}
void Camera::setPanSpeed(float n){
  m_panSpeed = n;
}
void Camera::setTruckSpeed(float n){
  m_truckSpeed = n;
}

glm::mat4 Camera::getViewMatrix(){
  return m_viewMatrix;
}
glm::mat4 Camera::getProjMatrix(){
  return m_projMatrix;
}