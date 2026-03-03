#include "Render.h"
#include "Object3D.h"
#include "EventManager.h"
#include <cstring>
#include <algorithm>

Render::Render(float anchura, float altura)
{
	// Crear ventana (si necesitas hints específicos muévelos antes de crear la ventana)
	this->window = glfwCreateWindow(anchura, altura, "Triangulos Rotando", nullptr, nullptr);
	initGL();
}

void Render::initGL()
{
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

	// Ajustar viewport al tamaño real del framebuffer
	int fbw = 0, fbh = 0;
	glfwGetFramebufferSize(window, &fbw, &fbh);
	if (fbw > 0 && fbh > 0) {
		glViewport(0, 0, fbw, fbh);
	}

	// Evitar permitir redimensionado (idealmente antes de crear la ventana)
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Inicializar gestor de eventos (Object3D::move lee EventManager::keyMap)
	EventManager::initEventManager(this->window);
}

void Render::putObject(Object3D* obj)
{
	// generar buffers
	bufferObject_t bo = { 0, 0, 0 };
	glGenVertexArrays(1, &bo.bufferId);       // VAO
	glGenBuffers(1, &bo.vertexBufferId);      // VBO
	glGenBuffers(1, &bo.indexBufferId);       // EBO

	// bind VAO
	glBindVertexArray(bo.bufferId);

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * obj->vertexList.size(), obj->vertexList.data(), GL_STATIC_DRAW);

	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj->idList.size(), obj->idList.data(), GL_STATIC_DRAW);

	// Configurar atributo de vértice (posición vec4 en location 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                      // location
		4,                      // 4 floats (x,y,z,w)
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void*)0
	);

	// Desvincular VAO por seguridad (EBO queda referenciado por el VAO)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// almacenar en mapa y en la lista de objetos
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

void Render::DrawGL()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (auto& obj : objectList)
	{
		auto& bo = bufferList[obj.objId];

		// Transformar vértices en CPU
		std::vector<vertex_t> transformedVertices;
		transformedVertices.reserve(obj.vertexList.size());

		for (auto& v : obj.vertexList)
		{
			vertex_t vt;
			vt.posicion = obj.modelMatrix * v.posicion;
			transformedVertices.push_back(vt);
		}

		// Reenviar datos al VBO
		glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(vertex_t) * transformedVertices.size(),
			transformedVertices.data(),
			GL_DYNAMIC_DRAW);

		// Dibujar usando VAO asociado
		glBindVertexArray(bo.bufferId);

		glDrawElements(GL_TRIANGLES,
			static_cast<GLsizei>(obj.idList.size()),
			GL_UNSIGNED_INT,
			0);

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

		for (auto& obj : objectList)
			obj.move(deltaTime);

		DrawGL();

		glfwSwapBuffers(window);
	}
}