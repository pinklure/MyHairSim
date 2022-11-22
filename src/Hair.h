#pragma once
#include "ComputeShader.h"
#include "Sphere.h"
#include "PathConfig.h"
#include "OBJ_Loader.h"
#include <glm/gtc/random.hpp>
#include "glm/gtc/quaternion.hpp"
#include <memory>
#include <vector>
#include <array>

const glm::vec4 WIND = {0.f, 0.f, 0.f, 0.2f};
const float GRAVITY = -9.81f;
const float FRICTION_FACTOR = 0.02f;
const float VELOCITY_DAMPING_COEFFICIENCY = 0.9f;

const float PARTICLE_MASS = 0.1f;
const uint32_t PARTICLE_PER_HAIR = 15U;
const uint32_t MAX_HAIR_COUNT = 30000U;
const float HAIR_LENGTH = 4.f;


class Hair : public Entity {
public:
	Hair(uint32_t _strandCount)
    : hair_count(_strandCount), computeShader("HairComputeShader.glsl")
    {
        computeShader.use();
        computeShader.setUint("hairData.strandCount", hair_count);
        computeShader.setFloat("hairData.particleMass", PARTICLE_MASS);
        computeShader.setFloat("force.gravity", GRAVITY);
        computeShader.setVec4("force.wind", WIND);
        computeShader.setFloat("frictionCoefficient", FRICTION_FACTOR);
        computeShader.setFloat("velocityDampingCoefficient", VELOCITY_DAMPING_COEFFICIENCY);
        constructModel();
    }
	~Hair() {
        glDeleteBuffers(1, &velocityArrayBuffer);
        glDeleteBuffers(1, &volumeDensities);
        glDeleteBuffers(1, &volumeVelocities);
    }

	void draw() const override {
        glBindVertexArray(vao);
        for (uint32_t i = 0; i < hair_count; ++i)
        {
            glDrawArrays(GL_LINE_STRIP, i * PARTICLE_PER_HAIR, PARTICLE_PER_HAIR);
        }
        glBindVertexArray(GL_NONE);
    }

	void applyPhysics(float deltaTime, float runningTime);

private:
	GLuint velocityArrayBuffer = GL_NONE;		// Shader storage buffer object for velocities
	GLuint volumeDensities = GL_NONE;
	GLuint volumeVelocities = GL_NONE;

	uint32_t hair_count;

	ComputeShader computeShader;
	void constructModel();

	// Head variables
	glm::vec3 headColor;
	GLuint headVbo = GL_NONE;
	GLuint headVao = GL_NONE;
	GLuint headEbo = GL_NONE;
	uint32_t indexCount = 0;
	std::array<std::unique_ptr<Sphere>, 7> ellipsoids;
	float ellipsoidsRadius = 0.5f;
};
inline void Hair::constructModel()
{
    for (auto& e : ellipsoids) {
        e = std::make_unique<Sphere>(50, 30, ellipsoidsRadius);
    }

    ellipsoids[0]->translate(glm::vec3(-1.149691f, -0.971486f, 0.240179f));
    ellipsoids[0]->rotate(-20.f, glm::vec3(1.f, 0.f, 0.f));
    ellipsoids[0]->rotate(10.f, glm::vec3(0.f, 1.f, 0.f));
    ellipsoids[0]->rotate(10.f, glm::vec3(0.f, 0.f, 1.f));
    ellipsoids[0]->scale(glm::vec3(0.321661, 0.794607, 0.595070));

    ellipsoids[1]->translate(glm::vec3(1.149691f, -0.971486f, 0.240179f));
    ellipsoids[1]->rotate(-20.f, glm::vec3(1.f, 0.f, 0.f));
    ellipsoids[1]->rotate(-10.f, glm::vec3(0.f, 1.f, 0.f));
    ellipsoids[1]->rotate(-10.f, glm::vec3(0.f, 0.f, 1.f));
    ellipsoids[1]->scale(glm::vec3(0.321661, 0.794607, 0.595070));

    ellipsoids[2]->translate(glm::vec3(0.000000f, -0.388153f, 0.191956f));
    ellipsoids[2]->scale(glm::vec3(2.368103f, 2.519852f, 2.818405f));

    ellipsoids[3]->translate(glm::vec3(0.000000f, -1.074509f, 1.604706f));
    ellipsoids[3]->rotate(-20.f, glm::vec3(1.f, 0.f, 0.f));
    ellipsoids[3]->scale(glm::vec3(0.559738f, 0.620941f, 0.421112f));

    ellipsoids[4]->translate(glm::vec3(0.000000, -0.717041, 0.566460));
    ellipsoids[4]->rotate(-30.f, glm::vec3(1.f, 0.f, 0.f));
    ellipsoids[4]->scale(glm::vec3(2.058214, 3.539791, 1.799336));

    ellipsoids[5]->translate(glm::vec3(0.000000f, -2.556824f, -0.068329f));
    ellipsoids[5]->rotate(20.f, glm::vec3(1.f, 0.f, 0.f));
    ellipsoids[5]->scale(glm::vec3(1.798798f, 1.282593f, 1.661377f));

    ellipsoids[6]->translate(glm::vec3(-0.015701f, -1.032532f, 0.122619f));
    ellipsoids[6]->scale(glm::vec3(2.357361f, 3.127426f, 2.326767f));

    const glm::vec3 headTranslation(0.f, -3.f, 0.f);
    const glm::vec3 headScale(0.2f);
    glm::quat headRotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
    headRotation = glm::rotate(headRotation, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    glm::mat4 headTransform = glm::scale(glm::translate(glm::mat4(1.f), headTranslation) * glm::mat4_cast(headRotation), headScale);
    headColor = glm::vec3(0.85f, 0.48f, 0.2f);

    objl::Loader loader;
    std::vector<float> headData;
    if (loader.LoadFile(TEXTURE_FOLDER + "FemaleHead/FemaleHead.obj"))
    {
        headData.reserve(loader.LoadedVertices.size() * 6 * 3); // 3 position component and 3 normal components
        for (auto& vertex : loader.LoadedVertices)
        {
            glm::vec3 transformedVertex = headTransform * glm::vec4(vertex.Position.X, vertex.Position.Y, vertex.Position.Z, 1.f);
            vertex.Position.X = transformedVertex.x;
            vertex.Position.Y = transformedVertex.y;
            vertex.Position.Z = transformedVertex.z;

            glm::vec3 transformedNormal = glm::inverse(glm::transpose(headTransform)) * glm::vec4(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z, 1.f);
            vertex.Normal.X = transformedNormal.x;
            vertex.Normal.Y = transformedNormal.y;
            vertex.Normal.Z = transformedNormal.z;

            headData.push_back(vertex.Position.X);
            headData.push_back(vertex.Position.Y);
            headData.push_back(vertex.Position.Z);
            headData.push_back(vertex.Normal.X);
            headData.push_back(vertex.Normal.Y);
            headData.push_back(vertex.Normal.Z);
        }

        glCreateVertexArrays(1, &headVao);
        glGenBuffers(1, &headVbo);
        glGenBuffers(1, &headEbo);
        glBindVertexArray(headVao);
        glBindBuffer(GL_ARRAY_BUFFER, headVbo);
        glBufferData(GL_ARRAY_BUFFER, headData.size() * sizeof(float), headData.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, headEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, loader.LoadedIndices.size() * sizeof(GLuint), loader.LoadedIndices.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);
        indexCount = loader.LoadedIndices.size();
    }
    else
    {
        std::cout << "File doesn't exist" << std::endl;
    }

    std::vector<float> data;
    data.reserve(MAX_HAIR_COUNT * PARTICLE_PER_HAIR * 3);

    float segmentLength = HAIR_LENGTH/ (PARTICLE_PER_HAIR - 1);

    computeShader.setFloat("hairData.segmentLength", HAIR_LENGTH/ (PARTICLE_PER_HAIR - 1));
    computeShader.setUint("hairData.particlesPerStrand", PARTICLE_PER_HAIR);
    computeShader.setFloat("ellipsoidRadius", ellipsoidsRadius);
    uint32_t counter = 0;
    for (uint32_t i = 0; i < loader.LoadedVertices.size(); i += 10)
    {
        const auto& vertex = loader.LoadedVertices[i];
        if ((vertex.Position.Y > -1.f && vertex.Position.Z < 0.f) || (vertex.Position.Y > -0.5f && vertex.Position.Z < 0.7f) || (vertex.Position.Y >= 0.5f && vertex.Position.Z < 1.7f))
        {
            ++counter;
            if (counter >= MAX_HAIR_COUNT- 1) break;
            for (uint32_t j = 0; j < PARTICLE_PER_HAIR; ++j)
            {
                glm::vec3 particle(vertex.Position.X, vertex.Position.Y, vertex.Position.Z);
                particle += glm::normalize(particle) * (float)j * segmentLength;
                data.push_back(particle.x);
                data.push_back(particle.y);
                data.push_back(particle.z);
            }
        }
    }

    const int strandsOnHair = counter;
    for (; counter < MAX_HAIR_COUNT; ++counter)
    {
        int randomNumber = glm::linearRand(0, strandsOnHair - 1) * PARTICLE_PER_HAIR * 3;
        glm::vec3 firstCoords(data[randomNumber], data[randomNumber + 1], data[randomNumber + 2]);
        randomNumber += PARTICLE_PER_HAIR * 3;
        glm::vec3 secondCoords(data[randomNumber], data[randomNumber + 1], data[randomNumber + 2]);
        glm::vec3 coordsBetween = secondCoords + (firstCoords - secondCoords) * 0.5f;
        for (uint32_t j = 0; j < PARTICLE_PER_HAIR; ++j)
        {
            glm::vec3 particle = coordsBetween + glm::normalize(coordsBetween) * (float)j * segmentLength;
            data.push_back(particle.x);
            data.push_back(particle.y);
            data.push_back(particle.z);
        }
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(GL_NONE);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);

    // Velocities
    data.clear();
    data.reserve(MAX_HAIR_COUNT * PARTICLE_PER_HAIR * 3);
    for (uint32_t i = 0; i < MAX_HAIR_COUNT * PARTICLE_PER_HAIR * 3; ++i)
        data.push_back(0.f);

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

    for (uint32_t i = 0; i < ellipsoids.size(); ++i)
    {
        computeShader.setMat4("ellipsoids[" + std::to_string(i) + "]", transformMatrix * ellipsoids[i]->getTransformMatrix());
    }

    computeShader.setMat4("model", transformMatrix);
    computeShader.setUint("hairData.strandCount", hair_count);
    computeShader.setFloat("deltaTime", deltaTime);
    computeShader.setFloat("runningTime", runningTime);
    computeShader.setUint("state", 0);
    GLuint localWorkGroupCountX = computeShader.getLocalWorkGroupsCount().x;
    GLuint globalWorkGroupCount = hair_count / localWorkGroupCountX;
    if (hair_count % localWorkGroupCountX != 0)
    {
        globalWorkGroupCount += 1;
    }

    computeShader.setGlobalWorkGroupCount(globalWorkGroupCount);
    computeShader.dispatch();
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    globalWorkGroupCount = hair_count * PARTICLE_PER_HAIR / localWorkGroupCountX;
    if ((hair_count * PARTICLE_PER_HAIR) % localWorkGroupCountX != 0)
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
