#include "Point.h"

/// <summary>
/// Creates an instance of Point. Member direction will be set to 0.
/// </summary>
/// <param name="x">: The x-coordinate (width)</param>
/// <param name="y">: The y-coordinate (heigth)</param>
Point::Point(int x, int y) {
	this->x = x;
	this->y = y;
	this->direction = 0;
}

/// <summary>
/// Calculate the adjustment for the y-coordinate that needs to be make for direction.
/// </summary>
/// <param name="direction">from 0 to 7</param>
/// <returns>The adjustment for the y-coordinate</returns>
int Point::getDeltaY(int direction) {

	if (direction == 0 || direction == 1 || direction == 7) {
		return -1;
	}
	else if (direction == 2 || direction == 6) {
		return 0;
	}
	else {
		return 1;
	}
}

/// <summary>
/// Returns the adjustment for the x-coordinate that needs to be make for direction.
/// </summary>
/// <param name="direction">direction from 0 to 7</param>
/// <returns>The adjustment for the x-coordinate</returns>
int Point::getDeltaX(int direction) {

	if (direction == 0 || direction == 4) {
		return 0;
	}
	else if (direction == 2 || direction == 1 || direction == 3) {
		return 1;
	}
	else {
		return -1;
	}
}

/// <summary>
/// Adds the deltas for x- and y-coordinate to move in direction.
/// </summary>
/// <param name="direction">direction from 0 to 7</param>
void Point::addDirection(int direction) {
	if (direction >= 0 && direction <= 7) {
		this->x += getDeltaX(direction);
		this->y += getDeltaY(direction);
	}
	else
		throw std::string("Unknown direction found");
}

/// <summary>
/// Compares the x- and y-coordinates of two Point-instances. Direction will not be analysed.
/// </summary>
/// <param name="p"></param>
/// <returns>true: x- and y-coordinates of two Points are the same; false: otherwise</returns>
bool Point::operator==(Point p) {
	if (this->x == p.x && this->y == p.y)
		return true;
	else
		return false;
}

/// <summary>
/// Converts Point to string
/// </summary>
/// <returns></returns>
std::string Point::toString() {
	return "Point(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ")";
}
