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

	// Una vez linkado, se pueden limpiar shaders
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

	// Atributos
	for (int i = 0; i < count; i++)
	{
		glGetActiveAttrib(idProgram, i, 256, &length, &size, &type, name);
		GLint location = glGetAttribLocation(idProgram, name);
		varList[name] = location;
	}

	// Uniforms
	glGetProgramiv(idProgram, GL_ACTIVE_UNIFORMS, &count);

	for (int i = 0; i < count; i++)
	{
		glGetActiveUniform(idProgram, i, 256, &length, &size, &type, name);
		GLint location = glGetUniformLocation(idProgram, name);
		varList[name] = location;
	}

	cout << "Atributo detectado en shader: " << name << endl;
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