#include "ginkgopch.h"
#include "BehaviorTreeSystem.h"

BehaviorTreeSystem& BehaviorTreeSystem::GetInstance() {
	static BehaviorTreeSystem instance;
	return instance;
}

BehaviorTreeSystem::BehaviorTreeSystem() {
	std::shared_ptr<BehaviorTree> new_tree = std::make_shared<BehaviorTree>("test tree");
	behaviorTrees.push_back(new_tree);
}

void BehaviorTreeSystem::Init() {
	behaviorTrees[0]->Init();
	behaviorTrees[0]->build_tree();
}

void BehaviorTreeSystem::Update(float dt) {
	for (int i = 0; i < behaviorTrees.size(); ++i) {
		behaviorTrees[i]->Update(dt);
	}
}

void BehaviorTreeSystem::Shutdown() {
	
}

 
