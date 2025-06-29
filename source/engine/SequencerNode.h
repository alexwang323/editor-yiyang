#pragma once
#include "BehaviorNode.h"

class SequencerNode : public BehaviorNode {
public:
	SequencerNode(const std::string& name);
	NodeStatus on_enter();
	NodeStatus on_update(float dt);
private:
	int currIndex;
};