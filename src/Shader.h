#pragma once
#include <string>
#include <glad/glad.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "PathConfig.h"

class Shader {
public:
	Shader() = default;
	virtual ~Shader() {
        glDeleteProgram(programID);
    }
	void use() const {
        glUseProgram(programID);
    }
	void setInt(const std::string& name, const int value) const;
	void setIntArray(const std::string& name, GLsizei count, const GLint value[]) const;
	void setUint(const std::string& name, const uint32_t value) const;
	void setUintArray(const std::string& name, int count, const uint32_t value[]) const;
	void setBool(const std::string& name, const bool value) const;
	void setBoolArray(const std::string& name, GLsizei count, bool values[]) const;
	void setFloat(const std::string& name, const float value) const;
	void setFloatArray(const std::string& name, GLsizei count, const GLfloat values[]) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2Array(const std::string& name, GLsizei count, const glm::vec2 values[]) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3Array(const std::string& name, GLsizei count, const glm::vec3 values[]) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4Array(const std::string& name, GLsizei count, const glm::vec4 values[]) const;
	void setMat2(const std::string& name, const glm::mat2& value) const;
	void setMat2Array(const std::string& name, GLsizei count, const glm::mat2 values[]) const;
	void setMat3(const std::string& name, const glm::mat3& value) const;
	void setMat3Array(const std::string& name, GLsizei count, const glm::mat3 values[]) const;
	void setMat4(const std::string& name, const glm::mat4& value) const;
	void setMat4Array(const std::string& name, GLsizei count, const glm::mat4 values[]) const;

	void bindShaderUboToBindingPoint(const std::string& uniformBlockName, const GLuint bindingPoint) const {
        glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, uniformBlockName.c_str()), bindingPoint);
    }

protected:
	GLuint programID = GL_NONE;
	mutable std::unordered_map<std::string, std::pair<GLint, bool>> uniformCache;
	GLint getUniformLocation(const std::string& name) const {
        if ((uniformCache.find(name)) != uniformCache.end())
            return uniformCache[name].first;

        GLint location = glGetUniformLocation(programID, name.c_str());
        if (location != -1)
        {
            uniformCache.insert(std::make_pair(name, std::make_pair(location, false)));
        }
        else if (!uniformCache[name].second)
        {
            std::cout << "Uniform variable '" << name << "' doesn't exist, or it is unused!" << std::endl;
            uniformCache[name].second = true;
        }

        return location;
    }

	void linkProgram() const {
        if (!programID)
            std::cout << "Error: No shaders attached!" << std::endl;

        GLint success;
        char infoLog[512];

        glLinkProgram(programID);
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            std::cout << "Failed to link program: " << infoLog << std::endl;
        }
    }
	void compileAndAttachShader(const std::string& shaderFileName, GLuint& shaderID) {
        std::string shaderCode;
        std::ifstream shaderFile;

        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            std::cout<<"SHADER_FOLDER: "<<SHADER_FOLDER<<"\n";
            shaderFile.open(SHADER_FOLDER + shaderFileName);
            std::stringstream ShaderStream;

            ShaderStream << shaderFile.rdbuf();
            shaderCode = ShaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << e.what() << std::endl;
            std::cout << "Error: File not successfully read/found!" << std::endl;
        }

        GLint success;
        char infoLog[512];
        const char* shaderCodeString = shaderCode.c_str();

        glShaderSource(shaderID, 1, &shaderCodeString, NULL);
        glCompileShader(shaderID);
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
            std::cout << "Failed to compile  shader: " << shaderFileName << " " << infoLog << std::endl;
        }

        if (!programID)
            programID = glCreateProgram();

        glAttachShader(programID, shaderID);
    }
};