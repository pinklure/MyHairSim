#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>
#include<iostream>
#include <glm/common.hpp>

struct Time {
	float deltaTime = 0.f;
	float lastDeltaTime = 0.f;
	float runningTime = 0.f;
	float frameRate = 0.f;
};

class Window {
public:
	Window(uint32_t winWidth = 1024, uint32_t winHeight = 768, const char* winName = "MyApplication", int sampleCount = 1) {
		GLFWwindow* window = nullptr;

		/* Initialize the library */
		if (!glfwInit())
			std::cerr << "Failed to initialize GLFW!" << std::endl;

		glfwWindowHint(GLFW_SAMPLES, sampleCount);
		/* Create a windowed mode window and its OpenGL context */
		window = glfwCreateWindow(winWidth, winHeight, winName, NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			std::cerr << "Failed to create window!" << std::endl;
		}

		glfwMakeContextCurrent(window);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowUserPointer(window, this);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cerr << "Failed to initialize GLAD" << std::endl;
		}

		this->windowHandle = window;
		onUpdate();
	}

	~Window() {
		glfwTerminate();
	}
	void onUpdate() {
		// Updating keys
		glfwPollEvents();
		glfwSwapBuffers(windowHandle);

		// Updating time
		float currentTime = (float)glfwGetTime();
		t.lastDeltaTime = t.deltaTime;
		t.deltaTime = currentTime - t.runningTime;
		t.runningTime = currentTime;
		t.frameRate = 1.f / t.deltaTime;

		if (glm::abs(t.deltaTime - t.lastDeltaTime) > 0.2f)
			std::cout << "Frame rate: " << t.frameRate << std::endl;
	}

	glm::ivec2 getWindowSize() const {
		glm::ivec2 winSize;
		glfwGetWindowSize(windowHandle, &winSize.x, &winSize.y);
		return winSize;
	}
	bool shouldClose() const { return glfwWindowShouldClose(windowHandle); }
	const Time& getTime() const { return t; }

private:
	Time t;
	GLFWwindow* windowHandle = nullptr;
};