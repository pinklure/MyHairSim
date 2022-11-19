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
	float frameRate = 0.f;
};

class Window {
public:
	Window(uint32_t winWidth = 1024, uint32_t winHeight = 768, const char* winName = "MyApplication", int sampleCount = 1) {
        if (!glfwInit())
            std::cerr << ">> Failed to initialize GLFW\n";
        glfwWindowHint(GLFW_SAMPLES, sampleCount);

        this->windowHandle = glfwCreateWindow(winWidth, winHeight, winName, nullptr, nullptr);
        if(this->windowHandle == nullptr) {
            glfwTerminate();
            std::cerr << ">> Failed to create window\n";
        }

        glfwMakeContextCurrent(this->windowHandle);
        glfwSetWindowUserPointer(this->windowHandle, this);
        glfwSetWindowSizeCallback(this->windowHandle, windowResizeCallback);
        glfwSetWindowUserPointer(this->windowHandle, &resized);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD\n";
        }

        onUpdate();
    }

	~Window() {
        glfwTerminate();
    }
	void onUpdate() {
        resized = false;

        glfwPollEvents();
        glfwSwapBuffers(windowHandle);

        auto currentTime = (float)glfwGetTime();
        t.lastDeltaTime = t.deltaTime;
        t.deltaTime = currentTime - t.runningTime;
        t.runningTime = currentTime;
        t.frameRate = 1.f / t.deltaTime;
    }

	glm::ivec2 getWindowSize() const {
        glm::ivec2 winSize;
        glfwGetWindowSize(windowHandle, &winSize.x, &winSize.y);
        return winSize;
    }

	bool shouldClose() const { return glfwWindowShouldClose(windowHandle); }
	const Time& getTime() const { return t; }
	bool isResized() const { return resized; }

private:
	Time t;
	GLFWwindow* windowHandle = nullptr;
	bool resized = false;
	mutable std::unordered_map<int, bool> keyStates;

    static void windowResizeCallback(GLFWwindow* window, int w, int h) {
        *(static_cast<bool*>(glfwGetWindowUserPointer(window))) = true;
    }
};