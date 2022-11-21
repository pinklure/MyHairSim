#include "Window.h"
#include "Camera.h"
#include "DrawingShader.h"
#include "Hair.h"
#include <memory>
#include <glm/gtc/matrix_access.hpp>

template<typename T> using Unique = std::unique_ptr<T>;

int main()
{
	Unique<Window> window = std::make_unique<Window>(1440, 810, "Hair Simulation", 4);
	glEnable(GL_MULTISAMPLE);

    // 开启深度测试
	glEnable(GL_DEPTH_TEST);
    // 在片段深度值小于等于缓冲区的深度值时通过测试
	glDepthFunc(GL_LEQUAL);

    // 重置颜色缓冲区
    {
        auto number = 0.8f;
        glClearColor(number, number, number, 1.f);
    }

    // 设置相机
	PerspectiveCamera cam;
    {
        cam.setProjectionAspectRatio(1440.f / 810);
        cam.setPosition(glm::vec3(-5.f, 3.f, 5.f));
        cam.setCenter(glm::vec3(0.f));
        cam.setProjectionViewingAngle(100.f);
    }

	// Basic hair
	Unique<Hair> hair = std::make_unique<Hair>(2000, 4.f, 0.f);
	hair->color = glm::vec3(0.45f, 0.18f, 0.012f);

	DrawingShader hairShader("HairVertexShader.glsl", "HairGeometryShader.glsl", "HairFragmentShader.glsl");

	hairShader.use();
	hairShader.setFloat("light.constant", 1.f);
	hairShader.setFloat("light.linear", 0.024f);
	hairShader.setFloat("light.quadratic", 0.0021f);

	glViewport(0, 0, window->getWindowSize().x, window->getWindowSize().y);

    while (!window->shouldClose())
	{
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glm::abs(window->getTime().deltaTime - window->getTime().lastDeltaTime) < 0.1f) {
            hair->applyPhysics(window->getTime().deltaTime, window->getTime().runningTime);
        }

		glEnable(GL_CULL_FACE);

		glm::vec3 tempColor = hair->color;
		hair->color = glm::vec3(1.f, 0.576f, 0.229f);
		hair->drawHead();
		hair->color = tempColor;

		hairShader.use();
		hairShader.setMat4("projection", cam.getProjection());
		hairShader.setMat4("view", cam.getView());
		hairShader.setMat4("model", hair->getTransformMatrix());
		hairShader.setFloat("curlRadius", hair->getCurlRadius());
		hairShader.setVec3("eyePosition", cam.getPosition());
		hairShader.setUint("particlesPerStrand", hair->getParticlesPerStrand());
		hair->updateColorsBasedOnMaterial(hairShader, Entity::Material::HAIR);
		hair->draw();

		if (window->isResized())
		{
			glm::ivec2 windowSize = window->getWindowSize();
			cam.setProjectionAspectRatio((float)windowSize.x / windowSize.y);
			glViewport(0, 0, windowSize.x, windowSize.y);
		}

		window->onUpdate();
	}

	return 0;
}
