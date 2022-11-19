#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#define FULL_CIRCLE (2 * glm::pi<float>())

class PerspectiveCamera {
public:
    PerspectiveCamera() {
        recalculateEulerAngles();
        project();
    }
    ~PerspectiveCamera() = default;
    [[nodiscard]] const glm::vec3& getPosition() const{
        return position;
    }
    [[nodiscard]] const glm::mat4& getView() const {
        return view;
    }
    [[nodiscard]] const glm::mat4& getProjection() const {
        return projection;
    }

    void setPosition(const glm::vec3& position) {
        this->position = position;
        center = position + wDirection;
        view = glm::lookAt(position, center, lookUp);
    }
    void setCenter(const glm::vec3& center) {
        this->center = center;
        recalculateEulerAngles();
    }
    void setProjectionViewingAngle(float angle) {
        viewingAngle = angle;
        project();
    }
    void setProjectionAspectRatio(float ratio) {
        aspectRatio = ratio;
        project();
    }

protected:
    void recalculateEulerAngles() {
        wDirection = glm::normalize(center - position);
        center = position + wDirection;
        uDirection = glm::cross(wDirection, lookUp);
        vDirection = glm::cross(uDirection, wDirection);

        yaw = glm::atan(glm::dot(glm::vec3(0.f, 0.f, -1.f), wDirection) / glm::dot(glm::vec3(1.f, 0.f, 0.f), wDirection));

        if (wDirection.z < 0.f)
        {
            yaw = FULL_CIRCLE - yaw;
        }

        view = glm::lookAt(position, center, lookUp);
    }

    void project() {
        projection = glm::perspective(glm::radians(viewingAngle), aspectRatio, nearPlane, farPlane);
    }

    glm::vec3 position{ 0.f, 10.f, 15.f };
    glm::vec3 center{ 0.f, 0.f, 0.f };
    glm::vec3 lookUp{ 0.f, 1.f, 0.f };
    glm::vec3 wDirection;
    glm::vec3 uDirection;
    glm::vec3 vDirection;
    glm::mat4 view;
    glm::mat4 projection;
    float yaw = 3 * glm::pi<float>() / 2;
    float nearPlane = 1.f;
    float farPlane = 100.f;

    float aspectRatio = 1024 / 768.f;
    float viewingAngle = 45.f;
};