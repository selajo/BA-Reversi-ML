#include "MCTS.h"
#include <cmath>
#include "../Utils/Helper.h"

/// <summary>
/// Calculates best child-node according to UCT 
/// </summary>
/// <param name="n">Node, which child-nodes should be checked.</param>
/// <param name="c">Value C for the UCT calculation</param>
/// <returns>Best child-node according to UCT</returns>
Node* MCTS::bestChild(Node* n, double c) {
	double uct_max = -5000, uct_min = 5000;
	double uct;
	Node* bestChild = n;

	for (Node* child : n->children) {
		if(child->lastUsed == Point(-1, -1))
			continue;
		else {
			uct = calcUCT(n, child, c);
			if (uct > uct_max) {
				bestChild = child;
				uct_max = uct;
			}
		}
	}
	return bestChild;
}

/// <summary>
/// Calculates UCT
/// </summary>
/// <param name="n">Node, which child-nodes should be checked.</param>
/// <param name="child"> Child Node of n</param>
/// <param name="c">Value C for the UCT calculation</param>
/// <returns>UCT value</returns>
double MCTS::calcUCT(Node* n, Node* child, double c) {
	double uct = (double)child->score / (double)child->visits;
	uct += c * child->psa * std::sqrt(2 * std::log(n->visits) / (double)child->visits);
	return uct;
}

/// <summary>
/// Backpropagation-phase: visits and won will be added to node.
/// </summary>
/// <param name="n"></param>
/// <param name="won"></param>
void MCTS::backup(Node* n, int won) {
	while (true) {
		if (n == NULL)
			break;

		n->score += won;
		n->visits += 1;
		n = n->parent;
	}
}

/// <summary>
/// Makes a random possible move.
/// </summary>
/// <param name="g"></param>
/// <param name="_player"></param>
Game MCTS::makeRandomMove(Game g, char _player) {
	g.calcPossibleMoves(_player);
	std::vector<Point> moves = g.getMoves();
	Point random = Utils::getRandomFromVector(moves);

	g.makeMoveTo(random.x, random.y, _player);
	return g;
}

/// <summary>
/// Makes random move that hasn't been used before
/// </summary>
/// <param name="n"></param>
/// <param name="_player"></param>
/// <param name="enemy"></param>
/// <returns>New node with random made move</returns>
Node* MCTS::makeRandomUnusedMove(Node* n, char _player, char enemy) {
	Node* child = new Node(n, n->g, _player);
	child->g.calcPossibleMoves(_player);
	std::vector<Point> moves = child->g.getMoves();

	//Calculate already used moves by enemy
	std::vector<Point> used;
	for (auto ch : child->children) {
		used.push_back(ch->lastUsedEnemy);
	}
	
	//Delete already used moves
	for (auto m : used) {
		moves.erase(std::remove(moves.begin(), moves.end(), m), moves.end());
	}

	if(moves.empty()) {
		return child;
	}
	Point random = Utils::getRandomFromVector(moves);
	child->g.makeMoveTo(random.x, random.y, _player);
	child->lastUsedEnemy = random;

	return child;
}

/// <summary>
/// Playes the game until the end randomly.
/// </summary>
/// <param name="g"></param>
/// <returns>1: player wins, 2: player loses</returns>
int MCTS::defaultPolicy(Game g, char currPlay) {
	//Enemy needs to move first
	char enemy = otherPlayer(currPlay);

	while (!g.checkForEndOfGame()) {
		if (g.checkForMoves(enemy)) {
			g = makeRandomMove(g, enemy);
		}

		if (g.checkForMoves(currPlay)) {
			g = makeRandomMove(g, currPlay);
		}
	}
	int score = g.getPlayerCount(this->player);
	if (score > 32)
		return 1;
	else if (score == 32)
		return 0;
	return -1;
}

/// <summary>
/// Expand-phase: Random move will be made from n, what creates a new child.
/// </summary>
/// <param name="n"></param>
/// <returns>Newly created child.</returns>
Node* MCTS::expand(Node* root, std::vector<Point>& moves) {
	if(root->g.checkForEndOfGame()) {
		return root;
	}
	else if(!root->g.checkForMoves(root->player)) {
		root->player = otherPlayer(root->player);
		root->g.calcPossibleMoves(root->player);
		moves = root->g.getMoves();
	}

	checkExistingMoves(root, moves);

	Point random;
	try {
		random = moves.front();
	}
	catch(string &s) {
		cout << "Exception" << endl;
		return root;
	}

	int count = std::count(moves.begin(), moves.end(), random);
	for(int i = 0; i < count; i++) {
		try {
			moves.erase(std::remove(moves.begin(), moves.end(), random), moves.end());
		}
		catch(exception &e) { }
	}

	Node* add = new Node(root, root->g, otherPlayer(root->player));
	add->g.calcPossibleMoves(root->player);
	add->g.makeMoveTo(random.x, random.y, root->player);
	add->lastUsed = random;

	return add;
}

/// <summary>
/// Checks if move has already been made and deletes them if necessary
/// </summary>
/// <param name="n"></param>
/// <param name="moves"></param>
void MCTS::checkExistingMoves(Node* n, std::vector<Point>& moves) {
	for(auto child : n->children) {
		moves.erase(std::remove(moves.begin(), moves.end(), child->lastUsed), moves.end());
	}

}

/// <summary>
/// Selection-phase: All child-nodes will be checked. If no child is available, it will be expanded.
/// </summary>
/// <param name="n"></param>
/// <returns>Node that will be checked.</returns>
Node* MCTS::treePolicy(Node* n, std::vector<Point> &moves) {
	int i;
	if(n->player == '0')
		n->g.calcPossibleMoves(this->player);
	else
		n->g.calcPossibleMoves(n->player);
	moves = n->g.getMoves();


	//go to node, which isn't fully expanded yet
	while(moves.size() == n->children.size()) {		
		n = bestChild(n, this->Cp);
		cout << "Went down a step" << endl;

		if(n->score == this->provenWin) {
			return n;
		}

		n->g.calcPossibleMoves(n->player);
		moves = n->g.getMoves();
		if(moves.empty())
			break;
	}
	return n;
}

/// <summary>
/// Evaluates recently made moves and brings root to current game situation
/// </summary>
/// <param name="g"></param>
void MCTS::evaluateRecentMoves(Game g) {
	bool made;
	if(this->recentMoves.empty()) {
		cout << "New game. Creating new tree." << endl;

		Game ig;
		if(ig.getMap() == g.getMap()) {
			root = new Node(NULL, g, this->player);
		}
	}
	else {
		int i = 0;
		for(auto move : this->recentMoves) {
			made = false;
			for(auto child : root->children) {
				//Child exists in tree: go down that path and delete rest of tree
				if(child->lastUsed.x == move.x && child->lastUsed.y == move.y) {
					root = child;
					root->cleanUpOtherPaths();
					made = true;
					break;
				}
			}
			if(!made) {
				//cout << "Previous node didn't exist" << endl;
				//Child didn't exist before
				this->root->deleteFromRoot();
				this->root = new Node(NULL, g, this->player);
			}
			i++;
			this->recentMoves.clear();
		}
	}
}

/// <summary>
/// Gets other player
/// </summary>
/// <param name="_player"></param>
/// <returns>'1' if _player='2' and vice versa</returns>
char MCTS::otherPlayer(char _player) {
	if(_player == '1')
		return '2';
	else
		return '1';
}

/// <summary>
/// MCTS-Algorithm: Selection, Expansion, Rollout and Backpropagation.
/// Selection: Current child-nodes will be checked.
/// Expansion: If no child is available, a new one will be created.
/// Rollout: Simulates the game until the end and returns outcome for player.
/// Backpropagation: All current nodes will be updated with the number of visits and outcomes.
/// </summary>
/// <param name="g"></param>
/// <returns>The best move MCTS found.</returns>
Point MCTS::uctSearch(Game g, int steps) {
	if(this->keepTree) {
		evaluateRecentMoves(g);
	}
	else {
		this->root = new Node(NULL, g, this->player);
	}

	Helper* helper = Helper::getInstance(player);
	Node* vt;
	Node* iroot = this->root;


	g.calcPossibleMoves(this->player);
	std::vector<Point> moves = g.getMoves();

	while (steps > 0) {

		//selection
		vt = treePolicy(iroot, moves);

		//End search if childnode if root is a proven win
		if((vt->score == this->provenWin) && (vt->parent == iroot)) {
			cout << "Found valueable child" << endl;
			break;
		}

		//expansion
		vt = expand(vt, moves);
		//std::cout << "Expand returns with map " << vt->g.mapToString() << endl;
		//std::cout << "With parent: " << " Player " << vt->parent->player << " " << vt->parent->g.mapToString();

		//Rollout
		int state = defaultPolicy(vt->g, vt->player);
		helper->addRollout(state);

		//Backpropagation
		backup(vt, state);

		steps--;
	}

	this->root = bestChild(iroot, 0);
	Point p = this->root->lastUsed;

	if(!this->keepTree) {
		iroot->deleteFromRoot();
	}

	return p;
}
