#pragma once
#include "BehaviorNode.h"
// temporary before having deserialization and separate behaviorTreeBuilder
#include "SequencerNode.h"
#include "LeafScriptNode.h"

class BehaviorTree {
public:
	// will assign the agent later
	BehaviorTree(const std::string& name);
	void Init();
	void Update(float dt);
	void Shutdown();

	void build_tree();
	void recursive_add(BehaviorNode* node);
private:
	std::string treeName;
	std::shared_ptr<BehaviorNode> root;
	// temporary 
	std::shared_ptr<SequencerNode> sequencer;
	std::shared_ptr<LeafScriptNode> leaf_moveToTarget;
	std::shared_ptr<LeafScriptNode> leaf_playSound;
};