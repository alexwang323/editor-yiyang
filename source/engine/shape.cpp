#include "ginkgopch.h"
#include "shape.h"
#include "renderingconstants.h"

Sphere::Sphere(const unsigned int subdivisions) {
    // 541 sphere
    //float d = 2.0f * PI / float(subdivisions * 2);
    //for (int i = 0; i <= subdivisions * 2; i++) {
    //    float s = i * 2.0f * PI / float(subdivisions * 2);
    //    for (int j = 0; j <= subdivisions; j++) {
    //        float t = j * PI / float(subdivisions);
    //        float x = cos(s) * sin(t);
    //        float y = sin(s) * sin(t);
    //        float z = cos(t);
    //        Pnt.push_back(glm::vec4(x, y, z, 1.0f));
    //        Nrm.push_back(glm::vec3(x, y, z));
    //        Tex.push_back(glm::vec2(s / (2 * PI), t / PI));
    //        Tan.push_back(glm::vec3(-sin(s), cos(s), 0.0));
    //        if (i > 0 && j > 0) {
    //            pushquad((i - 1) * (subdivisions + 1) + (j - 1),
    //                (i - 1) * (subdivisions + 1) + (j),
    //                (i) * (subdivisions + 1) + (j),
    //                (i) * (subdivisions + 1) + (j - 1));
    //        }
    //    }
    //}

    // LearnOpenGL sphere
    for (unsigned int x = 0; x <= subdivisions; ++x) {
        for (unsigned int y = 0; y <= subdivisions; ++y) {
            float xSegment = (float)x / (float)subdivisions;
            float ySegment = (float)y / (float)subdivisions;
            float xPos = glm::cos(xSegment * 2.0f * PI) * glm::sin(ySegment * PI);
            float yPos = glm::cos(ySegment * PI);
            float zPos = glm::sin(xSegment * 2.0f * PI) * glm::sin(ySegment * PI);

            Pnt.push_back(glm::vec4(xPos, yPos, zPos, 1.0f));
            Nrm.push_back(glm::vec3(xPos, yPos, zPos));
            Tex.push_back(glm::vec2(xSegment, ySegment));
            
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < subdivisions; ++y) {
        if (!oddRow) {
            for (unsigned int x = 0; x <= subdivisions; ++x) {
                pushquad(
                    y * (subdivisions + 1) + x,
                    (y + 1) * (subdivisions + 1) + x,
                    (y + 1) * (subdivisions + 1) + (x + 1),
                    y * (subdivisions + 1) + (x + 1)
                );
            }
        } else {
            for (int x = subdivisions; x > 0; --x) {
                pushquad(
                    (y + 1) * (subdivisions + 1) + x,
                    y * (subdivisions + 1) + x,
                    y * (subdivisions + 1) + (x - 1),
                    (y + 1) * (subdivisions + 1) + (x - 1)
                );
            }
        }
        oddRow = !oddRow;
    }

    MakeVAO();
}

Box::Box() {
    glm::mat4 I(1.0f);

    // Six faces, each a rotation of a rectangle placed on the z axis.
    Face(I);
    float r90 = PI / 2;
    Face(glm::rotate(I, r90, glm::vec3(1.0f, 0.0f, 0.0f)));
    Face(glm::rotate(I, -r90, glm::vec3(1.0f, 0.0f, 0.0f)));
    Face(glm::rotate(I, r90, glm::vec3(0.0f, 1.0f, 0.0f)));
    Face(glm::rotate(I, -r90, glm::vec3(0.0f, 1.0f, 0.0f)));
    Face(glm::rotate(I, PI, glm::vec3(1.0f, 0.0f, 0.0f)));

    MakeVAO();
}

void Box::Face(const glm::mat4 side) {
    int n = (int)Pnt.size();

    float verts[8] = { 1.0f,1.0f, -1.0f,1.0f, -1.0f,-1.0f, 1.0f,-1.0f };
    float texcd[8] = { 1.0f,1.0f,  0.0f,1.0f,  0.0f, 0.0f, 1.0f, 0.0f };

    // Four vertices to make a single face, with its own normal and
    // texture coordinates.
    for (int i = 0; i < 8; i += 2) {
        Pnt.push_back(side * glm::vec4(verts[i], verts[i + 1], 1.0f, 1.0f));
        Nrm.push_back(glm::vec3(side * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
        Tex.push_back(glm::vec2(texcd[i], texcd[i + 1]));
        Tan.push_back(glm::vec3(side * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
    }

    pushquad(n, n + 1, n + 2, n + 3);
}

Cone::Cone(const unsigned int subdivisions) {

    glm::vec3 top(0.0f, 2.0f, 0.0f);
    glm::vec3 base(0.0f, 0.0f, 0.0f);


    float angleStep = 2 * PI / subdivisions;

    // Generate vertices for the cone's base
    std::vector<int> verts{ 0,1,2,3,4,5 };
    for (unsigned int i = 0; i < 2*subdivisions; ++i) {
        float angle = i * angleStep;
        float nextAngle = (i + 1) * angleStep;

        float radius = 1.0f;
        glm::vec3 p1(radius * cos(angle), 0.0f, radius * sin(angle));
        glm::vec3 p2(radius * cos(nextAngle), 0.0f, radius * sin(nextAngle));

        glm::vec3 n1 = glm::normalize(glm::cross(p2 - top, p1 - top));
        glm::vec3 n2 = glm::normalize(glm::cross(p1 - base, p2 - base));

        Pnt.push_back(glm::vec4(top, 1.0f));
        Nrm.push_back(n1);
        Pnt.push_back(glm::vec4(p1, 1.0f));
        Nrm.push_back(n1);
        Pnt.push_back(glm::vec4(p2, 1.0f));
        Nrm.push_back(n1);

        Tris.push_back(glm::ivec3(verts[0], verts[1], verts[2]));

        Pnt.push_back(glm::vec4(base, 1.0f));
        Nrm.push_back(n2);
        Pnt.push_back(glm::vec4(p1, 1.0f));
        Nrm.push_back(n2);
        Pnt.push_back(glm::vec4(p2, 1.0f));
        Nrm.push_back(n2);

        Tris.push_back(glm::ivec3(verts[3], verts[4], verts[5]));

        for (int j = 0; j < verts.size(); ++j) {
            verts[j] += 6;
        }
    }

    MakeVAO();
}

Plane::Plane(const unsigned int subdivisions) {
    float r = 1.0f;
    for (unsigned int i = 0; i <= subdivisions; i++) {
        float s = i / float(subdivisions);
        for (unsigned int j = 0; j <= subdivisions; j++) {
            float t = j / float(subdivisions);
            Pnt.push_back(glm::vec4(s * 2.0f * r - r, t * 2.0f * r - r, 0.0f, 1.0f));
            Nrm.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
            Tex.push_back(glm::vec2(s, t));
            Tan.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
            if (i > 0 && j > 0) {
                pushquad((i - 1) * (subdivisions + 1) + (j - 1), //i
                    (i - 1) * (subdivisions + 1) + (j), //j
                    (i) * (subdivisions + 1) + (j), //k
                    (i) * (subdivisions + 1) + (j - 1)); //l
            }
        }
    }

    MakeVAO();
}

Cylinder::Cylinder(const unsigned int subdivisions, float height)
{
    //so we account for +2 subdivisions?
    for (unsigned int x = 0; x <= subdivisions; ++x)
    {
        float xSegment = (float)x / (float)subdivisions;
        float xPos = glm::cos(xSegment * 2.0f * PI);
        float zPos = glm::sin(xSegment * 2.0f * PI);
        for (unsigned int y = 0; y <= subdivisions; ++y)
        {
            
            float ySegment = (float)y / (float)subdivisions;

            //float 
            
            float yPos = height*0.5f*glm::cos(ySegment * PI);
            float nDirY = (yPos < 0.0f) ? -1.0f : 1.0f;

            
            if ((y <= 1) || (y >= (subdivisions - 1)))
            {
                Pnt.push_back(glm::vec4(0.0f, yPos, 0.0f, 1.0f));
                Nrm.push_back(glm::normalize(glm::vec3(0.0f, nDirY, 0.0f)));
            }
            else
            {
                Pnt.push_back(glm::vec4(xPos, yPos, zPos, 1.0f));
                Nrm.push_back(glm::normalize(glm::vec3(xPos, 0.0f, yPos)));
            }
            Tex.push_back(glm::vec2(xSegment, ySegment));
        }

    }

    bool oddRow = false;
    for (unsigned int y = 0; y < subdivisions; ++y) {
        if (!oddRow) {
            for (unsigned int x = 0; x <= subdivisions; ++x) {
                pushquad(
                    y * (subdivisions + 1) + x,
                    (y + 1) * (subdivisions + 1) + x,
                    (y + 1) * (subdivisions + 1) + (x + 1),
                    y * (subdivisions + 1) + (x + 1)
                );
            }
        }
        else {
            for (int x = subdivisions; x > 0; --x) {
                pushquad(
                    (y + 1) * (subdivisions + 1) + x,
                    y * (subdivisions + 1) + x,
                    y * (subdivisions + 1) + (x - 1),
                    (y + 1) * (subdivisions + 1) + (x - 1)
                );
            }
        }
        oddRow = !oddRow;
    }
    MakeVAO();
}


AssimpModel::AssimpModel(AssetManager::ModelOutput& modelData) {
    Pnt = modelData.m_Positions;
    Nrm = modelData.m_Normals;
    Tex = modelData.m_TexCoords;
    Tan = modelData.m_Tangents;
    Tris = modelData.m_Triangles;
    triCount = (unsigned int)modelData.m_Triangles.size();
    MakeVAO();
}

Capsule::Capsule(const unsigned int subdivisions, float height, float radius)
{
    //assumption radius < height
    const float heightoff = height;
    for (unsigned int x = 0; x <= subdivisions; ++x) {
        for (unsigned int y = 0; y <= subdivisions; ++y) {
            float xSegment = (float)x / (float)subdivisions;
            float ySegment = (float)y / (float)subdivisions;
            float xPos = glm::cos(xSegment * 2.0f * PI) * glm::sin(ySegment * PI);
            float yPos = radius*glm::cos(ySegment * PI);
            float yNom = glm::cos(ySegment * PI);
            yPos += (yPos < 0)? -heightoff: heightoff;
            float zPos = glm::sin(xSegment * 2.0f * PI) * glm::sin(ySegment * PI);

            Pnt.push_back(glm::vec4(radius * xPos, yPos, radius * zPos, 1.0f));
            Nrm.push_back(glm::vec3(xPos, yNom, zPos));
            Tex.push_back(glm::vec2(xSegment, ySegment));

        }
    }
    
    bool oddRow = false;
    for (unsigned int y = 0; y < subdivisions; ++y) {
        if (!oddRow) {
            for (unsigned int x = 0; x <= subdivisions; ++x) {
                pushquad(
                    y * (subdivisions + 1) + x,
                    (y + 1) * (subdivisions + 1) + x,
                    (y + 1) * (subdivisions + 1) + (x + 1),
                    y * (subdivisions + 1) + (x + 1)
                );
            }
        }
        else {
            for (unsigned int x = subdivisions; x > 0; --x) {
                pushquad(
                    (y + 1) * (subdivisions + 1) + x,
                    y * (subdivisions + 1) + x,
                    y * (subdivisions + 1) + (x - 1),
                    (y + 1) * (subdivisions + 1) + (x - 1)
                );
            }
        }
        oddRow = !oddRow;
    }
    MakeVAO();
}

Disc::Disc(const unsigned int subdivisions)
{
    for (unsigned int x = 0; x <= subdivisions; ++x) {
        for (unsigned int y = 0; y <= subdivisions; ++y) {
            float xSegment = (float)x / (float)subdivisions;
            float ySegment = (float)y / (float)subdivisions;
            float xPos = glm::cos(xSegment * 2.0f * PI) * glm::sin(ySegment * PI);
            float yPos = (glm::cos(ySegment * PI) < 0) ? -1.0f : 1.0f;
            float zPos = glm::sin(xSegment * 2.0f * PI) * glm::sin(ySegment * PI);

            Pnt.push_back(glm::vec4(xPos, 0.0f, zPos, 1.0f));
            Nrm.push_back(glm::vec3(0.0f,yPos,0.0f));
            Tex.push_back(glm::vec2(xSegment, ySegment));

        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < subdivisions; ++y) {
        if (!oddRow) {
            for (unsigned int x = 0; x <= subdivisions; ++x) {
                pushquad(
                    y * (subdivisions + 1) + x,
                    (y + 1) * (subdivisions + 1) + x,
                    (y + 1) * (subdivisions + 1) + (x + 1),
                    y * (subdivisions + 1) + (x + 1)
                );
            }
        }
        else {
            for (unsigned int x = subdivisions; x > 0; --x) {
                pushquad(
                    (y + 1) * (subdivisions + 1) + x,
                    y * (subdivisions + 1) + x,
                    y * (subdivisions + 1) + (x - 1),
                    (y + 1) * (subdivisions + 1) + (x - 1)
                );
            }
        }
        oddRow = !oddRow;
    }
    MakeVAO();
}

Quad::Quad() {
    
    // yes, these numbers are magic. but quads are magic too.
    Pnt.push_back(glm::vec4(-1.0f,  1.0f, 0.0f, 1.0f));
    Pnt.push_back(glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f));
    Pnt.push_back(glm::vec4( 1.0f,  1.0f, 0.0f, 1.0f));
    Pnt.push_back(glm::vec4( 1.0f, -1.0f, 0.0f, 1.0f));

    Nrm.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    Nrm.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    Nrm.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    Nrm.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

    Tex.push_back(glm::vec2(0.0f, 1.0f));
    Tex.push_back(glm::vec2(0.0f, 0.0f));
    Tex.push_back(glm::vec2(1.0f, 1.0f));
    Tex.push_back(glm::vec2(1.0f, 0.0f));

    // makequad does this wrong, recreating quad.
    Tris.push_back(glm::ivec3(0, 1, 2));
    Tris.push_back(glm::ivec3(1, 2, 3));
    MakeVAO();
}
