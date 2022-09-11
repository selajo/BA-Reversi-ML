#include "MiniMax.h"
#include "../Utils/Helper.h"

/**
 * Creates instance of MiniMax
 * @param player
 * @param enemy
 * @param heu
 * @param kbest K for K Best Pruning, if 0: K Best Pruning won't be used
 */
MiniMax::MiniMax(char player, char enemy, IHeuristic* heu, int kbest) : originalPlayer(player), originalEnemy(enemy), kbest(kbest) {
	this->h = heu;
}

/**
 * Evaluated state of node using heuristics
 * @param n
 * @param player
 * @param enemy
 * @return Value calculated by heurtistic
 */
float MiniMax::evaluateState(Node* n, char player, char enemy) {
	return h->calcHeuristic(n->g, originalPlayer, originalEnemy);
}

/**
 * Returns best move found with MiniMax Search. Creating tree beforehand
 * @param n Current node
 * @param depth Depth for search
 * @param player Current player
 * @param enemy Current enemy
 * @return move_score of search
 */
move_score MiniMax::bestMove(Node* n, int depth, char player, char enemy) {
	originalPlayer = player;
	originalEnemy = enemy;
	Node* curr = new Node(NULL, n->g, player);
	createTree(curr, depth, player, enemy);

	move_score ms = this->miniMax(curr, depth, player, enemy);

	curr->deleteFromRoot();
	return ms;
}

/**
 * Runs MiniMax Search
 * @param n Root node
 * @param depth Depth for search
 * @param player Current player
 * @param enemy Current enemy
 * @return move_score of search
 */
move_score MiniMax::miniMax(Node* n, int depth, char player, char enemy) {
	//best is initialized with [NULL, 0];
	move_score currentScore, best;

	//If player is unable to move, but the enemy still is
	if (!n->g.checkForMoves(player) && n->g.checkForMoves(enemy)) {
		char save = player;
		player = enemy;
		enemy = save;
	}

	if (player == originalPlayer) 
		best.score = -1000;
	else
		best.score = 1000;

	//Reached end
	if(depth == 0 || n->g.checkForEndOfGame()) {
		int score = evaluateState(n, originalPlayer, originalEnemy);
		return move_score(Point(-1, -1), score);
	}

	auto moves = n->g.getMoves();
	for (auto move : moves) {
		//Create child node, which makes the move
		Node* child = createChildAndMove(n, move, player);
		currentScore = miniMax(child, depth - 1, enemy, player);

		//player is MAX
		if (player == originalPlayer) {
			if (currentScore.score > best.score)
				best.setter(move, currentScore.score);
		}
		//player is MIN
		else {
			if (currentScore.score < best.score)
				best.setter(move, currentScore.score);
		}
	}
	return best;
}

/**
 * Creates child for tree
 * @param n Current node
 * @param move Move that needs to be made
 * @param player Current player
 * @return New node
 */
Node* MiniMax::createChildAndMove(Node* n, Point move, char player) {
	Node* child = new Node(n, n->g, player);
	child->g.calcPossibleMoves(player);
	child->g.makeMoveTo(move.x, move.y, player);
	child->lastUsed = move;

	return child;
}

/**
 * Creates tree for MiniMax Search
 * @param n Current node
 * @param depth Depth of minimax search
 * @param player Current player
 * @param enemy Current enemy
 */
void MiniMax::createTree(Node* n, int depth, char player, char enemy) {
	if(depth == 0) {
		return;
	}
	else {
		if(n->g.checkForEndOfGame()) {
			n->score = this->evaluateState(n, player, enemy);
			this->usefulTree = true;
			return;
		}
		else if(!n->g.checkForMoves(player)) {
			createTree(n, depth, enemy, player);
		}
		else {
			for(auto move : n->g.getMoves()) {
				Node* child = createChildAndMove(n, move, player);
				int iScore = this->evaluateState(child, player, enemy);
				child->score = iScore;

				if(iScore != 0) {
					this->usefulTree = true;
				}

				createTree(child, depth-1, enemy, player);
			}
			if(this->moveOrder) {
				moveOrdering(n, player==originalPlayer);
			}
		}
	}
}

/**
 * Calculates best move found by Alpha Beta Pruning with EndState
 * @param g Current game
 * @param depth Depth of AlphaBeta Search
 * @param player Current player
 * @param enemy Current enemy
 * @return move_score of search
 */
move_score AlphaBeta_ES::bestMove(Game g, int depth, char player, char enemy) {
	originalPlayer = player;
	originalEnemy = enemy;

	move_score ms;
	ms = this->alphaBeta(g, depth, move_score(Point(), -1000), move_score(Point(), 1000), player, enemy, Point());

	return ms;
}

/**
 * Recursivly runs Alpha Beta Pruning using EndStates
 * According to https://www.geeksforgeeks.org/minimax-algorithm-in-game-theory-set-4-alpha-beta-pruning/
 * @param g Current game
 * @param depth Current depth
 * @param alpha Alpha value
 * @param beta Beta value
 * @param player Current player
 * @param enemy Current enemy
 * @param lastUsed Last used move
 * @return move_score of search
 */
move_score AlphaBeta_ES::alphaBeta(Game g, int depth, move_score alpha, move_score beta, char player, char enemy, Point lastUsed) {
	//If player is unable to move, but the enemy still is able to
	int steps = kbest;
	Game save;

	//Reached end
	if (g.checkForEndOfGame() || depth == 0) {
		return move_score(lastUsed, this->evaluateState(g, player, enemy));
	}

	move_score max, min, ret;
	if (player == originalPlayer) {
		max.score = -5000;

		g.calcPossibleMoves(player);
		auto moves = g.getMoves();

		for (Point move : moves) {
			save = g;
			save.setMap(g.getMap());
			save.calcPossibleMoves(player);
			if(kbest > 0) {
				if(steps == 0) {
					break;
				}
				steps--;
			}

			save.makeMoveTo(move.x, move.y, player);

			ret = alphaBeta(save, depth - 1, alpha, beta, enemy, player, move);
			ret.p = move;

			max = move_score::max(max, ret);
			alpha = move_score::max(alpha, max);

			if (alpha.score >= beta.score)
				break;
		}
		return max;
	}
	else {
		min.score = 5000;

		g.calcPossibleMoves(player);
		auto moves = g.getMoves();

		for (Point move : moves) {
			save = g;
			save.setMap(g.getMap());
			save.calcPossibleMoves(player);
			if(kbest > 0) {
				if(steps == 0) {
					break;
				}
				steps--;
			}

			save.makeMoveTo(move.x, move.y, player);

			ret = alphaBeta(save, depth - 1, alpha, beta, player, enemy, move);
			ret.p = move;

			min = move_score::min(min, ret);
			beta = move_score::min(beta, min);

			if (alpha.score >= beta.score)
				break;
		}
		return min;
	}
}


/**
 * Sorts children of Node n.
 * According to: https://stackoverflow.com/questions/1380463/sorting-a-vector-of-custom-objects (2021)
 * @param n Current node
 * @param desc True: desending, False: ascending
 */
void MiniMax::moveOrdering(Node* n, bool desc) {
	if(n->parent == NULL) {
		return;
	}

	if(desc) {
		sort( n->parent->children.begin( ), n->parent->children.end( ), [ ]( const Node* lhs, const Node* rhs ) {
			return lhs->score > rhs->score;
		});
	}
	else {
		sort( n->parent->children.begin( ), n->parent->children.end( ), [ ]( const Node* lhs, const Node* rhs ){
			return lhs->score < rhs->score;
		});
	}
}



//////MiniMaxEndState methods begin here///////

/**
 * Evaluates state using EndStates
 * @param g Current game state
 * @param player Current player
 * @param enemy Curren enemy
 * @return 10: Player won, -10: player lost, 0: else
 */
float MiniMaxEndState::evaluateState(Game g, char player, char enemy) {
	if (g.checkForEndOfGame()) {
		if (g.getPlayerCount(originalPlayer) > g.getPlayerCount(originalEnemy))
			return 10;
		else if (g.getPlayerCount(originalPlayer) < g.getPlayerCount(originalEnemy))
			return -10;
		else
			return 0;
	}
	return 0;
}

/////// Alpha Beta ///////

/**
 * Calculates best move via Alpha Beta Pruning using heuristics. Creates tree beforehand
 * @param n Current node
 * @param depth Depth of AlphaBeta Search
 * @param player Current player
 * @param enemy Current enemy
 * @return move_score of search
 */
move_score AlphaBeta::bestMove(Node* n, int depth, char player, char enemy) {
	originalPlayer = player;
	originalEnemy = enemy;
	Node* curr = new Node(NULL, n->g, player);
	curr->score = this->evaluateState(n, player, enemy);

	createTree(curr, depth, player, enemy);

	move_score ms;
	//Only AlphaBeta if tree found something useful
	if(this->usefulTree) {
		ms = this->alphaBeta(curr, depth, move_score(Point(), -1000), move_score(Point(), 1000), player, enemy);
	}
	else {
		ms = move_score(curr->children.front()->lastUsed, 0);
	}


	curr->deleteFromRoot();
	return ms;
}

/**
 * Recursively runs Alpha Beta Pruning
 * According to https://www.geeksforgeeks.org/minimax-algorithm-in-game-theory-set-4-alpha-beta-pruning/
 * @param n Current node
 * @param depth Current depth
 * @param alpha Alpha value
 * @param beta Beta value
 * @param player Current player
 * @param enemy Current enemy
 * @return move_score of search
 */
move_score AlphaBeta::alphaBeta(Node* n, int depth, move_score alpha, move_score beta, char player, char enemy) {
	//If player is unable to move, but the enemy still is able to
	int steps = kbest;

	//Reached end
	if (n->g.checkForEndOfGame() || depth == 0) {
		return move_score(n->lastUsed, n->score);
	}

	move_score max, min, ret;
	if (n->player == originalPlayer) {
		max.score = -5000;

		for (auto child : n->children) {
			if(kbest > 0) {
				if(steps == 0) {
					break;
				}
				steps--;
			}

			ret = alphaBeta(child, depth - 1, alpha, beta, enemy, player);
			ret.p = child->lastUsed;

			max = move_score::max(max, ret);
			alpha = move_score::max(alpha, max);

			if (alpha.score >= beta.score)
				break;
		}
		return max;
	}
	else {
		min.score = 5000;

		for (auto child : n->children) {
			if(kbest > 0) {
				if(steps == 0) {
					break;
				}
				steps--;
			}

			ret = alphaBeta(child, depth - 1, alpha, beta, player, enemy);
			ret.p = child->lastUsed;

			min = move_score::min(min, ret);
			beta = move_score::min(beta, min);

			if (alpha.score >= beta.score)
				break;
		}
		return min;
	}
}

