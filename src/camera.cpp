#include "camera.h"

// constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : position(position), up(up), yaw(yaw), pitch(pitch)
{
    position = position;
    world_up = up;
    yaw = yaw;
    pitch = pitch;
    update_camera_vectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::get_view_matrix()
{
    return glm::lookAt(position, position + front, up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::process_keyboard(Camera_Movement direction, float delta_time)
{
    float velocity = movement_speed * delta_time;
    if (direction == FORWARD)
    {
        position += velocity * front;
    }

    if (direction == BACKWARD)
    {
        position -= velocity * front;
    }

    if (direction == LEFT)
    {
        position -= velocity * right;
    }

    if (direction == RIGHT)
    {
        position += velocity * right;
    }
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch)
{
    x_offset *= mouse_sensitivity;
    y_offset *= mouse_sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch)
    {
        if (pitch > 89.0f)
        {
            pitch = 89.0f;
        }
        if (pitch < -89.0f)
        {
            pitch = -89.0f;
        }
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::process_mouse_scroll(float y_offset)
{
    zoom -= y_offset;
    if (zoom < 1.0f)
    {
        zoom = 1.0f;
    }
    if (zoom > 45.0f)
    {
        zoom = 45.0f;
    }
}

void Camera::update_camera_vectors()
{
    // calculate the new Front vector
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, world_up)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}
