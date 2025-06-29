#include "ginkgopch.h"
#include "BehaviorNode.h"

BehaviorNode::BehaviorNode(const std::string& name) :
	node_name(name), m_Status(READY), parent(nullptr) {}

BehaviorNode::NodeStatus BehaviorNode::tick(float dt) {
	if (m_Status == NodeStatus::READY) {
		return on_enter();
	}
	else if (m_Status == NodeStatus::RUNNING) {
		return on_update(dt);
	}
	else if (m_Status == NodeStatus::SUCCESS || m_Status == NodeStatus::FAILURE) {
		//std::cout << node_name << " exiting " << std::endl;
		return on_exit();
	}
	else {
		return m_Status;
	}
}

void BehaviorNode::set_status(NodeStatus newStatus) {
	m_Status = newStatus;
}

void BehaviorNode::set_status_all(NodeStatus newStatus) {
	m_Status = newStatus;
	for (auto&& child : children) {
		child->set_status_all(newStatus);
	}
}

void BehaviorNode::set_status_children(NodeStatus newStatus) {
	for (auto&& child : children) {
		child->set_status(newStatus);
	}
}

const std::string& BehaviorNode::get_name() const {
	return node_name;
}

BehaviorNode::NodeStatus BehaviorNode::get_status() {
	return m_Status;
}

BehaviorNode::NodeStatus BehaviorNode::on_enter() {
	// basic logic is to mark as running
	set_status(NodeStatus::RUNNING);

	// set the node's children as ready to run
	set_status_children(NodeStatus::READY);

	return m_Status;
}

BehaviorNode::NodeStatus BehaviorNode::on_update(float dt) {
	// for root node it's only going to have one child
	if (children[0]->tick(dt) == NodeStatus::SUSPENDED) {
		set_status(NodeStatus::SUSPENDED);
		//std::cout << node_name << " suspended " << std::endl;
	}
	return m_Status;
}

BehaviorNode::NodeStatus BehaviorNode::on_exit() {
	// base logic is to mark the node as done executing, not used right now
	set_status(NodeStatus::SUSPENDED);
	return m_Status;
}

std::shared_ptr<BehaviorNode> BehaviorNode::getChild(int index) {
	return children[index];
}

size_t BehaviorNode::getNumChildren() {
	return children.size();
}

void BehaviorNode::add_child(std::shared_ptr<BehaviorNode> child) {
	children.emplace_back(child);
	child->parent = std::make_shared<BehaviorNode>(*this);
}

void BehaviorNode::CreateBehaviorNodeLuaBind(std::shared_ptr<sol::state> lua) {
	lua->new_enum("NodeStatus",
		"READY", NodeStatus::READY,
		"RUNNING", NodeStatus::RUNNING,
		"SUSPENDED", NodeStatus::SUSPENDED,
		"SUCCESS", NodeStatus::SUCCESS,
		"FAILURE", NodeStatus::FAILURE
	);
}