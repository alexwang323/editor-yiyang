#pragma once
#include <rapidjson/document.h>
#include "ginkgopch.h"
#include "GameObject.h"
#include "RefWrapper.h"
#include <sol/sol.hpp>

#define MAX_NUM_OBJECTS 200

class JSONmanager;

class GameObjectFactory {
public:
	GameObject makeGameObjectArchetype(); // from json
	bool reloadJSON(const std::string& filename);
	static GameObjectFactory& GetInstance();
	void serializeScene(GameObject& go); // just for demonstration
	void SendGameObjectToLua(sol::state& lua, GameObject& gameobject);
	void addGameObject(GameObject& go);
	// should probably return game object id, which also its index in gameObjects
	GameObject& findGameObject(const std::string& go_name); 
	static void CreateGOFluaBind(std::shared_ptr<sol::state> lua);
	void sendGOFtoLua(std::shared_ptr<sol::state> lua);
	void closeDown();
private:
	GameObjectFactory();
	std::vector<Ref<GameObject>> gameObjects; // working on not using the ptr here
	std::vector<GameObject*> go_memory_manager;
	int nextID = 0; // increment by 1 and then assign
};