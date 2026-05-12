#include "Object3D.h"
#include "EventManager.h"

static unsigned int nextObjectId = 1;

Object3D::Object3D()
{
    posicion = make_vector4f(0, 0, 0, 1);
    rotacion = make_vector4f(0, 0, 0, 0);
    escala = make_vector4f(1, 1, 1, 0);
    modelMatrix = make_identity();
    mat = nullptr;

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
    if (mat != nullptr)
    {
        delete mat;
        mat = nullptr;
    }
}

void Object3D::createTriangle()
{
    objId = nextObjectId++;

    vertexList = {
        vertex_t{ make_vector4f(0.0f, 0.5f, 0.0f, 1.0f), make_vector4f(1.0f, 0.0f, 0.0f, 1.0f), make_vector3f(0.0f, 0.0f, 1.0f), make_vector2f(0.5f, 1.0f) },
        vertex_t{ make_vector4f(0.5f, -0.5f, 0.0f, 1.0f), make_vector4f(0.0f, 1.0f, 0.0f, 1.0f), make_vector3f(0.0f, 0.0f, 1.0f), make_vector2f(1.0f, 0.0f) },
        vertex_t{ make_vector4f(-0.5f, -0.5f, 0.0f, 1.0f), make_vector4f(0.0f, 0.0f, 1.0f, 1.0f), make_vector3f(0.0f, 0.0f, 1.0f), make_vector2f(0.0f, 0.0f) }
    };

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

    string vertexShaderFile;
    string fragmentShaderFile;
    string attrPos;
    string attrColor;
    string attrNormal;
    string attrTexCoord;
    string uniformMVP;
    string textureFile;
    float matKa = 0.3f, matKd = 0.8f, matKs = 0.5f;
    int matShiny = 32;

    while (getline(file, line))
    {
        line = trimLine(line);

        if (line.empty() || line[0] == '#' || line[0] == '�') continue;

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
        else if (line.find("attributeVertNormal=") == 0)
            attrNormal = line.substr(21);
        else if (line.find("attributeVertTexCoord=") == 0)
            attrTexCoord = line.substr(23);
        else if (line.find("uniformMVP=") == 0)
            uniformMVP = line.substr(11);
        else if (line.find("textureFile=") == 0)
            textureFile = line.substr(12);
        else if (line.find("materialKa=") == 0)
            matKa = stof(line.substr(12));
        else if (line.find("materialKd=") == 0)
            matKd = stof(line.substr(12));
        else if (line.find("materialKs=") == 0)
            matKs = stof(line.substr(12));
        else if (line.find("materialShiny=") == 0)
            matShiny = stoi(line.substr(15));
    }

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
    attrNormalName = attrNormal;
    attrTexCoordName = attrTexCoord;
    uniformMVPName = uniformMVP;

    if (!textureFile.empty())
    {
        mat = new Material(textureFile, matKa, matKd, matKs);
        mat->shiny = matShiny;
    }

    while (line.empty() || line[0] == '#' || line[0] == '�')
        getline(file, line);

    int numVertices = stoi(line);

    while ((int)vertexList.size() < numVertices && getline(file, line))
    {
        line = trimLine(line);

        if (line.empty() || line[0] == '#' || line[0] == '�') continue;

        auto data = splitString<float>(line, ' ');
        if (data.size() < 7) continue;

        vertex_t v;
        v.posicion = make_vector4f(data[0], data[1], data[2], 1.0f);
        v.color = make_vector4f(data[3], data[4], data[5], data[6]);
        v.normal = make_vector3f(0.0f, 0.0f, 1.0f);
        v.texCoord = make_vector2f(0.0f, 0.0f);

        if (data.size() >= 9)
            v.normal = make_vector3f(data[7], data[8], data[9]);
        if (data.size() >= 11)
            v.texCoord = make_vector2f(data[10], data[11]);

        vertexList.push_back(v);
    }

    do
    {
        if (!getline(file, line))
        {
            cout << "ERROR: no se pudo leer el numero de caras\n";
            file.close();
            return;
        }

        line = trimLine(line);

    } while (line.empty() || line[0] == '#' || line[0] == '�');

    int numFaces = stoi(line);

    while ((int)idList.size() < numFaces * 3 && getline(file, line))
    {
        line = trimLine(line);

        if (line.empty() || line[0] == '#' || line[0] == '�') continue;

        auto data = splitString<int>(line, ' ');
        if (data.size() < 3) continue;

        idList.push_back(data[0]);
        idList.push_back(data[1]);
        idList.push_back(data[2]);
    }

    file.close();

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
    Matriz4x4f T = make_translate(posicion.x, posicion.y, posicion.z);
    Matriz4x4f R = make_rotate(rotacion.x, rotacion.y, rotacion.z);
    Matriz4x4f S = make_scale(escala.x, escala.y, escala.z);

    modelMatrix = T * R * S;
}