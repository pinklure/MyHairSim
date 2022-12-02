#pragma once
#include "Entity.h"
#include "ComputeShader.h"
#include <vector>
#include <glm/gtc/random.hpp>
#include <iostream>
#include <functional>

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
		computeShader.setFloat("gravity", gravity);
		computeShader.setVec4("wind", wind);
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
                for(auto i = 0ul; i<strandCount; i++) {
                    glDrawArrays(GL_LINE_STRIP, firsts[i], particlesPerStrand);
                }
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
	uint32_t strandCount;
	ComputeShader computeShader;
	GLuint particlesPerStrand = 0;
	glm::vec4 wind{ 0.f, 0.f, 0.f, 0.2f };
	float gravity = -9.81f;
	const uint32_t maximumStrandCount = 30000;
	float frictionFactor = 0.03f;
	void constructModel();
};

inline void Hair::constructModel(){
	const float segmentLength = 4.f / (particlesPerStrand - 1);
	std::vector<float> data;

	for (uint32_t i = 0; i < maximumStrandCount; ++i){
            glm::vec3 randCoords;
            do {
                randCoords = glm::sphericalRand(1.f);
            } while(randCoords.z > 0.1f || randCoords.y < -0.7f);

            for (uint32_t j = 0; j < particlesPerStrand; ++j){
                    glm::vec3 temp = randCoords;
                    data.push_back(temp.x);
                    data.push_back(temp.y);
                    data.push_back(temp.z - j * segmentLength);
            }
	}

	for (uint32_t i = 0; i < maximumStrandCount - 1; ++i){
		firsts.push_back(particlesPerStrand * i);
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
        data.resize(maximumStrandCount * particlesPerStrand * 3, 0);

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


static void ZeroInitStorageBuffer(GLuint buffer_id, uint32_t size) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
    auto ptr = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    for(auto i = 0ul; i < size; i++, ptr[i] = 0);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, GL_NONE);
}

GLuint ceil_div(uint32_t a, uint32_t b) {
    auto res = a / b;
    if(a % b != 0) {
        res += 1;
    }
    return res;
}



static void dispatch_compute(ComputeShader& compute_shader, uint32_t global_work_group_count, uint8_t state) {
    compute_shader.setUint("state", state);
    compute_shader.setGlobalWorkGroupCount(global_work_group_count);
    compute_shader.dispatch();
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


inline void Hair::applyPhysics(float deltaTime, float runningTime)
{
	uint32_t volumeSize = 11 * 11 * 11;

        ZeroInitStorageBuffer(volumeDensities, volumeSize);
        ZeroInitStorageBuffer(volumeVelocities, volumeSize * 3);

	computeShader.use();
	computeShader.setUint("hairData.strandCount", strandCount);
	computeShader.setFloat("deltaTime", deltaTime);
	computeShader.setFloat("runningTime", runningTime);

	auto localWorkGroupCount = computeShader.getLocalWorkGroupsCount();
        // Follow Ther Leader Algorithm
        dispatch_compute(computeShader, ceil_div(strandCount, localWorkGroupCount), 0);
        /* // Fill Volumes */
        dispatch_compute(computeShader, ceil_div(strandCount * particlesPerStrand, localWorkGroupCount), 1);
        // Coliisions
        dispatch_compute(computeShader, ceil_div(strandCount * particlesPerStrand, localWorkGroupCount), 2);
}
