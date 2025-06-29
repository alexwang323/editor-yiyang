#include "ginkgopch.h"
#include "material.h"
#include "shader.h"
#include "AssetManager.h"

// Note: shader MUST be in use before this call.
void Material::BindMaterial(Shader* shader) {
	this->shader = shader;
	int loc = glGetUniformLocation(shader->programId, "u_diffuse");
	glUniform3fv(loc, 1, &diffuse[0]);
	loc = glGetUniformLocation(shader->programId, "u_specular");
	glUniform3fv(loc, 1, &specular[0]);
	loc = glGetUniformLocation(shader->programId, "u_roughness");
	glUniform1f(loc, roughness);

	loc = glGetUniformLocation(shader->programId, "u_usingalbedo");
	if (hasTexture[0]) {
		glUniform1i(loc, 1);
		BindTexture(0, shader, "s_albedo");
	} else {
		glUniform1i(loc, 0);
	}

	loc = glGetUniformLocation(shader->programId, "u_usingnormal");
	if (hasTexture[1]) {
		glUniform1i(loc, 1);
		BindTexture(1, shader, "s_normal");
	} else {
		glUniform1i(loc, 0);
	}

	loc = glGetUniformLocation(shader->programId, "u_usingspecular");
	if (hasTexture[2]) {
		glUniform1i(loc, 1);
		BindTexture(2, shader, "s_specular");
	} else {
		glUniform1i(loc, 0);
	}

	loc = glGetUniformLocation(shader->programId, "u_usingroughness");
	if (hasTexture[3]) {
		glUniform1i(loc, 1);
		BindTexture(3, shader, "s_roughness");
	}
	else {
		glUniform1i(loc, 0);
	}
}

// unbinds all textures bound to the current shader.
void Material::UnbindMaterial() {
	// when we have multiple textures on a material, this will have to iterate.
	// also note: we will need to have material alignment.
	
	for (int i = 0; i < hasTexture.size(); ++i) {
		if (hasTexture[i])
			UnbindTexture(i);
	}
}

void Material::LoadTexture(MATERIAL_BIT mapType, const std::string& textureName) {
	std::cout << "start using the textures" << std::endl;
	AssetManager& assetManager = AssetManager::GetInstance();
	GLuint textureId = assetManager.RetrieveTextureID(textureName);
	switch (mapType) {
	case MATERIAL_BIT::ALBEDO_BIT:
		albedoID = textureId;
		textureFiles[0] = textureName;
		break;
	case MATERIAL_BIT::NORMAL_BIT:
		normalID = textureId;
		textureFiles[1] = textureName;
		break;
	case MATERIAL_BIT::SPECULAR_BIT:
		specularID = textureId;
		textureFiles[2] = textureName;
		break;
	case MATERIAL_BIT::ROUGHNESS_BIT:
		roughnessID = textureId;
		textureFiles[3] = textureName;
		break;
	}
	hasTexture |= mapType;
}

void Material::LoadExistingTexture(MATERIAL_BIT mapType, const GLuint textureId) {
	switch (mapType) {
	case MATERIAL_BIT::ALBEDO_BIT:
		albedoID = textureId;
		break;
	case MATERIAL_BIT::NORMAL_BIT:
		normalID = textureId;
		break;
	case MATERIAL_BIT::SPECULAR_BIT:
		specularID = textureId;
		break;
	case MATERIAL_BIT::ROUGHNESS_BIT:
		roughnessID = textureId;
		break;
	}
	hasTexture |= mapType;
}

void Material::BindTexture(const int unit, Shader* shader, const std::string& uniformName) {
	glActiveTexture(GL_TEXTURE0 + unit);
		
	switch (unit) {
	case 0:
		glBindTexture(GL_TEXTURE_2D, albedoID);
		break;
	case 1:
		glBindTexture(GL_TEXTURE_2D, normalID);
		break;
	case 2:
		glBindTexture(GL_TEXTURE_2D, specularID);
		break;
	case 3:
		glBindTexture(GL_TEXTURE_2D, roughnessID);
		break;
	}
	int loc = glGetUniformLocation(shader->programId, uniformName.c_str());
	glUniform1i(loc, unit);
}

void Material::UnbindTexture(const int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::CreateMaterialToLuaBind(std::shared_ptr<sol::state> lua) {

	lua->new_enum("MATERIAL_BIT",
		"ALBEDO_BIT", MATERIAL_BIT::ALBEDO_BIT,
		"NORMAL_BIT", MATERIAL_BIT::NORMAL_BIT,
		"SPECULAR_BIT", MATERIAL_BIT::SPECULAR_BIT,
		"ROUGHNESS_BIT", MATERIAL_BIT::ROUGHNESS_BIT
		);

	lua->new_usertype<Material>("Material",
		sol::constructors<Material(),
		Material(glm::vec3 _diffuse, glm::vec3 _specular, float _shininess)>(),
		"LoadTexture", &Material::LoadTexture,
		"LoadExistingTexture", &Material::LoadExistingTexture,
		"diffuse", &Material::diffuse,
		"specular", &Material::specular,
		"roughness", &Material::roughness,
		"hasTexture", &Material::hasTexture
	);
}
