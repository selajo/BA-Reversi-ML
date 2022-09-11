#pragma once
#include "../Utils/Node.h"
#include "../Utils/Utils.h"
#include "../MiniMax/MiniMax.h"
#include <algorithm>
#include <vector>

/// <summary>Abstract class for strategy pattern</summary>
class Algorithm {
public:
	int mmDepth;
	int mctsDepth;
	std::vector<Point> recentMoves;

	virtual Point uctSearch(Game g, int steps) = 0;
	bool keepTree = false;
	Heuristic * h;


	virtual ~Algorithm() = default;
};

/// <summary>Monte Carlo Tree Search</summary>
class MCTS : public Algorithm{
public:
	char player;
	char enemy;
	double Cp;
	int provenWin = 1000;
	int provenLoss = -1000;

	Node * root;

	MCTS(char play, char enem, double cp) : player(play), enemy(enem), Cp(cp) {
		root = new Node(NULL, Game(), play);
	};

	virtual Node* bestChild(Node* n, double c);
	virtual double calcUCT(Node* n, Node* child, double c);
	virtual void backup(Node *n, int won);
	virtual int defaultPolicy(Game g, char currPlay);
	virtual Game makeRandomMove(Game g, char _player);
	virtual Node* makeRandomUnusedMove(Node* n, char _player, char _enemy);

	virtual Node* expand(Node* n, std::vector<Point>& moves);
	virtual Node* treePolicy(Node* n, std::vector<Point>& moves);
	virtual Point uctSearch(Game g, int steps);

	void evaluateRecentMoves(Game g);
	void checkExistingMoves(Node* n, std::vector<Point>& moves);
	char otherPlayer(char _player);

	virtual ~MCTS() {};
};

/// <summary>MCTS with solver expansion</summary>
class MCTS_Solver : public MCTS {
public:
	int threshold = 20;
	
	MCTS_Solver(char play, char enem, double cp) : MCTS(play, enem, cp) {};
	
	virtual void backup(Node *n, int won);
	virtual int proveValue(Node* n, int won);
	virtual double calcUCT(Node* n, Node* child, double c);
};

/// <summary>Runs AlphaBeta search</summary>
class AlphaBetaSearch : public MCTS {
public:
	AlphaBetaSearch(char play, char enem) : MCTS(play, enem, 1.0){ };

	Point uctSearch(Game g, int steps) {
		MiniMax * mm = new AlphaBeta(player, enemy, this->h);
		Node * n = new Node(NULL, g, player);
		cout << "Starting AlphaBeta" << endl;
		return mm->bestMove(n, this->mmDepth, player, enemy).p;
	}
};
