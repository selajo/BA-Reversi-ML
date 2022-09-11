#include "DecisionMaker.h"

/**
 * Returns correpsonding Algorithm type
 * @param input Algorithm to use
 * @param player
 * @param enemy
 * @param cp Cp used for exploration in UCTS-Search
 * @param limit Limit for MS and IPEM
 * @param prior Prior for IPEM
 * @return Correpsonding Algorithm type
 */
MCTS* DecisionMaker::setAlgorithm(string input, char player, char enemy, float cp, int limit, int prior) {
		if(input == "MR") {
			return new MCTS_MR(player, enemy, cp);
		}
		else if(input == "MS") {
			return new MCTS_MS(player, enemy, cp, limit);
		}
		else if(input == "MB") {
			return new MCTS_MB(player, enemy, cp);
		}
		else if(input == "IPEM")  {
			return new MCTS_IPEM(player, enemy, cp, limit, prior);
		}
		else if(input == "AlphaBeta") {
			return new AlphaBetaSearch(player, enemy);
		}
		else if(input == "Solver") {
			return new MCTS_Solver(player, enemy, cp);
		}
		else {
			return new MCTS(player, enemy, cp);
	}
}
