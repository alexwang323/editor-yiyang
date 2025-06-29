// [ginkgo-engine] material.h : captures lighting parameters

#pragma once

#include "ginkgopch.h"
#include <sol/sol.hpp>
#include <bitset>

enum MATERIAL_BIT {
	ALBEDO_BIT = 0b0001,
	NORMAL_BIT = 0b0010,
	SPECULAR_BIT = 0b0100,
	ROUGHNESS_BIT = 0b1000
};

class Shader;

class Material {

	friend class SceneGraphNode;
	friend class AssetManager;

public:
	Material() :
		diffuse(glm::vec3(1.0f)), specular(glm::vec3(0.01f)), roughness(0.5f) {
	};

	Material(glm::vec3 _diffuse, glm::vec3 _specular, float _roughness) :
		diffuse(_diffuse), specular(_specular), roughness(_roughness)
	{};

	void BindMaterial(Shader* shader);
	void UnbindMaterial();

	static void CreateMaterialToLuaBind(std::shared_ptr<sol::state> lua);
	void LoadTexture(MATERIAL_BIT mapType, const std::string& textureName);
	void LoadExistingTexture(MATERIAL_BIT mapType, const GLuint	textureId);

	// Getters
	glm::vec3 GetDiffuse() const { return diffuse; }
	glm::vec3 GetSpecular() const { return specular; }
	float GetRoughness() const { return roughness; }
	std::bitset<4> HasTexture() const { return hasTexture; }

	// Setters
	void SetDiffuse(const glm::vec3& color) { diffuse = color; }
	void SetSpecular(const glm::vec3& color) { specular = color; }
	void SetRoughness(float roughnessValue) { roughness = roughnessValue; }
	void SetHasTexture(std::bitset<4> useTexture) { hasTexture = useTexture; }

	Shader* GetShader() const { return shader; }
	void SetShader(Shader* _shader) { shader = _shader; }


protected:
	Shader* shader = nullptr;

	glm::vec3 diffuse;
	glm::vec3 specular;
	float roughness; // valid values are in range [0.05f, 1.0f]

	// change to bitmap of textures?
	std::bitset<4> hasTexture;
	std::array<std::string, 4> textureFiles;

	unsigned int albedoID = 0;
	unsigned int normalID = 0;
	unsigned int specularID = 0;
	unsigned int roughnessID = 0;

private:
	void BindTexture(const int unit, Shader* shader, const std::string& uniformName);
	void UnbindTexture(const int unit);
};
