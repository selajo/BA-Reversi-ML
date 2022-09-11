#pragma once
#include "MCTS.h"
#include "../MiniMax/MiniMax.h"

/// <summary>MiniMax in Rollout</summary>
class MCTS_MR : public MCTS_Solver {
public:
	int defaultPolicy(Game g, char currPlay);
	void makeMove(Game& g, Point p, char _player);
	MCTS_MR(char play, char enem, double cp) : MCTS_Solver(play, enem, cp) {};
};

/// <summary>MiniMax in Selection and Expansion </summary>
class MCTS_MS : public MCTS_Solver {
public:
	int limit;
	MCTS_MS(char play, char enem, double cp, int lim) : MCTS_Solver(play, enem, cp), limit(lim) {};
	Node* treePolicy(Node* n, std::vector<Point>& moves);
	virtual void miniMaxAndUpdate(Node* n, char player, char enemy);
};

/// <summary>MiniMax in Backpropagation</summary>
class MCTS_MB : public MCTS_Solver {
public: 
	MCTS_MB(char play, char enem, double cp) : MCTS_Solver(play, enem, cp) {};
	void backToParent(Node* n);
	void backup(Node* n, int won);
};

/// <summary>MiniMax in Selection and Expansion using heuristics </summary>
class MCTS_IPEM : public MCTS_MS {
public:
	int prior;
	MCTS_IPEM(char play, char enem, double cp, int lim, int pri) : MCTS_MS(play, enem, cp, lim), prior(pri) {};
	void miniMaxAndUpdate(Node* n, char player, char enemy);
};
