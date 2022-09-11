#include "../MCTS/MCTS_Hybrids.h"
#include <iostream>
#include <memory>

using namespace std;

/// @class DecisionMaker
/// @brief Context class to get correspronding Algorithm
///
class DecisionMaker {
public:
	DecisionMaker() { };
	MCTS* setAlgorithm(string input, char player, char enemy, float cp, int limit=0, int prior=0);
};
