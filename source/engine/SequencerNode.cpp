#include "ginkgopch.h"
#include "SequencerNode.h"

SequencerNode::SequencerNode(const std::string& name) : BehaviorNode(name) {
	currIndex = 0;
}

BehaviorNode::NodeStatus SequencerNode::on_enter() {
	currIndex = 0;
	return BehaviorNode::on_enter();
}

BehaviorNode::NodeStatus SequencerNode::on_update(float dt) {
	std::shared_ptr<BehaviorNode> currNode = children[currIndex];
	currNode->tick(dt);
	if (currNode->get_status() == NodeStatus::FAILURE) {
		set_status(NodeStatus::FAILURE);
		return NodeStatus::FAILURE;
	}
	else if (currNode->get_status() == NodeStatus::SUCCESS) {
		// move on to the next node
		++currIndex;
		// if we hit the size, then all nodes succeed
		if (currIndex == getNumChildren()) {
			set_status(NodeStatus::SUCCESS);
			//std::cout << node_name << " success " << std::endl;
			return NodeStatus::SUCCESS;
		}
		return NodeStatus::RUNNING;
	}
	return NodeStatus::RUNNING;
}
