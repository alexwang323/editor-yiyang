// [ginkgo-engine] mesh.h - mesh abstraction. Captures data arrays, VAO, geometry data.

#pragma once

#include "ginkgopch.h"

#include "transformcomponent.h"

#include <utility> // used for std::pair (mesh + matrix)

class Shader;

enum MESH_TYPE {
    MESH_TYPE_SPHERE = 0,
    MESH_TYPE_CUBE = 1,
    MESH_TYPE_CONE = 2,
    MESH_TYPE_PLANE = 3,
    MESH_TYPE_CYLINDER = 4,
    MESH_TYPE_FROM_FILE = 5,
    MESH_TYPE_CAPSULE = 6,
    MESH_TYPE_DISC = 7,
    MESH_TYPE_QUAD = 8,
    MESH_TYPE_MAX_ENUM = 0x7FFFFFFF
};

enum MESH_DRAW_MODE {
    MESH_DRAW_MODE_NORMAL = GL_TRIANGLES,
    MESH_DRAW_MODE_WIREFRAME = GL_LINE_LOOP,
    MESH_DRAW_MODE_MAX_ENUM = 0x7FFFFFFF
};

class Mesh {
public:
    Mesh() : 
        drawable(false),
        Pbuff(0),
        Nbuff(0),
        Tbuff(0),
        Dbuff(0),
        Ibuff(0)
    {};

    ~Mesh() {
        glDeleteVertexArrays(1, &vaoID);
        glDeleteBuffers(1, &Pbuff);
        glDeleteBuffers(1, &Nbuff);
        glDeleteBuffers(1, &Tbuff);
        glDeleteBuffers(1, &Dbuff);
        glDeleteBuffers(1, &Ibuff);
    }

    void MakeVAO();
    void DrawVAO(MESH_DRAW_MODE mode);

    void Draw(MESH_DRAW_MODE mode, Shader* shader, glm::mat4 transform);

    void SetVisibility(bool visibility) { drawable = visibility; }
    bool Drawable() const { return drawable; }

    void pushquad(int i, int j, int k, int l);
    unsigned int VaoFromTris();

protected:
    unsigned int vaoID = -1;
    bool drawable;
    GLuint Pbuff, Nbuff, Tbuff, Dbuff, Ibuff;

    std::vector<glm::vec4> Pnt;
    std::vector<glm::vec3> Nrm;
    std::vector<glm::vec2> Tex;
    std::vector<glm::vec3> Tan;

    std::vector<glm::ivec3> Tris;
    unsigned int triCount = 0;
};