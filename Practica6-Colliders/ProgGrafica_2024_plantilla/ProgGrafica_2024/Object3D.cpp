#include "Object3D.h"
#include "EventManager.h"

Object3D::Object3D()
{
    objectId = idCounter++;

    vertexList =
    {
        {{ -0.5,  0.5, 0, 1 }, {1,0,0,1}},
        {{  0.5,  0.5, 0, 1 }, {1,0,0,1}},
        {{ -0.5, -0.5, 0, 1 }, {1,1,0,1}},
        {{  0.5, -0.5, 0, 1 }, {0,0,1,1}}
    };

    indexList = { 0,1,2, 2,1,3 };

    prg = new Shader();
    prg->addShaderProgram("data/shader.vertex");
    prg->addShaderProgram("data/shader.fragment");
    prg->linkProgram();

    computeNormals();
}

Object3D::Object3D(string frsModel)
{
    objectId = idCounter++;
    this->frsModel = frsModel;
    loadFromFile();
}

matrix4x4f Object3D::computeModelMatrix()
{
    matrix4x4f model =
        make_traslate(posicion.x, posicion.y, posicion.z) *
        make_rotate(rotacion.x, rotacion.y, rotacion.z) *
        make_scale(escalado.x, escalado.y, escalado.z);
    return model;
}

void Object3D::computeNormals()
{
    for (auto& v : vertexList)
        v.normal = { 0,0,0,0 };

    auto it = indexList.begin();
    while (it != indexList.end()) {
        int v1 = *it; it++;
        int v2 = *it; it++;
        int v3 = *it; it++;

        vec4float arista1 = normalize(vertexList[v2].posicion - vertexList[v1].posicion);
        vec4float arista3 = normalize(vertexList[v3].posicion - vertexList[v1].posicion);
        vec4float normal = cross(arista1, arista3);

        vertexList[v1].normal = vertexList[v1].normal + normal;
        vertexList[v2].normal = vertexList[v2].normal + normal;
        vertexList[v3].normal = vertexList[v3].normal + normal;
    }

    for (auto& v : vertexList)
        v.normal = normalize(v.normal);
}


#define skipComments()\
lineReader = istringstream(line);\
 do {\
    std::getline(f >> std::ws, line);\
 } while (line[0] == '\xef' || line[0] == '#');\
lineReader = istringstream(line);\

#define readMultipleData()\
vector<float> data;\
while (!lineReader.eof() && lineReader.tellg() != -1)\
{\
    float d = 0;\
    lineReader >> d;\
    data.push_back(d);\
}\
data.pop_back();\


void Object3D::loadFromFile()
{
    typedef enum { vertices, faces, normals, colors, textureCoords, done } dataTypesReading;
    string textureFile, vertexShader, fragmentShader;

    std::ifstream f(frsModel, std::ios::binary);
    if (!f.is_open())
    {
        cout << "ERROR: no se pudo abrir " << frsModel << "\n";
        return;
    }

    // Lee la siguiente linea no-comentario (comentarios empiezan por 0xE2 = euro UTF-8)
    auto nextLine = [&](string& line) {
        do {
            if (!std::getline(f >> std::ws, line)) return;
            // quitar \r final si existe
            if (!line.empty() && line.back() == '\r') line.pop_back();
        } while (!line.empty() && (unsigned char)line[0] == 0xE2);
        };

    // Parsea floats de una linea ignorando tokens con letras (ej: "1.0f")
    auto parseFloats = [](const string& line, vector<float>& out) {
        istringstream ss(line);
        string token;
        while (ss >> token) {
            bool hasAlpha = false;
            for (char c : token)
                if (std::isalpha((unsigned char)c)) { hasAlpha = true; break; }
            if (!hasAlpha) {
                try { out.push_back(std::stof(token)); }
                catch (...) {}
            }
        }
        };

    dataTypesReading mode = vertices;
    string line;

    do {
        switch (mode)
        {
        case vertices: {
            nextLine(line);
            int numVertex = std::stoi(line);
            vertexList.resize(numVertex);
            for (int i = 0; i < numVertex; i++) {
                nextLine(line);
                istringstream ss(line);
                ss >> vertexList[i].posicion.x
                    >> vertexList[i].posicion.y
                    >> vertexList[i].posicion.z;
                vertexList[i].posicion.w = 1.0f;
            }
            mode = faces;
        } break;

        case faces: {
            nextLine(line);
            int numFaces = std::stoi(line);
            indexList.resize(numFaces * 3);
            for (int i = 0; i < numFaces; i++) {
                nextLine(line);
                istringstream ss(line);
                ss >> indexList[i * 3] >> indexList[i * 3 + 1] >> indexList[i * 3 + 2];
            }
            mode = colors;
        } break;

        case colors: {
            nextLine(line);
            int numColors = std::stoi(line);
            for (int i = 0; i < numColors; i++) {
                nextLine(line);
                vector<float> data;
                parseFloats(line, data);
                if (data.size() < 4) continue;
                int numIds = (int)data.size() - 4;
                vec4float color = make_vec4float(
                    data[numIds], data[numIds + 1], data[numIds + 2], data[numIds + 3]);
                for (int j = 0; j < numIds; j++)
                    vertexList[(int)data[j]].color = color;
            }
            mode = normals;
        } break;

        case normals: {
            nextLine(line);
            int numNormals = std::stoi(line);
            for (int i = 0; i < numNormals; i++) {
                nextLine(line);
                vector<float> data;
                parseFloats(line, data);
                if (data.size() < 4) continue;
                int numIds = (int)data.size() - 4;
                vec4float normal = make_vec4float(
                    data[numIds], data[numIds + 1], data[numIds + 2], data[numIds + 3]);
                for (int j = 0; j < numIds; j++)
                    vertexList[(int)data[j]].normal = normal;
            }
            mode = textureCoords;
        } break;

        case textureCoords: {
            nextLine(line);
            int numTC = std::stoi(line);
            // linea extra de comentario antes de las coords
            for (int i = 0; i < numTC; i++) {
                nextLine(line);
                vector<float> data;
                parseFloats(line, data);
                if (data.size() < 3) continue;
                int idx = (int)data[0];
                vertexList[idx].texturaCoords.x = data[1];
                vertexList[idx].texturaCoords.y = data[2];
            }
            nextLine(line); textureFile = line;
            nextLine(line); vertexShader = line;
            nextLine(line); fragmentShader = line;
            mode = done;
        } break;
        }
    } while (!f.eof() && mode != done);

    prg = new Shader();
    prg->addShaderProgram(vertexShader);
    prg->addShaderProgram(fragmentShader);
    prg->linkProgram();

    mat->textura = new Texture(textureFile);
    mat->usaTextura = true;

    // --- Construir collider de esfera ---
    if (vertexList.empty() || indexList.empty())
    {
        cout << "ERROR: " << frsModel << " no se pudo cargar o esta vacio\n";
        return;
    }

    vec4float maxV = vertexList[0].posicion;
    vec4float minV = vertexList[0].posicion;
    for (auto& v : vertexList) {
        maxV.x = std::fmaxf(v.posicion.x, maxV.x);
        maxV.y = std::fmaxf(v.posicion.y, maxV.y);
        maxV.z = std::fmaxf(v.posicion.z, maxV.z);
        minV.x = std::fminf(v.posicion.x, minV.x);
        minV.y = std::fminf(v.posicion.y, minV.y);
        minV.z = std::fminf(v.posicion.z, minV.z);
    }

    vec4float centro;
    centro.x = (maxV.x + minV.x) * 0.5f;
    centro.y = (maxV.y + minV.y) * 0.5f;
    centro.z = (maxV.z + minV.z) * 0.5f;
    centro.w = 1.0f;

    float radio = 0.0f;
    for (auto& v : vertexList) {
        vec4float d = v.posicion - centro;
        float dist = std::sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
        if (dist > radio) radio = dist;
    }

    coll = new Sphere();
    coll->center = centro;
    coll->centerPrime = centro;
    coll->radius = radio;
    coll->max = maxV;
    coll->min = minV;

    auto idx = indexList.begin();
    while (idx != indexList.end()) {
        int i0 = *idx; idx++;
        int i1 = *idx; idx++;
        int i2 = *idx; idx++;

        vec4float p0 = vertexList[i0].posicion;
        vec4float p1 = vertexList[i1].posicion;
        vec4float p2 = vertexList[i2].posicion;

        particle3D* p = new particle3D();
        p->pos.x = (p0.x + p1.x + p2.x) / 3.0f;
        p->pos.y = (p0.y + p1.y + p2.y) / 3.0f;
        p->pos.z = (p0.z + p1.z + p2.z) / 3.0f;
        p->pos.w = 1.0f;
        p->size.x = std::fmaxf(p0.x, std::fmaxf(p1.x, p2.x)) - std::fminf(p0.x, std::fminf(p1.x, p2.x));
        p->size.y = std::fmaxf(p0.y, std::fmaxf(p1.y, p2.y)) - std::fminf(p0.y, std::fminf(p1.y, p2.y));
        p->size.z = std::fmaxf(p0.z, std::fmaxf(p1.z, p2.z)) - std::fminf(p0.z, std::fminf(p1.z, p2.z));
        coll->particleList.push_back(p);
    }
}


#include "System.h"

void Ship::moveObject(double timeStep)
{
    float speed = 0.5f;
    if (EventManager::keyMap[GLFW_KEY_A])
        posicion.x -= speed * timeStep;
    if (EventManager::keyMap[GLFW_KEY_D])
        posicion.x += speed * timeStep;

    if (EventManager::keyMap[GLFW_KEY_SPACE])
    {
        Bullet* b = new Bullet("data/ship.frs");
        b->posicion = posicion;
        b->rotacion.z = 90;
        b->escalado = { 0.25f, 0.25f, 0.25f, 1.0f };
        b->direccion = { 0, 1, 0, 0 };
        System::render->addObject(b);
    }
}

void Bullet::moveObject(double timeStep)
{
    float speed = 5.0f;
    this->posicion = posicion + direccion * speed * timeStep;

    if (posicion.y > 10)
        this->active = false;

    auto collList = System::render->getCollisions(this, enemyType);
    if (direccion.y > 0 && collList.size() > 0)
        for (auto& o : collList)
            o->active = false;
}

void Enemy::moveObject(double timeStep)
{
    float speed = 1.0f;
    this->posicion.x += speed * timeStep;

    if (posicion.x > 5)
        this->posicion.x = -5;

    if ((std::rand() % 180) == 0)
    {
        Bullet* b = new Bullet("data/ship.frs");
        b->posicion = posicion;
        b->rotacion.z = 90;
        b->escalado = { 0.25f, 0.25f, 0.25f, 1.0f };
        b->direccion = { 0, -1, 0, 0 };
        System::render->addObject(b);
    }
}