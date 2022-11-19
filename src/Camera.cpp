#include "Camera.h"
#include <glm/gtx/string_cast.hpp>

#define FULL_CIRCLE (2 * glm::pi<float>())

Camera::Camera()
{
	recalculateEulerAngles();
}

void Camera::recalculateEulerAngles()
{
	wDirection = glm::normalize(center - position);
	center = position + wDirection;
	uDirection = glm::cross(wDirection, lookUp);
	vDirection = glm::cross(uDirection, wDirection);

	pitch = glm::asin(glm::dot(glm::vec3(0.f, 1.f, 0.f), wDirection));
	yaw = glm::atan(glm::dot(glm::vec3(0.f, 0.f, -1.f), wDirection) / glm::dot(glm::vec3(1.f, 0.f, 0.f), wDirection));

	if (wDirection.z < 0.f) 
	{
		yaw = FULL_CIRCLE - yaw;
	}

	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

void Camera::setPosition(const glm::vec3& position)
{
	this->position = position;
	center = position + wDirection;
	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

void Camera::setCenter(const glm::vec3& center)
{
	this->center = center;
	recalculateEulerAngles();
}

PerspectiveCamera::PerspectiveCamera()
{
	project();
}

void PerspectiveCamera::setProjectionViewingAngle(float angle)
{
	viewingAngle = angle;
	project();
}

void PerspectiveCamera::setProjectionAspectRatio(float ratio)
{
	aspectRatio = ratio;
	project();
}

void PerspectiveCamera::project()
{
	projection = glm::perspective(glm::radians(viewingAngle), aspectRatio, nearPlane, farPlane);
	projectionChanged = true;
}