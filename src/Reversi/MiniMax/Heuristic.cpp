#include "Heuristic.h"

/**
 * Creates instance. Calculates static board
 * @param boardWeight
 * @param mobilityWeight
 * @param cornersWeight
 * @param scoreWeight
 */
Heuristic::Heuristic(float boardWeight, float mobilityWeight, float cornersWeight, float scoreWeight) {
	this->boardWeight = boardWeight;
	this->mobilityWeight = mobilityWeight;
	this->cornersWeight = cornersWeight;
	this->scoreWeight = scoreWeight;

	vector<int> first = {4, -3, 2, 2, 2, 2, -3, 4};
	vector<int> second = {-3, -4, -1, -1, -1, -1, -4, -3};
	vector<int> third = {2, -1, 1, 0, 0, 1, -1, 2};
	vector<int> fourth = {2, -1, 0, 1, 1, 0, -1, 2};

	vector<vector<int>> board {
		first,
		second,
		third,
		fourth,
		fourth,
		third,
		second,
		first
	};

	staticBoard = board;
}

/**
 * Gets Static Board
 * @return
 */
vector<vector<int>> Heuristic::getStaticBoard() {
	return staticBoard;
}

/**
 * Calculates value of Static Board given current board of game
 * @param board
 * @param player
 * @return
 */
int Heuristic::calcStaticBoard(char** board, char player) {
	int value = 0;

	for(int line = 0; line < 8; line++) {
		for(int col = 0; col < 8; col++) {
			if(board[line][col] == player) {
				value += staticBoard[line][col];
			}
		}
	}

	return value;
}

/**
 * Calculates Mobility given current game
 * @param g
 * @param player
 * @param enemy
 * @return
 */
float Heuristic::mobility(Game g, char player, char enemy){
	float totalValue = 0;
	g.calcPossibleMoves(player);
	int playerVal = g.getMoves().size();

	g.calcPossibleMoves(enemy);
	int enemyVal = g.getMoves().size();

	if(playerVal + enemyVal != 0) {
		totalValue = 100.0 * (playerVal - enemyVal) / (playerVal + enemyVal);
	}

	return totalValue;
}

/**
 * Calculates captured corners given current game
 * @param g
 * @param player
 * @param enemy
 * @return
 */
float Heuristic::cornersCaptured(Game g, char player, char enemy) {
	float totalValue = 0;
	int playerValue = 0;
	int enemyValue = 0;
	char** map = g.getMap();

	for(int line = 0; line < 8; line+=7) {
		for(int col = 0; col < 8; col+=7) {
			if(map[line][col] == player)
				playerValue += 1;
			else if(map[line][col] == enemy)
				enemyValue += 1;
		}
	}

	if(playerValue + enemyValue != 0) {
		totalValue = 100.0 * (playerValue - enemyValue) / (playerValue + enemyValue);
	}

	return totalValue;
}

/**
 * Calculates current score of player
 * @param g
 * @param player
 * @param enemy
 * @return
 */
float Heuristic::playerScore(Game g, char player, char enemy) {
	int playerScore = g.getPlayerCount(player);
	int enemyScore = g.getPlayerCount(enemy);

	return 100.0 * (playerScore - enemyScore) / (playerScore + enemyScore);

}

/**
 * Calculates heuristic value with Mobility, Corners, Static Board and Score
 * @param g
 * @param player
 * @param enemy
 * @return
 */
float Heuristic::calcHeuristic(Game g, char player, char enemy) {
	float corners = this->cornersWeight * cornersCaptured(g, player, enemy);
	float mobility = this->mobilityWeight * this->mobility(g, player, enemy);
	float staticB = this->boardWeight * calcStaticBoard(g.getMap(), player);
	float scoreP = this->scoreWeight * playerScore(g, player, enemy);
	return (corners + mobility + staticB + scoreP) / 4.0;
}

/**
 * Changes Weights
 * @param bw
 * @param sw
 * @param mw
 * @param cw
 */
void Heuristic::changeWeights(float bw, float sw, float mw, float cw) {
	this->boardWeight = bw;
	this->scoreWeight = sw;
	this->mobilityWeight = mw;
	this->cornersWeight = cw;
}



