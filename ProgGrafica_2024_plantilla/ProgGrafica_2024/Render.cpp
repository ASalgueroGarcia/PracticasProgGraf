#include "Render.h"

Render::Render(float anchura, float altura) 
{
	this->window = glfwCreateWindow(anchura, altura, "Triangulos Rotando", nullptr, nullptr);
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

	// guardar IDs
	bufferList[obj->objId] = bo;
}

void Render::removeObject(Object3D* obj)
{
}

void Render::DrawGL()
{
}

void Render::mainLoop()
{
}
