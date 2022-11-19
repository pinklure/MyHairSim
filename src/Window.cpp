#include <iostream>
#include "Window.h"
#include <glm/common.hpp>

Window::Window(uint32_t winWidth, uint32_t winHeight, const char* winName, int sampleCount)
{
	/* Initialize the library */
	if (!glfwInit())
		std::cerr << "Failed to initialize GLFW!" << std::endl;

	glfwWindowHint(GLFW_SAMPLES, sampleCount);
	/* Create a windowed mode window and its OpenGL context */
	auto window = glfwCreateWindow(winWidth, winHeight, winName, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		std::cerr << "Failed to create window!" << std::endl;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowUserPointer(window, this);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetWindowUserPointer(window, &resized);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
	}

	this->windowHandle = window;

	onUpdate();
}

Window::~Window()
{
	glfwTerminate();
}

void Window::onUpdate()
{
	resized = false;

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

glm::ivec2 Window::getWindowSize() const
{
	glm::ivec2 winSize;
	glfwGetWindowSize(windowHandle, &winSize.x, &winSize.y);
	return winSize;
}

void Window::windowResizeCallback(GLFWwindow* window, int w, int h)
{
	*(static_cast<bool*>(glfwGetWindowUserPointer(window))) = true;
}
