from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

from PyPoint cimport Point
from PyUtils cimport Utils


cdef extern from *:
	ctypedef Game const_game "const Game"

cdef extern from "../Game/Game.h":
	cppclass Game:
		Game()
		Game(int _length)
		Game(const_game &Game)

		bool checkForEnd(Point p)
		bool checkForEndOfGame()
		bool checkForEnemy(Point p, char player)
		bool checkForMoves(char player)
		bool checkForOwnPlayer(Point p, char player)

		char** getMap()
		char getPlayer()
		void setMap(char** _map)
		void setPlayer(char _player)
		int getPlayerCount(char player)
		void calcPossibleMoves(char player)
		vector[Point] getMoves()
		
		void makeMoveTo(int posX, int posY, char player)
		string mapToString()
		string possibleMovesToString(char player)

		int tryPossibleMove(Point p, int dir, char player, bool color=false)

		Point calculateMove()
