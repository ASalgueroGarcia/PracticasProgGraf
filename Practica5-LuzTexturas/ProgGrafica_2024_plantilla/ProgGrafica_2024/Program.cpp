#include "Program.h"

Program::Program()
{
	idProgram = glCreateProgram();
}

void Program::addShader(std::string fileName)
{
	Shader* sh = new Shader(fileName);
	shaderList.push_back(sh);
}

void Program::linkProgram()
{
	for (auto sh : shaderList)
	{
		glAttachShader(idProgram, sh->idShader);
	}

	glLinkProgram(idProgram);

	checkErrors();

	clean();
}

void Program::checkErrors()
{
	GLint success;
	glGetProgramiv(idProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		GLchar infoLog[1024];
		glGetProgramInfoLog(idProgram, 1024, NULL, infoLog);
		std::cout << "ERROR linking program:\n" << infoLog << std::endl;
	}
}

void Program::clean()
{
	for (auto sh : shaderList)
	{
		sh->clean();
		delete sh;
	}
	shaderList.clear();
}

void Program::readVarList()
{
	GLint count;
	glGetProgramiv(idProgram, GL_ACTIVE_ATTRIBUTES, &count);

	char name[256];
	GLsizei length;
	GLint size;
	GLenum type;

	for (int i = 0; i < count; i++)
	{
		glGetActiveAttrib(idProgram, i, 256, &length, &size, &type, name);
		GLint location = glGetAttribLocation(idProgram, name);
		varList[name] = location;
		std::cout << "Atributo detectado en shader: " << name << std::endl;
	}

	glGetProgramiv(idProgram, GL_ACTIVE_UNIFORMS, &count);

	for (int i = 0; i < count; i++)
	{
		glGetActiveUniform(idProgram, i, 256, &length, &size, &type, name);
		GLint location = glGetUniformLocation(idProgram, name);
		varList[name] = location;
		std::cout << "Uniforme detectado en shader: " << name << std::endl;
	}
}

void Program::initLightUniforms(Light* light)
{
	if (light == nullptr) return;
	setUniformVector3f("lightPosition", make_vector3f(light->position.x, light->position.y, light->position.z));
	setUniformVector3f("lightColor", make_vector3f(light->color.x, light->color.y, light->color.z));
	setUniformFloat("lightKa", light->ka);
	setUniformFloat("lightKd", light->kd);
	setUniformFloat("lightKs", light->ks);
}

void Program::initMaterialUniforms(Material* mat)
{
	if (mat == nullptr) return;
	setUniformInt("usaTextura", mat->usaTextura ? 1 : 0);
	setUniformInt("shiny", mat->shiny);
	setUniformFloat("materialAlpha", mat->alpha);
	setUniformFloat("materialKa", mat->ka);
	setUniformFloat("materialKd", mat->kd);
	setUniformFloat("materialKs", mat->ks);
}

void Program::use()
{
	glUseProgram(idProgram);
}

void Program::setUniformData(std::string nombre, const Matriz4x4f& mat)
{
	if (varList.find(nombre) == varList.end()) return;

	GLint loc = varList[nombre];

	glUniformMatrix4fv(loc, 1, GL_TRUE, mat.max);
}

void Program::setAttributeData(std::string nombre, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
	if (varList.find(nombre) == varList.end())	return;

	GLint loc = varList[nombre];

	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, size, type, normalized, stride, pointer);
}

void Program::setUniformLight(Light* light)
{
	if (light == nullptr) return;

	setUniformVector3f("lightDirection", make_vector3f(light->direction.x, light->direction.y, light->direction.z));
	setUniformVector3f("lightPosition", make_vector3f(light->position.x, light->position.y, light->position.z));
	setUniformVector3f("lightColor", make_vector3f(light->color.x, light->color.y, light->color.z));
	setUniformFloat("lightKa", light->ka);
	setUniformFloat("lightKd", light->kd);
	setUniformFloat("lightKs", light->ks);
}

void Program::setUniformMaterial(Material* mat)
{
	if (mat == nullptr) return;

	setUniformInt("usaTextura", mat->usaTextura ? 1 : 0);
	setUniformInt("shiny", mat->shiny);
	setUniformFloat("materialAlpha", mat->alpha);
	setUniformFloat("materialKa", mat->ka);
	setUniformFloat("materialKd", mat->kd);
	setUniformFloat("materialKs", mat->ks);
}

void Program::setUniformVector3f(std::string nombre, const Vector3f& vec)
{
	if (varList.find(nombre) == varList.end()) return;
	glUniform3f(varList[nombre], vec.x, vec.y, vec.z);
}

void Program::setUniformFloat(std::string nombre, float value)
{
	if (varList.find(nombre) == varList.end()) return;
	glUniform1f(varList[nombre], value);
}

void Program::setUniformInt(std::string nombre, int value)
{
	if (varList.find(nombre) == varList.end()) return;
	glUniform1i(varList[nombre], value);
}