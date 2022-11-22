#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>
#include <iostream>

struct Time {
	float deltaTime = 0.f;
	float lastDeltaTime = 0.f;
	float runningTime = 0.f;
};

class Window {
public:
	Window(uint32_t winWidth = 1024, uint32_t winHeight = 768, const char* winName = "MyApplication", int sampleCount = 1) {
        if (!glfwInit()) {
            std::cerr << ">> Failed to initialize GLFW\n";
            exit(-1);
        }
        glfwWindowHint(GLFW_SAMPLES, sampleCount);

        this->windowHandle = glfwCreateWindow(winWidth, winHeight, winName, nullptr, nullptr);
        if(this->windowHandle == nullptr) {
            glfwTerminate();
            std::cerr << ">> Failed to create window\n";
            exit(-1);
        }

        glfwMakeContextCurrent(this->windowHandle);
        glfwSetWindowUserPointer(this->windowHandle, this);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD\n";
        }

        update();
    }

	~Window() {
        glfwTerminate();
    }
	void update() {
        auto currentTime = (float)glfwGetTime();
        tm.lastDeltaTime = tm.deltaTime;
        tm.deltaTime = currentTime - tm.runningTime;
        tm.runningTime = currentTime;

        glfwPollEvents();
        glfwSwapBuffers(windowHandle);
    }

	glm::ivec2 window_size() const {
        glm::ivec2 winSize;
        glfwGetWindowSize(windowHandle, &winSize.x, &winSize.y);
        return winSize;
    }

	bool shouldClose() const { return glfwWindowShouldClose(windowHandle); }
	const Time& getTime() const { return tm; }

private:
	Time tm;
	GLFWwindow* windowHandle = nullptr;
};