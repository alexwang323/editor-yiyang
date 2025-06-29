#pragma once
#include <string>
#include <sol/sol.hpp>

class GameObject;

class BehaviorNode {
public:
	enum NodeStatus {
		READY = 0, // node is enterable
		RUNNING = 1, // node is currently running
		SUSPENDED = 2, // node won't execute anything
		SUCCESS = 3, // node succeeded,
		FAILURE = 4 // node failed
	};
	enum NodeTypes {
		LEAF = 0,
		DECORATOR = 1,
		SELECTOR = 2,
		SEQUENCER = 3
	};
	// gonna set the agent in a separate function instead of in behaviorNode ctor
	BehaviorNode(const std::string& name);
	// node themselves will be deleted with the tree
	virtual ~BehaviorNode() {}

	//setting the status for all nodes in the tree
	NodeStatus tick(float dt);

	void set_status(NodeStatus newStatus);
	// set this all children's status, recursively
	void set_status_all(NodeStatus newStatus);
	// set only the direct children's status
	void set_status_children(NodeStatus newStatus);

	const std::string& get_name () const;
	NodeStatus get_status();

	virtual NodeStatus on_enter();
	virtual NodeStatus on_update(float dt);
	virtual NodeStatus on_exit();

	std::shared_ptr<BehaviorNode> getChild(int index);
	size_t getNumChildren();
	void add_child(std::shared_ptr<BehaviorNode> child);

	static void CreateBehaviorNodeLuaBind(std::shared_ptr<sol::state> lua);
protected:
	std::string node_name;
	std::vector<std::shared_ptr<BehaviorNode>> children;
private:
	NodeStatus m_Status;
	std::shared_ptr<BehaviorNode> parent;
};
