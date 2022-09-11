#pragma once
#include "../Game/Game.h"
#include <iostream>

using namespace std;

/// <summary>Nodes for game trees</summary>
class Node {
public:
	int visits;
	float score;
	float psa = 1.0;
	char player;

	Game g;

	Point lastUsed = Point(-1, -1);
	Point lastUsedEnemy = Point(-1, -1);

	Node* parent;
	std::vector<Node*> children;

	/**
	 * Creates instance
	 * @param _parent Parent node
	 * @param _g Current game
	 * @param _player Current player
	 */
	Node(Node* _parent, Game _g, char _player) {
		player = _player;
		parent = _parent;
		visits = 0;
		score = 0;
		if (parent != NULL) {
			parent->addChild(this);
		}
		g = Game(8);
		this->g.setMap(_g.getMap());
		this->psa = 1.0;
	};

	Node(const Node& n) {
		cout << "Called copy" << endl;
		this->visits = n.visits;
		this->score = n.score;
		this->player = n.player;
		//this->g.setMap(n.g.getMap());
		this->g = n.g;
		this->lastUsed = n.lastUsed;
		this->lastUsedEnemy = n.lastUsedEnemy;
		this->parent = n.parent;
		this->children = n.children;
		this->psa = 1.0;
	}


	void addChild(Node* child);

	/**
	 * Checks if score of node is less than current node
	 * @param n
	 * @return
	 */
    bool operator < (const Node& n) const
    {
        return (score < n.score);
    }

	/**
	 * Checks if score of node is greater than current node
	 * @param n
	 * @return
	 */
    bool operator > (const Node& n) const
    {
        return (score > n.score);
    }

    /**
     * Deletes tree
     */
    void deleteFromRoot() {
    	for(auto i : this->children) {
    		i->deleteFromRoot();
    	}
    	delete(this);
    }

    /**
     * Prints all nodes of tree
     */
    void printFromParent() {
    	cout << "Score player " << player << " " << this->score << " ";
    	for(auto i : this->children) {
    		i->printFromParent();
    	}
    }

    void cleanUpOtherPaths();

    ~Node() { }
};
