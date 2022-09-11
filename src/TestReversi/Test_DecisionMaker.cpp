#include <gtest/gtest.h>
#include "../Reversi/DecisionMaker/DecisionMaker.cpp"
#include "../Reversi/MCTS/MCTS_Hybrids.cpp"

/////// DecisionMaker ///////

TEST(DecisionMakerTest, MCTS) {
	MCTS* mc;
	DecisionMaker dm;
	mc = dm.setAlgorithm("MCTS", '1', '2', 1.0);
}

TEST(DecisionMakerTest, MCTS_MB) {
	MCTS* mc;
	DecisionMaker dm;
	mc = dm.setAlgorithm("MB", '1', '2', 1.0);
}
