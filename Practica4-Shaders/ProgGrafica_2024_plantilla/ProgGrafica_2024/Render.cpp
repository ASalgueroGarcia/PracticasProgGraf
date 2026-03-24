#include "Render.h"
#include "Object3D.h"
#include "EventManager.h"
#include <cstring>
#include <algorithm>

Render::Render(float anchura, float altura)
{	
	// Crear ventana principal de OpenGL
	this->window = glfwCreateWindow(anchura, altura, "Triangulos Rotando", nullptr, nullptr);
	initGL();
}

void Render::initGL()
{
	// Asociar el contexto OpenGL de esta ventana al hilo actual
	glfwMakeContextCurrent(window);

	// Cargar funciones de OpenGL mediante GLAD
	gladLoadGL(glfwGetProcAddress);

	// Activar depth test
	glEnable(GL_DEPTH_TEST);

	// ===== Carga y compilación de shaders =====
	// Leer vertex shader 
	std::ifstream vFile("data/shader.vertex");
	std::stringstream vBuffer;
	vBuffer << vFile.rdbuf();
	std::string vCode = vBuffer.str();
	const char* vShaderCode = vCode.c_str();

	// Leer fragment shader
	std::ifstream fFile("data/shader.fragment");
	std::stringstream fBuffer;
	fBuffer << fFile.rdbuf();
	std::string fCode = fBuffer.str();
	const char* fShaderCode = fCode.c_str();

	// Crear vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);

	// Crear fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);

	// Crear programa shader
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Borrar shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Activar shaders
	glUseProgram(shaderProgram);

	// ===== Configuración viewport
	// Ajustar viewport al tamaño real del framebuffer
	int fbw = 0, fbh = 0;
	glfwGetFramebufferSize(window, &fbw, &fbh);
	if (fbw > 0 && fbh > 0) {
		glViewport(0, 0, fbw, fbh);
	}

	// Evitar permitir redimensionado
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Inicializar gestor de eventos (Object3D::move lee EventManager::keyMap)
	EventManager::initEventManager(this->window);
}

void Render::putObject(Object3D* obj)
{
	// Generar buffers
	bufferObject_t bo = { 0, 0, 0 };
	glGenVertexArrays(1, &bo.bufferId);
	glGenBuffers(1, &bo.vertexBufferId);
	glGenBuffers(1, &bo.indexBufferId);

	glBindVertexArray(bo.bufferId);

	// Cargar vertices
	glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertex_t) * obj->vertexList.size(),
		obj->vertexList.data(),
		GL_STATIC_DRAW);

	// Cargar indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * obj->idList.size(),
		obj->idList.data(),
		GL_STATIC_DRAW);

	// ===== ATRIBUTO POSICION =====
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void*)offsetof(vertex_t, posicion)
	);

	// ===== ATRIBUTO COLOR =====
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void*)offsetof(vertex_t, color)
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Desvincular VAO por seguridad
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Almacenar en mapa y en la lista de objetos
	bufferList[obj->objId] = bo;
	objectList.push_back(*obj);
}

void Render::removeObject(Object3D* obj)
{
	// Buscar los buffers asociados al objeto
	auto object = bufferList.find(obj->objId);

	if (object != bufferList.end())
	{
		bufferObject_t& bo = object->second;

		// Eliminar buffers de la GPU
		glDeleteBuffers(1, &bo.vertexBufferId);
		glDeleteBuffers(1, &bo.indexBufferId);
		glDeleteVertexArrays(1, &bo.bufferId);

		// Eliminar del mapa
		bufferList.erase(object);
	}

	// Eliminar el objeto de la lista de objetos
	objectList.erase(
		std::remove_if(objectList.begin(), objectList.end(),
			[&](const Object3D& o)
			{
				return o.objId == obj->objId;
			}),
		objectList.end()
	);
}

void Render::putCamera(Camera* cam)
{
	this->cam = cam;
}

void Render::DrawGL()
{
	// Limpiar pantalla y depth buffer
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& obj : objectList)
	{
		auto& bo = bufferList[obj.objId];

		// ===== USAR PROGRAMA DEL OBJETO =====
		obj.prg->use();

		// ===== MATRICES =====
		Matriz4x4f view = cam->getMatrixLookAt();
		Matriz4x4f proj = cam->getMatrixPerspective();
		Matriz4x4f MVP = proj * view * obj.modelMatrix;

		// Pasar MVP al shader (nombre desde .fiis)
		obj.prg->setUniformData(obj.uniformMVPName, MVP);

		glBindVertexArray(bo.bufferId);
		glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId);

		// ===== ATRIBUTO POSICION =====
		obj.prg->setAttributeData(
			obj.attrPosName,
			4,
			GL_FLOAT,
			GL_FALSE,
			sizeof(vertex_t),
			(void*)offsetof(vertex_t, posicion)
		);

		// ===== ATRIBUTO COLOR =====
		obj.prg->setAttributeData(
			obj.attrColorName,
			4,
			GL_FLOAT,
			GL_FALSE,
			sizeof(vertex_t),
			(void*)offsetof(vertex_t, color)
		);

		glDrawElements(
			GL_TRIANGLES,
			static_cast<GLsizei>(obj.idList.size()),
			GL_UNSIGNED_INT,
			0
		);

		glBindVertexArray(0);
	}
}
void Render::mainLoop()
{
	float lastTime = glfwGetTime();	// Tiempo inicial

	// Bucle principal
	while (!glfwWindowShouldClose(window))
	{
		// Tiempo entre frames
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		EventManager::updateEvents();

		// mover camara
		if (cam)
			cam->move(deltaTime);

		for (auto& obj : objectList)
			obj.move(deltaTime);

		DrawGL();

		glfwSwapBuffers(window);
	}
}