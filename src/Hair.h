#pragma once
#include "Entity.h"
#include "ComputeShader.h"
#include <vector>
#include <glm/gtc/random.hpp>

class Hair : public Entity {
public:
	Hair(uint32_t _strandCount = 5000U, uint32_t strandSegmentsCount = 10U)
	: strandCount(_strandCount), computeShader("HairComputeShader.glsl")
	{
		strandSegmentsCount = glm::clamp<uint32_t>(strandSegmentsCount, 2U, 49U);
		particlesPerStrand = strandSegmentsCount + 1;
		computeShader.use();
		computeShader.setUint("hairData.strandCount", strandCount);
		computeShader.setUint("hairData.particlesPerStrand", particlesPerStrand);
		computeShader.setFloat("hairData.particleMass", 0.1f);
		computeShader.setFloat("hairData.segmentLength", 4.f / strandSegmentsCount);
		computeShader.setFloat("force.gravity", gravity);
		computeShader.setVec4("force.wind", wind);
		computeShader.setFloat("frictionCoefficient", frictionFactor);
		computeShader.setFloat("headRadius", 1.f);
		constructModel();
	}
	~Hair() {
		glDeleteBuffers(1, &velocityArrayBuffer);
		glDeleteBuffers(1, &volumeDensities);
		glDeleteBuffers(1, &volumeVelocities);
	}
	void draw() const override {
		glBindVertexArray(vao);
		glMultiDrawArrays(GL_LINE_STRIP, firsts.data(), lasts.data(), strandCount);
		glBindVertexArray(GL_NONE);
	}
	void applyPhysics(float deltaTime, float runningTime);

	/*
	* Sets wind direction and strength. 
	* If direction set to { 0.f, 0.f, 0.f } wind is dynamic with specified strength
	* Wind strength is clamped in range [0, 1]
	*/
	
	/*
	* Sets friction factor clamped in range [0, 1] 
	*/

private:
	GLuint velocityArrayBuffer = GL_NONE;		// Shader storage buffer object for velocities
	GLuint volumeDensities = GL_NONE;
	GLuint volumeVelocities = GL_NONE;

	std::vector<GLint> firsts;
	std::vector<GLint> lasts;
	uint32_t strandCount;
	ComputeShader computeShader;
	GLuint particlesPerStrand = 0;
	glm::vec4 wind{ 0.f, 0.f, 0.f, 0.2f };
	float gravity = -9.81f;
	const uint32_t maximumStrandCount = 30000;
	float frictionFactor = 0.03f;
	void constructModel();
};

inline void Hair::constructModel()
{
	const float segmentLength = 4.f / (particlesPerStrand - 1);
	std::vector<float> data;
	data.reserve(maximumStrandCount * particlesPerStrand * 3);

	for (uint32_t i = 0; i < maximumStrandCount; ++i)
	{
		glm::vec3 randCoords = glm::sphericalRand(1.0f);
		while (randCoords.z > 0.1f || randCoords.y < -0.7f)
			randCoords = glm::sphericalRand(1.0f);

		for (uint32_t j = 0; j < particlesPerStrand; ++j)
		{
			glm::vec3 temp = randCoords;
			data.push_back(temp.x);
			data.push_back(temp.y);
			data.push_back(temp.z - j * segmentLength);
		}
	}

	firsts.reserve(maximumStrandCount);
	lasts.reserve(maximumStrandCount);
	for (uint32_t i = 0; i < maximumStrandCount - 1; ++i)
	{
		firsts.push_back(particlesPerStrand * i);
		lasts.push_back(particlesPerStrand);
	}

        // Bind Vertex Array Object
	glBindVertexArray(vao);
        // Bind Array Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Pass data to Array Buffer
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
        // Tell opengl how to use vertex data
        // loation, vertex_attr_size, data_type, if_normalize,
        // let opengl decide stride size, offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // with vertex_attr location as argument to enable vertex_attr
	glEnableVertexAttribArray(0);

        // Unbind Array Buffer to avoid unexpect modify
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        // Unbind Vertex Array to avoid unexpect modify
	glBindVertexArray(GL_NONE);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);

	// Velocities
        data.resize(0);
        data.resize(maximumStrandCount * particlesPerStrand, 0);
	/* data.reserve(maximumStrandCount * particlesPerStrand * 3); */
	/* for (uint32_t i = 0; i < maximumStrandCount * particlesPerStrand * 3; ++i) */
	/* 	data.push_back(0.f); */

	glGenBuffers(1, &velocityArrayBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityArrayBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityArrayBuffer);

	GLsizeiptr voxelGridSize = 11 * 11 * 11 * sizeof(float); // 10x10x10 voxels, 11 vertices per dimension
	glGenBuffers(1, &volumeDensities);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeDensities);
	glBufferData(GL_SHADER_STORAGE_BUFFER, voxelGridSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, volumeDensities);

	voxelGridSize *= 3;	// 3-component vectors
	glGenBuffers(1, &volumeVelocities);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeVelocities);
	glBufferData(GL_SHADER_STORAGE_BUFFER, voxelGridSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, volumeVelocities);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL_NONE);
}

inline void Hair::applyPhysics(float deltaTime, float runningTime)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeDensities);
	int* densities = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	uint32_t volumeSize = 11 * 11 * 11;
	for (uint32_t i = 0; i < volumeSize; ++i)
	{
		densities[i] = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, volumeVelocities);
	int* velocities = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

	for (uint32_t i = 0; i < volumeSize * 3; i++)
	{
		velocities[i] = 0;	
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL_NONE);

	computeShader.use();
	computeShader.setUint("hairData.strandCount", strandCount);
	computeShader.setFloat("deltaTime", deltaTime);
	computeShader.setFloat("runningTime", runningTime);
	computeShader.setUint("state", 0);
	GLuint localWorkGroupCountX = computeShader.getLocalWorkGroupsCount().x;
	GLuint globalWorkGroupCount = strandCount / localWorkGroupCountX;
	if (strandCount % localWorkGroupCountX != 0)
	{
		globalWorkGroupCount += 1;
	}

	computeShader.setGlobalWorkGroupCount(globalWorkGroupCount);
	computeShader.dispatch();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	globalWorkGroupCount = strandCount * particlesPerStrand / localWorkGroupCountX;
	if ((strandCount * particlesPerStrand) % localWorkGroupCountX != 0)
	{
		globalWorkGroupCount += 1;
	}
	computeShader.setGlobalWorkGroupCount(globalWorkGroupCount);
	computeShader.setUint("state", 1);
	computeShader.dispatch();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	computeShader.setUint("state", 2);
	computeShader.dispatch();
}
