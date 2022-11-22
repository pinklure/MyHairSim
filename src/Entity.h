#pragma once
#include "Shader.h"
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Entity {
public:
	Entity(): rotationQuat(glm::angleAxis(0.f, glm::vec3(1.f, 0.f, 0.f))) {
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);
    }
	virtual ~Entity() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
	virtual void draw() const = 0;
	void rotate(float angle, const glm::vec3& axis) {
        rotationQuat = glm::rotate(rotationQuat, glm::radians(angle), glm::normalize(axis));
        transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * glm::mat4_cast(rotationQuat), scaleVector);
    }
	void scale(const glm::vec3& scale) {
        scaleVector = scale;
        transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * glm::mat4_cast(rotationQuat), scaleVector);
    }
	void translate(const glm::vec3& translation) {
        translationVector = translation;
        transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * glm::mat4_cast(rotationQuat), scaleVector);
    }
	const glm::mat4& getTransformMatrix() const {
        return transformMatrix;
    }

	glm::vec3 color{ 1.f };

protected:
	glm::mat4 transformMatrix{ 1.f };
	glm::quat rotationQuat;
	glm::vec3 translationVector{ 0.f };
	glm::vec3 scaleVector{ 1.f };
	GLuint vbo = 0;
	GLuint vao = 0;
	GLuint vertexCount = 0;
};

