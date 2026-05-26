#pragma once
#include "Object3D.h"
#include "Camera.h"
#include "Luz.h"

class Render
{
public:
    typedef struct {
        unsigned int bufferId;
        unsigned int vertexBufferId;
        unsigned int indexBufferId;
    } bufferObject_t;

    // Camara que mira al centro {0,0,0} desde {0,0,20}
    Camera* cam = new Camera({ 0,0,20,1 }, { 0,0,0,1 });
    Luz* luz = new Luz({ 0,0,3,1 });

    vector<Object3D*> objectList;     // lista de objetos activos
    vector<Object3D*> nextObjectList; // lista de objetos pendientes de añadir

    map<unsigned int, bufferObject_t> bufferObjectList;
    bool salir = false;
    GLFWwindow* window = nullptr;

    Render();

    void setupObject(Object3D* obj);
    void addObject(Object3D* obj);
    void deleteObject(Object3D* obj);

    vector<Object3D*> getCollisions(Object3D* obj, int objType);

    void updateObjects(double timeStep);
    void drawObjects();
    void mainLoop();
};