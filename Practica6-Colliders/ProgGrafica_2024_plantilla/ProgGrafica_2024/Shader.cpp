#include "Shader.h"

Shader::Shader(std::string fileName)
{
	// Guardar nombre del fichero
	this->fileName = fileName;

	// Inicializar identificador
	idShader = 0;

	// Detectar tipo de shader según el nombre del fichero
	if (fileName.find(".vertex") != std::string::npos)
	{
		type = GL_VERTEX_SHADER;
	}
	else if (fileName.find(".fragment") != std::string::npos)
	{
		type = GL_FRAGMENT_SHADER;
	}
	else
	{
		type = 0;
		std::cout << "ERROR: tipo de shader no reconocido en fichero: " << fileName << std::endl;
		return;
	}

	// Leer código fuente y compilar
	readSource();
	compileShader();
}

void Shader::readSource()
{
	std::ifstream file(fileName);

	if (!file.is_open())
	{
		std::cout << "ERROR: no se pudo abrir el fichero shader: " << fileName << std::endl;
		return;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	source = buffer.str();

	file.close();
}

void Shader::compileShader()
{
	// Si no hay código fuente, no se puede compilar
	if (source.empty())
	{
		std::cout << "ERROR: el shader está vacío: " << fileName << std::endl;
		return;
	}

	// Crear shader
	idShader = glCreateShader(type);

	const char* shaderCode = source.c_str();
	glShaderSource(idShader, 1, &shaderCode, NULL);
	glCompileShader(idShader);

	checkErrors();
}

void Shader::checkErrors()
{
	GLint compiled = 0;
	glGetShaderiv(idShader, GL_COMPILE_STATUS, &compiled);

	if (compiled != GL_TRUE)
	{
		GLsizei logLength = 0;
		GLchar message[1024];

		glGetShaderInfoLog(idShader, 1024, &logLength, message);

		std::cout << "ERROR compilando shader: " << fileName << std::endl;
		std::cout << message << std::endl;
	}
}

void Shader::clean()
{
	// Una vez linkado en un programa, el shader ya puede eliminarse
	if (idShader != 0)
	{
		glDeleteShader(idShader);
		idShader = 0;
	}

	// Limpiar también el código fuente guardado
	source.clear();
}