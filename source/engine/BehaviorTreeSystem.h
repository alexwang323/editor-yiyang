#pragma once
#include "BehaviorTree.h"

class BehaviorTreeSystem {
public:
	static BehaviorTreeSystem& GetInstance();
	void Init();
	void Update(float dt);
	void Shutdown();
private:
	BehaviorTreeSystem();
	//std::shared_ptr<BehaviorTree> buildBehaviorTree();
	std::vector<std::shared_ptr<BehaviorTree>> behaviorTrees;
};