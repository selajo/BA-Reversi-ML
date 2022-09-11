#include "Game.h"
#include "../Utils/Utils.h"
/// <summary>
/// Creates an instance of Game.
/// map will be instanciated with '0' and the default positions for the players ('1', '2') will be set.
/// </summary>
Game::Game(int _length) {
	this->map = new char* [this->length];
    this->moves = std::vector<Point>();
    this->length = _length;

    // Create a row for every pointer
    for (int i = 0; i < this->length; i++)
    {
        map[i] = new char[this->length];
        // Initialize all entries
        std::memset(map[i], '0', this->length * sizeof(bool));
    }

    this->map[3][3] = '1';
    this->map[3][4] = '2';
    this->map[4][3] = '2';
    this->map[4][4] = '1';
}

/// <summary>
/// Creates an instance of Game with fixed length = 8..
/// map will be instanciated with '0' and the default positions for the players ('1', '2') will be set.
/// </summary>
Game::Game() {
    this->length = 8;
    this->map = new char* [this->length];
    this->moves = std::vector<Point>();

    // Create a row for every pointer
    for (int i = 0; i < this->length; i++)
    {
        map[i] = new char[this->length];
        // Initialize all entries
        std::memset(map[i], '0', this->length * sizeof(bool));
    }

    this->map[3][3] = '1';
    this->map[3][4] = '2';
    this->map[4][3] = '2';
    this->map[4][4] = '1';
}

/// <summary>
/// Converts the map to std::string.
/// </summary>
/// <returns>map as std::string</returns>
std::string Game::mapToString() {
    std::string out = "Printing map\n  ";

    for (int i = 0; i < this->length; i++) {
        out += std::to_string(i) + " ";
    }
    out += "\n";

    for (int line = 0; line < this->length; line++) {
        out += std::to_string(line) + " ";
        for (int col = 0; col < this->length; col++) {
            out += std::string(1, map[line][col]) + " ";
        }
        out += "\n";
    }
    return out;
}

/// <summary>
/// Calculates the number of stones of player
/// </summary>
/// <param name="player"></param>
/// <returns>Number of stones of player</returns>
int Game::getPlayerCount(char player) {
    int counter = 0;
    for (int line = 0; line < this->length; line++) {
        for (int col = 0; col < this->length; col++) {
            if (this->map[line][col] == player)
                counter++;
        }
    }

    return counter;
}

/// <summary>
/// Calculates all possible goal-points player can move to. Will be saved in moves.
/// </summary>
/// <param name="player"></param>
void Game::calcPossibleMoves(char player) {
    std::vector<Point> _moves;
    Point p = Point(0, 0);

    for (int line = 0; line < this->length; line++) {
        for (int col = 0; col < this->length; col++) {
            if (map[line][col] == '0') {
                p = Point(col, line);

                for (int dir = 0; dir <= 7; dir++) {
                    int way = tryPossibleMove(p, dir, player, false);

                    //found legal move with path.length > 0
                    if (way != 0) {
                        p.direction = dir;
                        _moves.push_back(p);
                    }
                }
            }
        }
    }

    this->moves = _moves;
}

/// <summary>
/// Getter for member moves
/// </summary>
/// <returns>vector moves</returns>
vector<Point> Game::getMoves() {
	vector<Point> move = this->moves;
	Utils::removeDup(move);
    return move;
}

/// <summary>
/// Calculates and returns the number of fields the player can move to with given Point p.
/// With the flag color the move can be colored by player.
/// </summary>
/// <param name="p"></param>
/// <param name="dir">: Direction in which should be moved.</param>
/// <param name="player"></param>
/// <param name="color">: If move should be colored for the player or not.</param>
/// <returns></returns>
int Game::tryPossibleMove(Point p, int dir, char player, bool color) {
    int counter = 0;
    Point curr = p;
    while (true) {
        //move
        curr.addDirection(dir);
        
        //End of map found -> no possible move
        if (checkForEnd(curr)) {
            counter = 0;
            break;
        }
        //Enemy found -> possible move
        if (checkForEnemy(curr, player)) {
            counter++;
            
            //Redeem stones for player
            if (color) {
                map[curr.y][curr.x] = player;
            }
        }
        //Found myself -> possible move ends here
        else if (checkForOwnPlayer(curr, player)) {
            break;
        }
        //Empty slot or end of map
        else {
            counter = 0;
            break;
        }
    }

    return counter;
}

/// <summary>
/// Checks if Point p is inside map.
/// </summary>
/// <param name="p"></param>
/// <returns>true: p is inside map; false: otherwise.</returns>
bool Game::checkForEnd(Point p) {
    if (p.x < 0 || p.x >= this->length || p.y < 0 || p.y >= this->length)
        return true;
    else
        return false;
}

/// <summary>
/// Checks if Point p is taken by an enemy of given player.
/// </summary>
/// <param name="p"></param>
/// <param name="player"></param>
/// <returns>true: p is taken by enemy; false: p is either taken by player or empty ('0')</returns>
bool Game::checkForEnemy(Point p, char player) {
    if (this->map[p.y][p.x] != '0' && this->map[p.y][p.x] != player)
        return true;
    else
        return false;
}

/// <summary>
/// Checks if Point p is taken by player.
/// </summary>
/// <param name="p"></param>
/// <param name="player"></param>
/// <returns>true: p is taken by player; false: p is either taken by enemy or empty ('0')</returns>
bool Game::checkForOwnPlayer(Point p, char player) {
    if (this->map[p.y][p.x] == player)
        return true;
    else
        return false;
}

/// <summary>
/// Gets possible goal-points player can take as a std::string.
/// </summary>
/// <param name="player"></param>
/// <returns>Possible goal-points player can take as a std::string.</returns>
std::string Game::possibleMovesToString(char player) {
    std::string out = "Possible moves of player " + std::string(1, player) + ":\n";

    for (auto i : this->moves) {
        out += i.toString() + "\n";
    }
    return out;
}

/// <summary>
/// Checks if Point, given by posX and posY, is a valid move the player can make and, if so, the move will be executed.
/// </summary>
/// <param name="posX">X-value of Point (width).</param>
/// <param name="posY">Y-Value of point (height).</param>
/// <param name="player"></param>
void Game::makeMoveTo(int posX, int posY, char player) {
    Point p = Point(posX, posY);

    for (auto i : this->moves) {
        if (i == p) {
            map[p.y][p.x] = player;
            tryPossibleMove(i, i.direction, player, true);
        }

    }
}

/// <summary>
/// Checks if player is able to make legal moves.
/// </summary>
/// <param name="player"></param>
/// <returns>true: player can make legal moves; false: player can't make legal moves</returns>
bool Game::checkForMoves(char player) {
    calcPossibleMoves(player);
    return !this->moves.empty();
}

/// <summary>
/// Checks if any player is able to make moves. If not, the end of the game is reached.
/// </summary>
/// <returns>true: no player is able to move; false: either one of the players can move</returns>
bool Game::checkForEndOfGame() {
    return (!checkForMoves('1') && !checkForMoves('2'));
}

/// <summary>
/// Getter for map
/// </summary>
/// <returns>map</returns>
char** Game::getMap() {
    return map;
}

/// <summary>
/// Setter for map
/// </summary>
/// <param name="_map">New map to be set</param>
void Game::setMap(char** _map) {
    this->map = new char* [this->length];
    this->moves = std::vector<Point>();

    // Create a row for every pointer
    for (int i = 0; i < this->length; i++)
    {
        map[i] = new char[this->length];

        // Initialize all entries
        std::memset(map[i], '0', this->length * sizeof(bool));
    }

    for (int i = 0; i < this->length; i++) {
        for (int j = 0; j < this->length; j++) {
            this->map[i][j] = _map[i][j];
        }
    }
}


Game::Game(const Game& g2) {
    this->moves = std::vector<Point>();
    this->map = new char* [this->length];
    for (int i = 0; i < this->length; i++)
    {
        map[i] = new char[this->length];
        std::memset(map[i], '0', this->length * sizeof(bool));
    }
    //Copy map from g2
    for (int i = 0; i < g2.length; i++) {
        for (int j = 0; j < g2.length; j++) {
            this->map[i][j] = g2.map[i][j];
        }
    }
}

/**
 * Sets player
 * @param _player
 */
void Game::setPlayer(char _player) {
    this->player = _player;
}

/**
 * Gets player
 * @return
 */
char Game::getPlayer() {
    return player;
}

/**
 * Calulates first move to be found
 * @return First move on field
 */
Point Game::calculateMove() {
	calcPossibleMoves(this->player);

	return this->moves.front();
}
