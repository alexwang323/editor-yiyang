#include "ginkgopch.h"
#include "BehaviorTree.h"
#include "ScriptingSystem.h"

BehaviorTree::BehaviorTree(const std::string& name) : treeName(name) {}

void BehaviorTree::Init() {
	root = std::make_shared<BehaviorNode>("root");
}

void BehaviorTree::Update(float dt) {
	root->tick(dt);
	if (root->get_status() != BehaviorNode::NodeStatus::READY
		&& root->get_status() != BehaviorNode::NodeStatus::RUNNING) {
		//std::cout << "Root node resetting to ready " << std::endl;
		root->set_status(BehaviorNode::NodeStatus::READY);
	}
}

void BehaviorTree::Shutdown() {
	// since I'm using smart pointers, 
	// shouldn't need to delete the node myself here
}

void BehaviorTree::build_tree() {
	// temporary 
	sequencer = std::make_shared<SequencerNode>("Sequencer");
	ScriptingSystem& scripting = ScriptingSystem::GetInstance();
	leaf_moveToTarget = scripting.get_behavior_scripts(0); 
	leaf_playSound = scripting.get_behavior_scripts(1);
	root->add_child(sequencer);
	sequencer->add_child(leaf_moveToTarget);
	sequencer->add_child(leaf_playSound);
}

void BehaviorTree::recursive_add(BehaviorNode* node) {
	// gonna be used when behaviorTreebuilder is done
}