#pragma once
//#include "Entity.h"
#include "ComputeShader.h"
#include "Sphere.h"
#include <memory>
#include <vector>
#include <array>
//#include "Window.h"

class Hair : public Entity {
public:
	Hair(uint32_t _strandCount = 5000U, float hairLength = 3.f, float hairCurliness = 0.0f);
	~Hair();
	void draw() const override;
	void drawHead() const;
	void applyPhysics(float deltaTime, float runningTime);
	float getCurlRadius() const { return curlRadius; }
	uint32_t getParticlesPerStrand() const { return particlesPerStrand; }

private:
	GLuint velocityArrayBuffer = GL_NONE;		// Shader storage buffer object for velocities
	GLuint volumeDensities = GL_NONE;
	GLuint volumeVelocities = GL_NONE;

	uint32_t strandCount;
	float curlRadius = 0.0f;
	ComputeShader computeShader;
	uint32_t particlesPerStrand = 15;
	glm::vec4 wind{ 0.f, 0.f, 0.f, 0.2f };
	float gravity = -9.81f;
	bool settingsChanged = false;
	const uint32_t maximumStrandCount = 30000U;
	float frictionFactor = 0.02f;
	void constructModel();
	float strandWidth = 0.2f;
	float hairLength = 1.f;
	float velocityDampingCoefficient = 0.9f;

	// Head variables
	glm::vec3 headColor;
	GLuint headVbo = GL_NONE;
	GLuint headVao = GL_NONE;
	GLuint headEbo = GL_NONE;
	uint32_t indexCount = 0;
	std::array<std::unique_ptr<Sphere>, 7> ellipsoids;
	float ellipsoidsRadius = 0.5f;
};