#include "Window.h"
#include "Camera.h"
#include "DrawingShader.h"
#include "Hair.h"
#include <memory>

template<typename T> using Unique = std::unique_ptr<T>;

int main()
{
	auto window = std::make_unique<Window>(1440, 810, "Hair Simulation", 4);
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
        cam.setPosition(glm::vec3(-3.5f, 2.f, 3.5f));
        cam.setCenter(glm::vec3(0.f));
        cam.setProjectionViewingAngle(100.f);
    }

	Unique<Hair> hair = std::make_unique<Hair>(2000);
	DrawingShader hairShader("HairVertexShader.glsl", "HairGeometryShader.glsl", "HairFragmentShader.glsl");

	glViewport(0, 0, window->window_size().x, window->window_size().y);

    while (!window->shouldClose())
	{
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glm::abs(window->getTime().deltaTime - window->getTime().lastDeltaTime) < 0.1f) {
            hair->applyPhysics(window->getTime().deltaTime, window->getTime().runningTime);
        }

		glEnable(GL_CULL_FACE);

		hairShader.use();
        // 投影变化
		hairShader.setMat4("projection", cam.getProjection());
        // 视角变化
		hairShader.setMat4("view", cam.getView());

        // 模型变化
		hairShader.setMat4("model", hair->getTransformMatrix());
		hairShader.setUint("particlesPerStrand", PARTICLE_PER_HAIR);

		hair->draw();

        window->update();
	}

	return 0;
}
