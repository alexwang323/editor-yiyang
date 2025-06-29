#include "ginkgopch.h"
#include "mesh.h"
#include "shader.h"

void Mesh::pushquad(int i, int j, int k, int l) {
    Tris.push_back(glm::ivec3(i, j, k));
    Tris.push_back(glm::ivec3(i, k, l));
}

unsigned int Mesh::VaoFromTris() {
    //printf("VaoFromTris %ld %ld\n", Pnt.size(), Tris.size());
    unsigned int vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    //GLuint Pbuff;
    glGenBuffers(1, &Pbuff);
    glBindBuffer(GL_ARRAY_BUFFER, Pbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * Pnt.size(),
        &Pnt[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (Nrm.size() > 0) {
        //GLuint Nbuff;
        glGenBuffers(1, &Nbuff);
        glBindBuffer(GL_ARRAY_BUFFER, Nbuff);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * Nrm.size(),
            &Nrm[0][0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (Tex.size() > 0) {
        //GLuint Tbuff;
        glGenBuffers(1, &Tbuff);
        glBindBuffer(GL_ARRAY_BUFFER, Tbuff);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * Tex.size(),
            &Tex[0][0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (Tan.size() > 0) {
        //GLuint Dbuff;
        glGenBuffers(1, &Dbuff);
        glBindBuffer(GL_ARRAY_BUFFER, Dbuff);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * Tan.size(),
            &Tan[0][0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //GLuint Ibuff;
    glGenBuffers(1, &Ibuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ibuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 3 * Tris.size(),
        &Tris[0][0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    // resizing to 0 clears capacity - saves space.
    // These are only stored to unify the VAO call
    Pnt.resize(0);
    Nrm.resize(0);
    Tex.resize(0);
    Tan.resize(0);

    drawable = true;
    return vaoID;
}

void Mesh::MakeVAO()
{
    vaoID = VaoFromTris();
    triCount = (unsigned int)Tris.size();
}

void Mesh::DrawVAO(MESH_DRAW_MODE mode)
{
    glBindVertexArray(vaoID);
    if (mode != MESH_DRAW_MODE_MAX_ENUM)
        glDrawElements(mode, 3 * triCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::Draw(MESH_DRAW_MODE mode, Shader* shader, glm::mat4 transform) {
    int loc = glGetUniformLocation(shader->programId, "u_model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &transform[0][0]);

    glm::mat4 inv = glm::inverse(transform);
    loc = glGetUniformLocation(shader->programId, "u_normaltransform");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &inv[0][0]);

    loc = glGetUniformLocation(shader->programId, "u_precalctangents");
    glUniform1i(loc, Tan.size() != 0 ? 1 : 0);

    if (drawable) {
        DrawVAO(mode);
    }
}