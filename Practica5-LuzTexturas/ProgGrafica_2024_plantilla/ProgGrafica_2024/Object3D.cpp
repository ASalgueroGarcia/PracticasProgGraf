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
    uniformMVPName = "MVP";

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

// Returns true for empty lines and comment lines (lines starting with '#' or
// any non-ASCII character such as the euro-sign comment marker used in FRS files).
static bool isFrsComment(const string& s)
{
    if (s.empty()) return true;
    unsigned char c = (unsigned char)s[0];
    return c == '#' || c > 127;
}

// Parse all whitespace-separated floats in a string.
// Handles values with trailing non-numeric suffixes like "1.0f".
static vector<float> parseFloats(const string& s)
{
    vector<float> r;
    istringstream ss(s);
    float v;
    while (ss >> v)
        r.push_back(v);
    return r;
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

    // Advance to the next non-comment, non-empty line.
    auto nextMeaningful = [&]() -> bool {
        while (getline(file, line)) {
            line = trimLine(line);
            if (!isFrsComment(line)) return true;
        }
        return false;
        };

    // ---- Vertex positions ----
    if (!nextMeaningful()) return;
    int nVerts = stoi(line);

    vertexList.resize(nVerts);
    for (auto& v : vertexList) {
        v.posicion = make_vector4f(0, 0, 0, 1);
        v.color = make_vector4f(0.8f, 0.8f, 0.8f, 1.0f);
        v.normal = make_vector3f(0, 0, 1);
        v.texCoord = make_vector2f(0, 0);
    }

    for (int i = 0; i < nVerts; ) {
        if (!getline(file, line)) break;
        line = trimLine(line);
        if (isFrsComment(line)) continue;
        auto d = parseFloats(line);
        if ((int)d.size() < 3) continue;
        vertexList[i].posicion = make_vector4f(d[0], d[1], d[2], 1.0f);
        i++;
    }

    // ---- Face indices ----
    if (!nextMeaningful()) {
        cout << "ERROR: no se pudo leer el numero de caras\n";
        file.close();
        return;
    }
    int nFaces = stoi(line);

    for (int i = 0; i < nFaces; ) {
        if (!getline(file, line)) break;
        line = trimLine(line);
        if (isFrsComment(line)) continue;
        auto d = parseFloats(line);
        if ((int)d.size() < 3) continue;
        idList.push_back((int)d[0]);
        idList.push_back((int)d[1]);
        idList.push_back((int)d[2]);
        i++;
    }

    // ---- Remaining sections: colors, normals, texcoords, file paths ----
    // FRS format stores these after the face data in this order:
    //   color groups:   "v0 v1 ...  r g b a"  (last 4 = rgba, rest = vertex IDs)
    //   normal groups:  "v0 v1 ...  nx ny nz 0"  (last 4 = normal+pad, rest = vertex IDs)
    //   texcoords:      "v u,v"  (space-separated id, comma-separated uv)
    //   file paths:     one path per line, no spaces
    string vsFile, fsFile, texFile;
    float matKa = 0.3f, matKd = 0.8f, matKs = 0.5f;
    int matShiny = 32;

    while (getline(file, line)) {
        line = trimLine(line);
        if (isFrsComment(line)) continue;

        // File paths have no spaces and end with a known extension.
        if (line.find(' ') == string::npos) {
            auto ends = [&](const string& suffix) {
                return line.size() >= suffix.size() &&
                    line.compare(line.size() - suffix.size(), suffix.size(), suffix) == 0;
                };
            if (ends(".vertex")) { vsFile = line; continue; }
            if (ends(".fragment")) { fsFile = line; continue; }
            if (ends(".png") || ends(".jpg") || ends(".bmp")) { texFile = line; continue; }
            // Pure count lines (single integer) fall here; skip them.
            continue;
        }

        // Texture-coordinate lines contain a comma: "id u,v"
        if (line.find(',') != string::npos) {
            istringstream ss(line);
            int id; string uv;
            if (ss >> id >> uv) {
                size_t comma = uv.find(',');
                if (comma != string::npos && id >= 0 && id < nVerts) {
                    try {
                        float u = stof(uv.substr(0, comma));
                        float v = stof(uv.substr(comma + 1));
                        vertexList[id].texCoord = make_vector2f(u, v);
                    }
                    catch (...) {}
                }
            }
            continue;
        }

        // Color/normal group lines: all numeric, last 4 values are the attribute,
        // preceding values are the vertex IDs that receive it.
        // Normals have a padding value of 0 as the 4th component ("0.0f" in file);
        // colors have alpha (typically 1.0) as the 4th component.
        auto d = parseFloats(line);
        if ((int)d.size() < 4) continue;

        float pad = d.back();
        float x = d[d.size() - 4];
        float y = d[d.size() - 3];
        float z = d[d.size() - 2];
        bool isNorm = fabsf(pad) < 0.01f;

        for (int i = 0; i < (int)d.size() - 4; i++) {
            int id = (int)d[i];
            if (id < 0 || id >= nVerts) continue;
            if (isNorm)
                vertexList[id].normal = make_vector3f(x, y, z);
            else
                vertexList[id].color = make_vector4f(x, y, z, pad);
        }
    }
    file.close();

    // ---- Shader program ----
    if (prg != nullptr) { delete prg; prg = nullptr; }
    prg = new Program();
    prg->addShader("data/program.vertex");
    prg->addShader("data/program.fragment");
    prg->linkProgram();
    prg->readVarList();

    uniformMVPName = "MVP";

    // ---- Material / texture ----
    if (mat != nullptr) { delete mat; mat = nullptr; }
    if (!texFile.empty()) {
        mat = new Material(texFile, matKa, matKd, matKs);
        mat->shiny = matShiny;
    }

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