#include "Render.h"

Render::Render(float anchura, float altura) 
{
	this->window = glfwCreateWindow(anchura, altura, "Triangulos Rotando", nullptr, nullptr);
	initGL();
}

void Render::initGL()
{
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	InputManager::init(this->window);
}

void Render::putObject(Object3D* obj)
{
	// generar buffers
	bufferObject_t bo = { -1,-1,-1 };
	glGenVertexArrays(1, &bo.bufferId); // generar lista de buffers
	glGenBuffers(1, &bo.vertexBufferId); // generar un buffer de vertices
	glGenBuffers(1, &bo.indexBufferId); // generar un buffer de IDs de vertices

	// copiar datos
	glBindVertexArray(bo.bufferId); // activar lista de buffers
	glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId); // activar buffer de vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * obj->vertexList.size(), obj->vertexList.data(), GL_STATIC_DRAW); // copia los datos al buffer activo (vertices)

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId); // activar array de indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj->idList.size(), obj->idList.data(), GL_STATIC_DRAW); // copia datos al buffer activo (indices)

	/*
	// Lectura de vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                          // location en el shader
		4,                          // 4 floats (x,y,z,w)
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void*)0
	);
	*/
	// guardar IDs
	bufferList[obj->objId] = bo;

	//objectList.push_back(*obj);
}

// Eliminar un objeto
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

// Función encargada de dibujar un frame completo en la ventana
void Render::DrawGL()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (auto& obj : objectList)
	{
		auto& bo = bufferList[obj.objId];

		// Transformar vértices en CPU
		std::vector<vertex_t> transformedVertices;

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

		// Dibujar
		glBindVertexArray(bo.bufferId);

		glDrawElements(GL_TRIANGLES,
			obj.idList.size(),
			GL_UNSIGNED_INT,
			0);
	}
}

// Función principal
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