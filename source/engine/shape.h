// [ginkgo-engine] shape.h : procedural mesh primitives, derivation of Mesh

#pragma once

#include "mesh.h"
#include "AssetManager.h"

const float PI = 3.14159f;
const float rad = PI / 180.0f;    // Convert degrees to radians

class Sphere : public Mesh {
public:
	Sphere(const unsigned int subdivisions);
};

class Box : public Mesh {
public:
	Box();

private:
	void Face(const glm::mat4 side);
};

class Cylinder : public Mesh {
public:
	Cylinder(const unsigned int subdivisions, float height);
};

class Cone : public Mesh {
public:
	Cone(const unsigned int subdivisions);
};

class Plane : public Mesh {
public:
	Plane(const unsigned int subdivisions);
};

class Capsule : public Mesh
{
public:
	Capsule(const unsigned int subdivisions, float height = 1.0f, float radius = 1.0f);
};

class Disc : public Mesh
{
public:
	Disc(const unsigned int subdivisions);
};

class AssimpModel : public Mesh {
public:
	AssimpModel(AssetManager::ModelOutput& modelData);
};

class Quad : public Mesh {
public:
	Quad();
};