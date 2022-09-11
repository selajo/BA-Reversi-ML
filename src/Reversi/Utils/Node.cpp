#include "Node.h"

/**
 * Adds child to node
 * @param child
 */
void Node::addChild(Node* child) {
	child->parent = this;
	children.push_back(child);
}

/**
 * Deletes all nodes that are before the current root
 */
void Node::cleanUpOtherPaths() {
	Node * parent = this->parent;
	//parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());

	for(Node* child : parent->children) {
		if(child != this) {
			child->deleteFromRoot();
		}
	}

	this->parent = NULL;
	delete(parent);
}
