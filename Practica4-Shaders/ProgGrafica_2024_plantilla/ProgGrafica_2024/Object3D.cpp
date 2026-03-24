#include "Object3D.h"
#include "EventManager.h"

// Generador simple de IDs único por proceso
static unsigned int nextObjectId = 1;

Object3D::Object3D()
{
    // Inicializar transformaciones por defecto
    posicion = make_vector4f(0, 0, 0, 1);
    rotacion = make_vector4f(0, 0, 0, 0);
    escala = make_vector4f(1, 1, 1, 0);
    modelMatrix = make_identity();

    // Crear programa de shaders por defecto para este objeto
    prg = new Program();
    prg->addShader("data/program.vertex");
    prg->addShader("data/program.fragment");
    prg->linkProgram();
    prg->readVarList();
}

Object3D::~Object3D()
{
    if (prg != nullptr)
    {
        delete prg;
        prg = nullptr;
    }
}

void Object3D::createTriangle()
{
    // Asignar ID único
    objId = nextObjectId++;

    // Inicializar vértices explícitamente
    vertexList = {
        vertex_t{ make_vector4f(0.0f, 0.5f, 0.0f, 1.0f) },	// vértice superior
        vertex_t{ make_vector4f(0.5f, -0.5f, 0.0f, 1.0f) },	// vértice inferior derecho
        vertex_t{ make_vector4f(-0.5f, -0.5f, 0.0f, 1.0f) }	// vértice inferior izq
    };

    // Indices en base 0
    idList = { 0, 1, 2 };
}


static string trimLine(const string& s)
{
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start])) start++;

    size_t end = s.size();
    while (end > start && isspace((unsigned char)s[end - 1])) end--;

    return s.substr(start, end - start);
}


static bool isUsefulDataLine(const string& line)
{
    string t = trimLine(line);
    if (t.empty()) return false;

    char c = t[0];
    if ((c >= '0' && c <= '9') || c == '-' || c == '.') return true;

    return false;
}

// Cargar fichero
void Object3D::loadFromFile(const char* fileName)
{
    objId = nextObjectId++;

    vertexList.clear();
    idList.clear();

    ifstream file(fileName);
    if (!file.is_open())
    {
        cout << "ERROR abriendo fichero: " << fileName << "\n";
        return;
    }

    string line;

    // ===== VARIABLES DE CABECERA =====
    string vertexShaderFile;
    string fragmentShaderFile;
    string attrPos;
    string attrColor;
    string uniformMVP;

    // ===== LEER CABECERA =====
    while (getline(file, line))
    {
        line = trimLine(line);

        if (line.empty() || line[0] == '#' || line[0] == '€') continue;

        // Si empieza por numero -> empieza geometria
        if ((line[0] >= '0' && line[0] <= '9') || line[0] == '-')
            break;

        if (line.find("vertexShaderFile=") == 0)
            vertexShaderFile = line.substr(17);

        else if (line.find("fragmentShaderFile=") == 0)
            fragmentShaderFile = line.substr(19);

        else if (line.find("attributeVertPos=") == 0)
            attrPos = line.substr(18);

        else if (line.find("attributeVertColor=") == 0)
            attrColor = line.substr(20);

        else if (line.find("uniformMVP=") == 0)
            uniformMVP = line.substr(11);
    }

    // ===== CREAR PROGRAMA SHADER =====
    if (prg != nullptr)
    {
        delete prg;
        prg = nullptr;
    }

    prg = new Program();
    prg->addShader(vertexShaderFile);
    prg->addShader(fragmentShaderFile);
    prg->linkProgram();
    prg->readVarList();

    attrPosName = attrPos;
    attrColorName = attrColor;
    uniformMVPName = uniformMVP;

    // ===== LEER NUMERO DE VERTICES =====
    while (line.empty() || line[0] == '#' || line[0] == '€')
        getline(file, line);

    int numVertices = stoi(line);

    // ===== LEER VERTICES =====
    while ((int)vertexList.size() < numVertices && getline(file, line))
    {
        line = trimLine(line);

        if (line.empty() || line[0] == '#' || line[0] == '€') continue;

        auto data = splitString<float>(line, ' ');
        if (data.size() < 7) continue;

        vertex_t v;

        // posicion
        v.posicion = make_vector4f(data[0], data[1], data[2], 1.0f);

        // color por vertice
        v.color = make_vector4f(data[3], data[4], data[5], data[6]);

        vertexList.push_back(v);
    }

    // ===== LEER NUMERO DE TRIANGULOS =====
    do
    {
        if (!getline(file, line))
        {
            cout << "ERROR: no se pudo leer el numero de caras\n";
            file.close();
            return;
        }

        line = trimLine(line);

    } while (line.empty() || line[0] == '#' || line[0] == '€');

    int numFaces = stoi(line);

    // ===== LEER INDICES =====
    while ((int)idList.size() < numFaces * 3 && getline(file, line))
    {
        line = trimLine(line);

        if (line.empty() || line[0] == '#' || line[0] == '€') continue;

        auto data = splitString<int>(line, ' ');
        if (data.size() < 3) continue;

        idList.push_back(data[0]);
        idList.push_back(data[1]);
        idList.push_back(data[2]);
    }

    file.close();

    // ===== DEBUG =====
    cout << "FRS cargado correctamente:\n";
    cout << "Vertices: " << vertexList.size() << "\n";
    cout << "Indices: " << idList.size() << "\n";
}

void Object3D::move(double timeStep)
{
    auto speed = 2.f;

    if (EventManager::keyMap[GLFW_KEY_D]) rotacion.y += speed * static_cast<float>(timeStep);
    if (EventManager::keyMap[GLFW_KEY_A]) rotacion.y -= speed * static_cast<float>(timeStep);

    updateModelMatrix();
}

void Object3D::updateModelMatrix()
{
    // update posicion
    Matriz4x4f T = make_translate(posicion.x, posicion.y, posicion.z);

    // update rotacion
    Matriz4x4f R = make_rotate(rotacion.x, rotacion.y, rotacion.z);

    // update escala
    Matriz4x4f S = make_scale(escala.x, escala.y, escala.z);

    modelMatrix = T * R * S;
}