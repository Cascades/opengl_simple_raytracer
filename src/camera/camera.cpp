#include "GL/glew.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "OpenGLRayTrace/camera.h"

OGLRT::Camera::Camera() :
    projection_matrix(glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10000.0f)),
    pos(glm::vec3(0.0f, 0.0f, 0.0f)),
    world_up(glm::vec3(0.0f, 1.0f, 0.0f)),
    dir(glm::vec3(0.0f, 0.0f, -1.0f)),
    up(glm::vec3(0.0f, 1.0f, 0.0f)),
    right(glm::normalize(glm::cross(dir, world_up))),
    yaw(-90.0f),
    pitch(0.0f),
    movenent_speed(200.0f),
    mouse_sensitivity(0.1f),
    zoom(45.0f)
{
    updateCameraVectors();
}

OGLRT::Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, int const& width, int const& height) :
    projection_matrix(glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 1.0f, 10000.0f)),
    pos(position),
    world_up(glm::vec3(0.0f, 1.0f, 0.0f)),
    dir(glm::vec3(0.0f, 0.0f, -1.0f)),
    up(up),
    right(glm::normalize(glm::cross(dir, world_up))),
    yaw(yaw),
    pitch(pitch),
    movenent_speed(200.0f),
    mouse_sensitivity(0.1f),
    zoom(45.0f)
{
    updateCameraVectors();
}

void OGLRT::Camera::set_projection_matrix(float const& fov, float const& width, float const& height, float const& near_z, float const& far_z)
{
    projection_matrix = glm::perspective(glm::radians(fov), width / height, near_z, far_z);
}

glm::mat4 OGLRT::Camera::GetViewMatrix()
{
    return glm::lookAt(pos, pos + dir, up);
}

void OGLRT::Camera::ProcessKeyboard(OGLRT::CameraMovement direction, float deltaTime)
{
    float velocity = movenent_speed * deltaTime;
    if (direction == OGLRT::CameraMovement::FORWARD)
        pos += dir * velocity;
    if (direction == OGLRT::CameraMovement::BACKWARD)
        pos -= dir * velocity;
    if (direction == OGLRT::CameraMovement::LEFT)
        pos -= right * velocity;
    if (direction == OGLRT::CameraMovement::RIGHT)
        pos += right * velocity;
}

void OGLRT::Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void OGLRT::Camera::ProcessMouseScroll(float yoffset)
{
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void OGLRT::Camera::updateCameraVectors()
{
    glm::vec3 new_front;
    new_front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    new_front.y = glm::sin(glm::radians(pitch));
    new_front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    dir = glm::normalize(new_front);
    right = glm::normalize(glm::cross(dir, world_up));
    up = glm::normalize(glm::cross(right, dir));
}