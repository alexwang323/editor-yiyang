#pragma once
// need to include all the component header files here
#include <rapidjson/document.h>
#include "transformcomponent.h"
#include "rendercomponent.h"

class JSONmanager;

#define DESERIALIZE_COMPONENT(COMPONENT, serializer) ComponentSerializer::Deserialize<COMP>(serializer)

class ComponentSerializer {
public:
	ComponentSerializer() = delete; // all functions are static

	template <typename TComponent, typename TSerializer>
	static void Serialize(TSerializer& serializer, TComponent& component);

	template <typename TComponent, typename TTable>
	static auto Deserialize(const TTable& table);
private:
	// JSON serializer
	static void SerializeComponent(JSONmanager& serializer, TransformComponent& transform);
	// TODO: add serializer for other components
	// JSON deserializer
	static TransformComponent DeserializeTransform(const rapidjson::Value& jsonValue);
	static RenderComponent DeserializeRenderComp(const rapidjson::Value& jsonValue);
	// TODO: add deserializer for other components
};

#include "ComponentSerializer.inl"

