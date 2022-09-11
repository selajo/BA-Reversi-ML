#pragma once
#include <stdexcept>
#include <string>

/// <summary>Realizes point of field of Game</summary>
class Point {
public:
	int x;
	int y;
	int direction;

	Point(int x, int y);
	Point() {
		x = -1;
		y = -1;
		direction = -1;
	}

	int getDeltaY(int direction);
	int getDeltaX(int direction);
	void addDirection(int direction);
	bool operator==(Point p);
	std::string toString();
};
