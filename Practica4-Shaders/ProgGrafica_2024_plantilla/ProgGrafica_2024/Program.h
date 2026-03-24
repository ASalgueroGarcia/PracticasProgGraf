#pragma once

#include "common.h"
#include "Shader.h"
#include "MathLib.h"

class Program
{
public:
	// Identificador del programa en OpenGL
	unsigned int idProgram;

	// Lista de shaders del programa
	std::vector<Shader*> shaderList;

	// Mapa de variables (nombre -> location)
	std::map<std::string, unsigned int> varList;

	/// <summary>
	/// Constructor: crea el programa en OpenGL
	/// </summary>
	Program();

	/// <summary>
	/// Añade un shader al programa a partir de un fichero
	/// </summary>
	void addShader(std::string fileName);

	/// <summary>
	/// Linka todos los shaders del programa
	/// </summary>
	void linkProgram();

	/// <summary>
	/// Comprueba errores de linkado
	/// </summary>
	void checkErrors();

	/// <summary>
	/// Limpia shaders (después de linkar)
	/// </summary>
	void clean();

	/// <summary>
	/// Lee las variables del shader y las guarda en varList
	/// </summary>
	void readVarList();

	/// <summary>
	/// Activa este programa
	/// </summary>
	void use();

	/// <summary>
	/// Enviar uniform al shader
	/// </summary>
	void setUniformData(std::string nombre, const Matriz4x4f& mat);

	/// <summary>
	/// Configurar atributo
	/// </summary>
	void setAttributeData(std::string nombre, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
};