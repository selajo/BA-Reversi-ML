#pragma once
#include "Point.h"

#include <cstring>
#include <iostream>
#include <vector>

/// <summary>Implements the game Reversi</summary>
class Game {
private:
	char** map;
	int length = 8;
	std::vector<Point> moves;
	char player = '1';

public:
	/// <summary> Constructor for Game.</summary>
	Game() ;
	Game(int _length);
	Game(const Game&); //copy constructor

	bool checkForEnd(Point p);
	bool checkForEndOfGame();
	bool checkForEnemy(Point p, char player);
	bool checkForMoves(char player);
	bool checkForOwnPlayer(Point p, char player);

	char** getMap();
	char getPlayer();
	void setMap(char** _map);
	void setPlayer(char _player);
	int getPlayerCount(char player);
	void calcPossibleMoves(char player);
	std::vector<Point> getMoves();

	void makeMoveTo(int posX, int posY, char player);
	std::string mapToString();
	std::string possibleMovesToString(char player);

	int tryPossibleMove(Point p, int dir, char player, bool color=false);

	Point calculateMove();

};
