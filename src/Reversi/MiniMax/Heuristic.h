#pragma once
#include "../Game/Game.h"
//#include "../Utils/Utils.h"
#include <vector>

using namespace std;

/// <summary>Interface for Heuristics</summary>
class IHeuristic {
public:
	float boardWeight;
	float mobilityWeight;
	float cornersWeight;
	float scoreWeight;
	virtual float calcHeuristic(Game g, char player, char enemy) = 0;
	virtual ~IHeuristic() {};
};

/// <summary>Heuristic using Mobility, Corners, Score and Static Board</summary>
class Heuristic : public IHeuristic {
public:

	vector<vector<int>> staticBoard;
	Heuristic(float boardWeight=1.0, float mobilityWeight=1.0, float cornersWeight=1.0, float scoreWeight=1);
	vector<vector<int>> getStaticBoard();

	int calcStaticBoard(char** board, char player);
	float playerScore(Game g, char player, char enemy);
	float mobility(Game g, char player, char enemy);
	float cornersCaptured(Game g, char player, char enemy);

	float calcHeuristic(Game g, char player, char enemy);
	void changeWeights(float bw, float sw, float mw, float cb);
};
