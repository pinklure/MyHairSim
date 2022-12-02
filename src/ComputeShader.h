#pragma once
#include "Shader.h"

class ComputeShader : public Shader {
public:
	ComputeShader(const std::string& shaderFile) {
		GLuint shaderID = glCreateShader(GL_COMPUTE_SHADER);
		compileAndAttachShader(shaderFile, shaderID);
		linkProgram();
		glDeleteShader(shaderID);
	}

	~ComputeShader() override = default;

	void dispatch() const {
		glDispatchCompute(globalWorkGroup, 1, 1);
	}

        auto getLocalWorkGroupsCount() const {
		glm::ivec3 values;
		glGetProgramiv(programID, GL_COMPUTE_WORK_GROUP_SIZE, &values.x);
                return values.x;
	}

	void setGlobalWorkGroupCount(GLuint workGroup) {
                globalWorkGroup = workGroup;
	}

private:
	GLuint globalWorkGroup = 1;
};
