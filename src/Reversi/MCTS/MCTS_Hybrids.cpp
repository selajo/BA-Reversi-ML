#include "MCTS_Hybrids.h"
#include "../Utils/Helper.h"

////// MCTS-MR: Minimax in Rollout ///////

/// <summary>
/// Rollout from MCTS_MR. Uses AlphaBeta_ES in each turn of currPlay
/// </summary>
/// <param name="game"></param>
/// <param name="currPlay">Current player</param>
/// <returns>1: player won, 0: draw, -1: player lost</returns>
int MCTS_MR::defaultPolicy(Game g, char currPlay) {

	//Enemy needs to move first
	char enemy = otherPlayer(currPlay);
	int useless = 0;
	AlphaBeta_ES* mm = new AlphaBeta_ES(currPlay, enemy, new Heuristic());

	while (!g.checkForEndOfGame()) {
		if (g.checkForMoves(enemy)) {
			g = makeRandomMove(g, enemy);
		}

		if(currPlay == this->player) {
			if (g.checkForMoves(currPlay)) {

				if (useless == 0) {
					move_score result = mm->bestMove(g, this->mmDepth, currPlay, enemy);

					//Found something useful: can stop right there
					if( result.score == 10)
						return 1;
					else if (result.score == -10)
						return -1;
					else {
						g = makeRandomMove(g, currPlay);
						useless = this->mmDepth / 2;
					}
				}
				else {
					g = makeRandomMove(g, currPlay);
					useless--;
				}

			}
		}
		else {
			if (g.checkForMoves(currPlay)) {
				g = makeRandomMove(g, currPlay);
			}
		}


	}
	delete(mm);

	int score = g.getPlayerCount(this->player);
	if (score > 32)
		return 1;
	else if (score == 32)
		return 0;
	return -1;
}

/// <summary>
/// Makes move to Point p with _player
/// </summary>
/// <param name="p"></param>
/// <param name="g"></param>
/// <param name="_player"></param>
void MCTS_MR::makeMove(Game& g, Point p, char _player) {
	g.calcPossibleMoves(_player);
	g.makeMoveTo(p.x, p.y, _player);
}


/////// MCTS_MS: Minimax in Selection & Expansion ///////

/// <summary>
/// Selection from MCTS_MS. Uses AlphaBeta_ES when limit of node is reached
/// </summary>
/// <param name="n"></param>
/// <param name="moves">Current player</param>
Node* MCTS_MS::treePolicy(Node* n, std::vector<Point>& moves) {
	Helper* helper = Helper::getInstance(player);

	if (n->visits >= this->limit) {
		helper->addLimit();
		miniMaxAndUpdate(n, otherPlayer(n->player), n->player);
	}

	return MCTS_Solver::treePolicy(n, moves);
}

/// <summary>
/// Runs AlphaBeta_ES and updates node values if proven
/// </summary>
/// <param name="n"></param>
/// <param name="player">Current player</param>
/// <param name="enemy">Current enemy</param>
void MCTS_MS::miniMaxAndUpdate(Node* n, char player, char enemy) {
	AlphaBeta_ES* mm = new AlphaBeta_ES(player, enemy, new Heuristic());
	Node* curr = n;
	move_score result = mm->bestMove(n->g, this->mmDepth, player, enemy);

	if ((result.score == 10 && player == this->player) || (result.score == -10 && player == this->enemy) ) {
		result.score = 1;
	}
	else if ((result.score == -10 && player == this->player) || (result.score == 10 && player == this->enemy)) {
		result.score = -1;
	}

	//Update parents if proven
	if(result.score > 0  && n->score > 0) {
		cout << "Proved my positive value" << endl;
		MCTS_Solver::backup(n, result.score);
	}
	else if(result.score < 0 && n->score < 0) {
		cout << "Proved my bad value" << endl;
		MCTS_Solver::backup(n, -1);
	}
}


/////// MCTS_MB: MiniMax in Backpropagation ///////

/// <summary>
/// Propagates to the mmDepth-last Node
/// </summary>
/// <param name="n"></param>
void MCTS_MB::backToParent(Node* n) {
	int i = this->mmDepth;
	while(i > 0) {
		i--;
		if(n->parent == NULL) {
			break;
		}
		n = n->parent;
	}
}

/// <summary>
/// Bropagation of MCTS_MB. Uses AlphaBeta_ES if terminal node is found
/// </summary>
/// <param name="n"></param>
/// /// <param name="won">Return value of Rollout</param>
void MCTS_MB::backup(Node* n, int won) {
	Node * curr = n;
	//found a win, but are the other moves, the enemy could make, also wins for player?
	if(n->g.checkForEndOfGame()) {
		cout << "Reached a node with EOG." << endl;
		int score = n->g.getPlayerCount(this->player);
		backToParent(n);

		AlphaBeta_ES* mm = new AlphaBeta_ES(n->player, otherPlayer(n->player), new Heuristic());
		move_score result = mm->bestMove(n->g, this->mmDepth, n->player, otherPlayer(n->player));

		if (score > 32) {
			cout << "I won in this case... time to check the other options that could have been made." << endl;
			//Minimax for enemy
			//Enemy cannot win -> proven win for player
			if (result.score < 0 && n->player == this->enemy) {
				cout << "Enemy cannot win... It's my time to shine." << endl;
				MCTS_Solver::backup(n, this->provenWin);
				return;
			}
		}
		//found a loss
		else if(score < 32) {
			cout << "I lost in this case... time to check the other options that could have been made." << endl;

			//Player cannot win -> proven loss for player
			if (result.score < 0 && n->player == this->player) {
				MCTS_Solver::backup(n, this->provenLoss);
				return;
			}
		}
	}

	MCTS_Solver::backup(curr, won);
}


/////// MCTS_IPEM ///////
/// /// <summary>
/// Runs AlphaBeta_ES and updates node unconditionally
/// </summary>
/// <param name="n"></param>
/// <param name="player">Current player</param>
/// <param name="enemy">Current enemy</param>
void MCTS_IPEM::miniMaxAndUpdate(Node* n, char player, char enemy) {
	AlphaBeta* mm = new AlphaBeta(player, enemy, this->h);
	Node* curr = n;
	move_score result = mm->bestMove(n, this->mmDepth, player, enemy);

	if(player == this->enemy)
		result.score = -result.score;

	cout << "Best score: " << result.score << endl;
	curr->visits += this->prior;
	curr->score += prior * result.score;
}
