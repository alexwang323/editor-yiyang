#include "ginkgopch.h"
#include "ComponentSerializer.h"
#include "jsonmanager.h"
#include <cstring>

void ComponentSerializer::SerializeComponent(JSONmanager& serializer, TransformComponent& transform) {
	serializer.StartNewObject("transform");
	serializer.StartNewArray("matrix_outer");
	glm::mat4 matrix = transform.getMatrix();
	serializer.StartNewArray("row0");
	serializer.AddKeyValuePair("", matrix[0][0]);
	serializer.AddKeyValuePair("", matrix[0][1]);
	serializer.AddKeyValuePair("", matrix[0][2]);
	serializer.AddKeyValuePair("", matrix[0][3]);
	serializer.EndArray();
	serializer.StartNewArray("row1");
	serializer.AddKeyValuePair("", matrix[1][0]);
	serializer.AddKeyValuePair("", matrix[1][1]);
	serializer.AddKeyValuePair("", matrix[1][2]);
	serializer.AddKeyValuePair("", matrix[1][3]);
	serializer.EndArray();
	serializer.StartNewArray("row2");
	serializer.AddKeyValuePair("", matrix[2][0]);
	serializer.AddKeyValuePair("", matrix[2][1]);
	serializer.AddKeyValuePair("", matrix[2][2]);
	serializer.AddKeyValuePair("", matrix[2][3]);
	serializer.EndArray();
	serializer.StartNewArray("row3");
	serializer.AddKeyValuePair("", matrix[3][0]);
	serializer.AddKeyValuePair("", matrix[3][1]);
	serializer.AddKeyValuePair("", matrix[3][2]);
	serializer.AddKeyValuePair("", matrix[3][3]);
	serializer.EndArray();
	serializer.EndArray();
	serializer.EndObject();
}

TransformComponent ComponentSerializer::DeserializeTransform(const rapidjson::Value& jsonValue) {
	return TransformComponent(
		glm::vec3{
			jsonValue["position"]["x"].GetFloat(),
			jsonValue["position"]["y"].GetFloat(),
			jsonValue["position"]["z"].GetFloat()
		},
		glm::vec3{
			jsonValue["rotation"]["x"].GetFloat(),
			jsonValue["rotation"]["y"].GetFloat(),
			jsonValue["rotation"]["z"].GetFloat()
		},
		glm::vec3{
			jsonValue["scale"]["x"].GetFloat(),
			jsonValue["scale"]["y"].GetFloat(),
			jsonValue["scale"]["z"].GetFloat()
		}
	);
}

RenderComponent ComponentSerializer::DeserializeRenderComp(const rapidjson::Value& jsonValue) {
	RenderComponent renderComp = RenderComponent();
	std::string meshType = std::string(jsonValue["meshType"].GetString());
	if (meshType == "SPHERE") {
		renderComp.Init(MESH_TYPE::MESH_TYPE_SPHERE);
	}
	else if (meshType == "CUBE") {
		renderComp.Init(MESH_TYPE::MESH_TYPE_CUBE);
	}
	else if (meshType == "CONE") {
		renderComp.Init(MESH_TYPE::MESH_TYPE_CONE);
	}
	else if (meshType == "PLANE") {
		renderComp.Init(MESH_TYPE::MESH_TYPE_PLANE);
	}
	else if (meshType == "CYLINDER") {
		renderComp.Init(MESH_TYPE::MESH_TYPE_CYLINDER);
	}
	else if (strncmp(jsonValue["meshType"].GetString(), "FILE", 4) == 0) {
		renderComp.Init(MESH_TYPE::MESH_TYPE_FROM_FILE);
	};
	Material mat = Material(
		glm::vec3{
			jsonValue["material"]["diffuse"]["r"].GetFloat(),
			jsonValue["material"]["diffuse"]["g"].GetFloat(),
			jsonValue["material"]["diffuse"]["b"].GetFloat()
		},
		glm::vec3{
			jsonValue["material"]["specular"]["r"].GetFloat(),
			jsonValue["material"]["specular"]["g"].GetFloat(),
			jsonValue["material"]["specular"]["b"].GetFloat()
		},
		jsonValue["material"]["roughness"].GetFloat()
	);
	renderComp.SetMaterial(mat);
	return renderComp;
}