#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget; // Not strictly used for FPS camera, we use Front
        this->cameraUpDirection = cameraUp;

        // Calculate initial Front vector
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        // Returns the view matrix using Position and the Direction we are looking
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        if (direction == MOVE_FORWARD)
            cameraPosition += cameraFrontDirection * speed;
        if (direction == MOVE_BACKWARD)
            cameraPosition -= cameraFrontDirection * speed;
        if (direction == MOVE_LEFT)
            cameraPosition -= cameraRightDirection * speed;
        if (direction == MOVE_RIGHT)
            cameraPosition += cameraRightDirection * speed;
    }


    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFrontDirection = glm::normalize(front);

        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }


    void Camera::setCameraPosition(glm::vec3 newPos) {
        this->cameraPosition = newPos;
    }


    void Camera::setCameraTarget(glm::vec3 target) {
        this->cameraTarget = target;
     
        this->cameraFrontDirection = glm::normalize(target - this->cameraPosition);
      
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        this->cameraUpDirection = glm::normalize(glm::cross(this->cameraRightDirection, this->cameraFrontDirection));
    }
}