#include "Render.h"
#include "EventManager.h"

Render::Render()
{
    if (glfwInit() != GLFW_TRUE)
    {
        cout << "ERROR Iniciando GLFW\n";
    }
    else
    {
#ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
        this->window = glfwCreateWindow(640, 480, "PRGR 2026", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        gladLoadGL(glfwGetProcAddress);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        this->salir = false;
        EventManager::initEventManager(window);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void Render::setupObject(Object3D* obj)
{
    bufferObject_t bo = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
    glGenVertexArrays(1, &bo.bufferId);
    glGenBuffers(1, &bo.vertexBufferId);
    glGenBuffers(1, &bo.indexBufferId);

    glBindVertexArray(bo.bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * obj->vertexList.size(),
        obj->vertexList.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj->indexList.size(),
        obj->indexList.data(), GL_STATIC_DRAW);

    bufferObjectList[obj->objectId] = bo;
}

void Render::addObject(Object3D* obj)
{
    setupObject(obj);
    nextObjectList.push_back(obj);
}

void Render::deleteObject(Object3D* obj)
{
    auto it = bufferObjectList.find(obj->objectId);
    if (it != bufferObjectList.end())
    {
        glDeleteVertexArrays(1, &it->second.bufferId);
        glDeleteBuffers(1, &it->second.vertexBufferId);
        glDeleteBuffers(1, &it->second.indexBufferId);
        bufferObjectList.erase(it);
    }
    delete obj;
}

vector<Object3D*> Render::getCollisions(Object3D* obj, int objType)
{
    vector<Object3D*> result;

    if (!obj->coll)
        return result;

    for (auto& other : objectList)
    {
        if (other == obj)             continue;
        if (!other->active)           continue;
        if (other->type != objType)   continue;
        if (!other->coll)             continue;

        if (obj->coll->collisionTest(other->coll))
            result.push_back(other);
    }
    return result;
}

void Render::updateObjects(double timeStep)
{
    cam->moveObject(timeStep);
    luz->moveObject(timeStep);

    for (auto& obj : objectList)
    {
        obj->moveObject(timeStep);
        if (obj->coll)
            obj->coll->updateCollider(obj->computeModelMatrix());
    }

    map<float, vector<Object3D*>> aux;
    for (auto& obj : objectList)
    {
        if (obj->active)
        {
            float dist = distance(cam->pos, obj->posicion);
            aux[dist].push_back(obj);
        }
        else
        {
            deleteObject(obj);
        }
    }

    for (auto& obj : nextObjectList)
    {
        float dist = distance(cam->pos, obj->posicion);
        aux[dist].push_back(obj);
    }
    nextObjectList.clear();

    objectList.clear();
    for (auto l = aux.rbegin(); l != aux.rend(); ++l)
    {
        int size = (int)l->second.size();
        int origSize = (int)objectList.size();
        objectList.resize(origSize + size);
        std::memcpy(objectList.data() + origSize,
            l->second.data(),
            size * sizeof(Object3D*));
    }
}

void Render::drawObjects()
{
    for (auto& obj : objectList)
    {
        obj->prg->useProgram();
        auto bo = bufferObjectList[obj->objectId];
        glBindVertexArray(bo.bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId);

        matrix4x4f model = obj->computeModelMatrix();
        auto mvp = transpose(
            cam->computeProjectionMatrix(-0.001f, 1000.0f, 3.1416f / 4.0f, 4.0f / 3.0f) *
            cam->computeViewMatrix() *
            model);

        obj->prg->setMatrix(mvp, "MVP");
        obj->prg->setMatrix(transpose(model), "M");
        obj->prg->setCamera(cam);
        obj->prg->setLight(luz);
        obj->prg->setMaterial(obj->mat);

        obj->prg->setAttributeData("vPos", 4, GL_FLOAT, sizeof(vertex_t), (void*)offsetof(vertex_t, posicion));
        obj->prg->setAttributeData("vColor", 4, GL_FLOAT, sizeof(vertex_t), (void*)offsetof(vertex_t, color));
        obj->prg->setAttributeData("vNormal", 4, GL_FLOAT, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));
        obj->prg->setAttributeData("vTextureCoords", 4, GL_FLOAT, sizeof(vertex_t), (void*)offsetof(vertex_t, texturaCoords));

        glDrawElements(GL_TRIANGLES, (GLsizei)obj->indexList.size(), GL_UNSIGNED_INT, nullptr);
    }
}

void Render::mainLoop()
{
    double lastTime = 0;
    double newTime = glfwGetTime();
    double deltaTime = newTime - lastTime;

    while (!salir)
    {
        newTime = glfwGetTime();
        deltaTime = newTime - lastTime;
        lastTime = newTime;

        updateObjects(deltaTime);

        if (EventManager::keyMap[GLFW_KEY_ESCAPE] || glfwWindowShouldClose(window))
            salir = true;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawObjects();
        glfwSwapBuffers(window);
        EventManager::updateEvents();
    }

    for (auto& obj : objectList)
        deleteObject(obj);
    objectList.clear();

    for (auto& obj : nextObjectList)
        deleteObject(obj);
    nextObjectList.clear();

    glfwTerminate();
}