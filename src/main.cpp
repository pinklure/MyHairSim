#include "Window.h"
#include <memory>
#include "Camera.h"
#include "Hair.h"
#include "DrawingShader.h"
#include <glm/gtc/matrix_access.hpp>
#include <iostream>

int main()
{
	auto window = std::make_unique<Window>(1440, 810, "Hair Simulation", 4);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.8f, 0.8f, 0.8f, 1.f);

	PerspectiveCamera cam;
	cam.setProjectionAspectRatio(1440.f / 810);
	cam.setPosition(glm::vec3(-5.f, 3.f, 5.f));
	cam.setCenter(glm::vec3(0.f));


	// Basic hair
	auto hair = std::make_unique<Hair>(1000, 20);
	/* auto hair = std::make_unique<Hair>(1, 20); */
	/* hair->color = glm::vec3(0.22f, 0.12f, 0.02f); */
	hair->color = glm::vec3(0, 0, 0);

	// Shaders setup
	DrawingShader basicShader("BasicVertexShader.glsl", "BasicFragmentShader.glsl");
	glViewport(0, 0, window->getWindowSize().x, window->getWindowSize().y);

        while(!window->shouldClose()) {
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glm::abs(window->getTime().deltaTime - window->getTime().lastDeltaTime) < 0.1f)
			hair->applyPhysics(window->getTime().deltaTime, window->getTime().runningTime);

		basicShader.use();
		basicShader.setMat4("projection", cam.getProjection());
		basicShader.setMat4("view", cam.getView());
		basicShader.setMat4("model", hair->getTransformMatrix());
		basicShader.setVec3("objectColor", hair->color);
		hair->draw();

		glEnable(GL_CULL_FACE);

		window->onUpdate();
	}
}
