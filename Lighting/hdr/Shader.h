#pragma once
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

	unsigned int getShader() {
		return shaderProgram;
	}
private:
	unsigned int shaderProgram;
	unsigned int vertexShader;
	unsigned int fragmentShader;

	void checkCompileErrors(unsigned int shader, std::string type);
};

