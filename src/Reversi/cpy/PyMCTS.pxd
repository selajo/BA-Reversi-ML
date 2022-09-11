from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool
from libc.stdint cimport uint32_t

from PyGame cimport Game
from PyPoint cimport Point
from PyUtils cimport Utils
from PyNode cimport Node


cdef extern from "../MCTS/MCTS.h":
	cppclass Algorithm:
		int mmDepth
		int mctsDepth

		Point uctSearch(Game g, int steps)

cdef extern from "../MCTS/MCTS.h":
	cppclass MCTS(Algorithm):
		int Nsa
		char player
		char enemy
		double Cp
		int provenWin
		int provenLoss
		int mmDepth
		int mctsDepth

		MCTS(char play, char enem, double cp)
		Node* bestChild(Node* n, double c)
		double calcUCT(Node* n, Node* child, double c)
		void backup(Node *n, int won)
		int defaultPolicy(Game g, bool enemyMove)
		Game makeRandomMove(Game g, char _player)
		Node* makeRandomUnusedMove(Node* n, char _player, char _enemy)

		Node* expand(Node* n, vector[Point] &moves)
		Node* treePolicy(Node* n, vector[Point]  &moves)
		Point uctSearch(Game g, int steps)
		
cdef extern from "../MCTS/MCTS.h":
	cppclass MCTS_Solver(MCTS):
		int threshold
		int provenWin
		int provenLoss
		
		MCTS_Solver(char play, char enem, double cp)

		double calcUCT(Node* n, Node* child, double c)
		void backup(Node *n, int won)
		int proveValue(Node* n, int won)

