#include "ginkgopch.h"
#include "GameObjectFactory.h"
#include "ComponentSerializer.h"
#include "jsonmanager.h"
#include "BehaviorTreeSystem.h"
#include "scene.h"

GameObjectFactory& GOF = GameObjectFactory::GetInstance();

GameObjectFactory::GameObjectFactory() {
	JSONmanager& JSON_MANAGER = JSONmanager::GetInstance();
	JSON_MANAGER.Init("Start.json");
}

GameObjectFactory& GameObjectFactory::GetInstance() {
	static GameObjectFactory instance;
	return instance;
}

GameObject GameObjectFactory::makeGameObjectArchetype()
{
	JSONmanager& JSON_MANAGAER = JSONmanager::GetInstance();
	auto game_objects = JSON_MANAGAER.findValue("GameObjects");
	if (game_objects) {
		const rapidjson::Value& game_object_data = game_objects->get();
		assert(game_object_data.IsArray() && "Game Object data in json not formatted as an array");
		// just create a game object with the first entry in game object data
		const rapidjson::Value& go_0 = game_object_data[0];
		TransformComponent transform = ComponentSerializer::Deserialize<TransformComponent>(go_0["Transform"]);
		RenderComponent renderComp = ComponentSerializer::Deserialize<RenderComponent>(go_0["Render"]);
		renderComp.SetTransform(transform);
		assert("game object name is not a string" && go_0["Name"].IsString());
		assert("parent name is not a string" && go_0["Parent"].IsString());
		const std::string& go_name = go_0["Name"].GetString();
		const std::string& parent_name = go_0["Parent"].GetString();
		GameObject* go = new GameObject(++nextID, go_name);
		go_memory_manager.push_back(go);
		go->SetParentName(parent_name);
		go->AddTransfromComponent(transform);
		go->AddRenderComponent(renderComp);

		// register the game object and render component
		Scene::GetInstance().RegisterGameObject(go);
		Scene::GetInstance().RegisterRenderComponent(renderComp.GetID(), go->GetID());

		addGameObject(*go); // not sure if this is safe
		return *go;
	}
	else {
		throw std::runtime_error("No GameObjects array in the file");
	}	
}

bool GameObjectFactory::reloadJSON(const std::string& filename)
{
	JSONmanager& JSON_MANAGER = JSONmanager::GetInstance();
	return JSON_MANAGER.ResetReadStream(filename);
}


void GameObjectFactory::serializeScene(GameObject& go) 
{
	bool result = false;
	JSONmanager& JSON_MANAGER = JSONmanager::GetInstance();
	result = JSON_MANAGER.OpenWriteStream("Write.json");
	result = JSON_MANAGER.StartNewArray("GameObjects");
	result = JSON_MANAGER.StartNewObject("");
	result = JSON_MANAGER.AddKeyValuePair("Name", go.GetName());
	result = JSON_MANAGER.AddKeyValuePair("Parent", go.GetParentName());
	ComponentSerializer::Serialize(JSON_MANAGER, go.GetTransform());
	result = JSON_MANAGER.EndObject();
	result = JSON_MANAGER.EndArray();
	result = JSON_MANAGER.EndWriteStream();
}

void GameObjectFactory::SendGameObjectToLua(sol::state& lua, GameObject& gameobject) {
	lua["firstArchetype"] = gameobject; // thinking in the future 
}

void GameObjectFactory::addGameObject(GameObject& go) {
	gameObjects.push_back(Ref(go));

	Scene::GetInstance().RegisterGameObject(&go);
	RenderComponent& rc = go.GetRender();
	Scene::GetInstance().RegisterRenderComponent(rc.GetID(), go.GetID());
}

GameObject& GameObjectFactory::findGameObject(const std::string& go_name) {
	for (int i = 0; i < gameObjects.size(); ++i) {
		if (gameObjects[i].get().GetName() == go_name) {
			return gameObjects[i].get();
		}
	}
	throw std::runtime_error("Can't find the game object with the given name");
}

void GameObjectFactory::CreateGOFluaBind(std::shared_ptr<sol::state> lua) {
	lua->new_usertype<GameObjectFactory>("GameObjectObjectFactory",
		"findGameObject", &GameObjectFactory::findGameObject,
		"makeGameObjectArchetype", &GameObjectFactory::makeGameObjectArchetype,
		"addGameObject", &GameObjectFactory::addGameObject,
		"nextID", &GameObjectFactory::nextID
	);
}

void GameObjectFactory::sendGOFtoLua(std::shared_ptr<sol::state> lua) {
	// made sure not creating another instance of GOF here in lua
	(*lua)["GOF"] = std::ref(*this); 
}

void GameObjectFactory::closeDown() {
	for (int i = 0; i < go_memory_manager.size(); ++i) {
		delete go_memory_manager[i];
	}
	go_memory_manager.clear();
	gameObjects.clear();
}