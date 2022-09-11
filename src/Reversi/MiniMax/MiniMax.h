#pragma once

#include <algorithm>

#include "../Utils/Node.h"
#include "Heuristic.h"
#include "../Utils/Utils.h"

/// <summary>MiniMax Search using heuritsics</summary>
class MiniMax {
public:
	bool usefulTree = false;
	bool moveOrder = false;
	char originalPlayer;
	char originalEnemy;
	int kbest = 0;
	IHeuristic* h;

	MiniMax(char player, char enemy, IHeuristic* h, int kbest=0);

	void createTree(Node* n, int depth, char player, char enemy);
	virtual float evaluateState(Node* n, char player, char enemy);
	Node* createChildAndMove(Node* n, Point move, char player);
	virtual move_score bestMove(Node* n, int depth, char player, char enemy);
	virtual move_score miniMax(Node* n, int depth, char player, char enemy);

	virtual void moveOrdering(Node* n, bool desc);
	virtual ~MiniMax() {};
};

/// <summary>MiniMax Search only evaluating by end states of game</summary>
class MiniMaxEndState : public MiniMax {
public:
	MiniMaxEndState(char player, char enemy, IHeuristic* h,  int kbest=0) : MiniMax(player, enemy, h, kbest) {};
	float evaluateState(Game g, char player, char enemy);
};

/// <summary>Alpha Beta Pruning only evaluating by end states of game</summary>
class AlphaBeta_ES : public MiniMaxEndState  {
public:
	AlphaBeta_ES(char player, char enemy, IHeuristic* h, int kbest=0) : MiniMaxEndState(player, enemy, h, kbest) {};
	move_score bestMove(Game g, int depth, char player, char enemy);
	move_score alphaBeta(Game g, int depth, move_score alpha, move_score beta, char player, char enemy, Point lastUsed);
};

/// <summary>Alpha Beta Pruning using heuristics</summary>
class AlphaBeta : public MiniMax {
public:
	AlphaBeta(char player, char enemy, IHeuristic*h, int kbest=0) : MiniMax(player, enemy, h, kbest) {};
	move_score bestMove(Node* n, int depth, char player, char enemy);
	move_score alphaBeta(Node* n, int depth, move_score alpha, move_score beta, char player, char enemy);
};

