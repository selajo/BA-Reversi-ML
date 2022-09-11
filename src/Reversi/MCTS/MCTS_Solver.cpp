#include "MCTS.h"
#include <cmath>
#include "../Utils/Helper.h"

int MCTS_Solver::proveValue(Node *n, int won) {
	bool proven = false;
	Helper* helper = Helper::getInstance(player);
	//Found provenWin: if one child has a provenWin as well -> proved
	//else: normal win (1)
	if(won == this->provenWin) {
		if(n->children.empty()) {
			proven = true;
		}
		for(Node* child : n->children) {
			if(child->score >= this->provenWin) {
				cout << "I proved my win" << endl;
				helper->addWin();
				proven = true;
				break;
			}
		}
		if(!proven) {
			won = 1;
		}
	}
	//Found provenLoss: if one child has a provenWin -> not proved (-1)
	else if(won == this->provenLoss) {
		for(Node* child : n->children) {
			if(child->score > this->provenLoss) {
				won = -1;
				break;
			}
		}
		if(won == this->provenLoss)
			helper->addLoss();
			cout << "Couldn't deprove loss..." << endl;
	}
	return won;
}

void MCTS_Solver::backup(Node* n, int won) {
	cout << "Solver backup" << endl;
	bool reachedEnd = n->g.checkForEndOfGame();
	//Node reached EOG -> time to prove
	if(reachedEnd) {
		cout << "Reached end. Time to prove" << endl;
		int score = n->g.getPlayerCount(this->player);
		if(score > 32) {
			won = this->provenWin;
		}
		else if(score < 32) {
			won = this->provenLoss;
		}

		while (true) {
			if (n == NULL)
				break;
			//prove Value of won and update won eventually
			won = this->proveValue(n, won);

			//could reach a draw
			if(won != 0) {
				n->score = won;
			}

			n->visits += 1;
			n = n->parent;
		}
	}
	//Normal backup
	else {
		MCTS::backup(n, won);
	}
}

double MCTS_Solver::calcUCT(Node* n, Node* child, double c) {
	double uct;
	if(child->score > this->provenLoss && child->score < this->provenWin) {
		uct = (double)child->score / (double)child->visits;
	}
	else {
		cout << "Got proven node" << endl;
		if(child->visits < this->threshold) {
			uct = (double)child->score / 1000.0 / (double)child->visits;
		}
		uct = (double)child->score / (double)child->visits;
	}

	uct += c * child->psa * std::sqrt(2 * std::log(n->visits) / (double)child->visits);
	cout << "Child " << child->lastUsed.toString() << " " << uct  << "from " << child->visits << " " << child->score << endl;

	return uct;
}
