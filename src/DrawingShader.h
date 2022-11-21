#pragma once
#include "Shader.h"

class DrawingShader : public Shader {
public:
	DrawingShader(const std::string& vertexShaderFile, const std::string& geometryShaderFile, const std::string& fragmentShaderFile) {
        GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
        compileAndAttachShader(vertexShaderFile, vertexShaderID);
        compileAndAttachShader(fragmentShaderFile, fragmentShaderID);
        compileAndAttachShader(geometryShaderFile, geometryShaderID);
        linkProgram();
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
        glDeleteShader(geometryShaderID);
    }
	~DrawingShader() override = default;
};