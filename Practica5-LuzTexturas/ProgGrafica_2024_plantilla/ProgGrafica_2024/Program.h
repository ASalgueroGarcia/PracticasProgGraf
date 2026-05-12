#pragma once

#include "common.h"
#include "Shader.h"
#include "MathLib.h"
#include "Light.h"
#include "Material.h"

class Program
{
public:
	unsigned int idProgram;
	std::vector<Shader*> shaderList;
	std::map<std::string, unsigned int> varList;

	Program();
	void addShader(std::string fileName);
	void linkProgram();
	void checkErrors();
	void clean();
	void readVarList();
	void use();
	void setUniformData(std::string nombre, const Matriz4x4f& mat);
	void setAttributeData(std::string nombre, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);

	void setUniformLight(Light* light);
	void setUniformMaterial(Material* mat);
	void setUniformVector3f(std::string nombre, const Vector3f& vec);
	void setUniformFloat(std::string nombre, float value);
	void setUniformInt(std::string nombre, int value);
};