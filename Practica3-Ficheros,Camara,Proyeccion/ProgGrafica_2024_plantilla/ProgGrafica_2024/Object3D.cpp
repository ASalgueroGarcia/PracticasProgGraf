#include "Object3D.h"
#include "EventManager.h"

// Generador simple de IDs único por proceso
static unsigned int nextObjectId = 1;

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

    vector<string> usefulLines;
    string line;

    while (getline(file, line))
    {
        if (isUsefulDataLine(line))
            usefulLines.push_back(trimLine(line));
    }

    file.close();

    if (usefulLines.size() < 3)
    {
        cout << "ERROR: formato .frs invalido\n";
        return;
    }

    int currentLine = 0;

    // ===== VERTICES =====
    int numVertices = stoi(usefulLines[currentLine++]);

    vector<vertex_t> baseVertices;

    for (int i = 0; i < numVertices; i++)
    {
        auto data = splitString<float>(usefulLines[currentLine++], ' ');
        if (data.size() < 3) continue;

        vertex_t v;

        v.posicion = make_vector4f(data[0], data[1], data[2], 1.0f);
        v.color = make_vector4f(1, 1, 1, 1);

        baseVertices.push_back(v);
    }

    // ===== CARAS =====
    int numFaces = stoi(usefulLines[currentLine++]);

    vector<int> faceIndices;

    for (int i = 0; i < numFaces; i++)
    {
        auto data = splitString<int>(usefulLines[currentLine++], ' ');
        if (data.size() < 3) continue;

        faceIndices.push_back(data[0]);
        faceIndices.push_back(data[1]);
        faceIndices.push_back(data[2]);
    }

    // ===== COLORES =====
    int numColors = stoi(usefulLines[currentLine++]);

    for (int i = 0; i < numColors; i++)
    {
        auto data = splitString<float>(usefulLines[currentLine++], ' ');
        if (data.size() < 7) continue;

        int i0 = (int)data[0];
        int i1 = (int)data[1];
        int i2 = (int)data[2];

        Vector4f color = make_vector4f(data[3], data[4], data[5], data[6]);

        vertex_t v0 = baseVertices[i0];
        vertex_t v1 = baseVertices[i1];
        vertex_t v2 = baseVertices[i2];

        v0.color = color;
        v1.color = color;
        v2.color = color;

        int base = vertexList.size();

        vertexList.push_back(v0);
        vertexList.push_back(v1);
        vertexList.push_back(v2);

        idList.push_back(base);
        idList.push_back(base + 1);
        idList.push_back(base + 2);
    }
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