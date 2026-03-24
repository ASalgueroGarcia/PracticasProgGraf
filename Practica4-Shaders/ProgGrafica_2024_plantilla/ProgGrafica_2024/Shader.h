#pragma once

#include "common.h"

class Shader
{
public:
	// Variable que almacena el identificador de OpenGL para este shader
	unsigned int idShader;

	// Variable que almacena el nombre del fichero de texto con el código de este shader
	std::string fileName;

	// Variable que almacena el tipo de shader que se ha creado
	GLenum type;

	// Variable que almacena el código del shader
	std::string source;

	/// <summary>
	/// Constructor de la clase. Recibe el nombre del fichero, lo guarda,
	/// detecta el tipo de shader, lee el código fuente y lo compila.
	/// </summary>
	Shader(std::string fileName);

	/// <summary>
	/// Método que abre el fichero de código del shader, lo lee y lo almacena
	/// en la variable source.
	/// </summary>
	void readSource();

	/// <summary>
	/// Método que compila el shader.
	/// </summary>
	void compileShader();

	/// <summary>
	/// Método que muestra si hay cualquier error tras compilar el shader.
	/// </summary>
	void checkErrors();

	/// <summary>
	/// Método que libera los datos de compilación y código del shader.
	/// Se invocará después desde Program cuando ya esté linkado.
	/// </summary>
	void clean();
};