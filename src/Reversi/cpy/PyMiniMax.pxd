from libcpp.vector cimport vector
from libcpp cimport bool
from PyGame cimport Game
from PyPoint cimport Point
from PyUtils cimport Utils, move_score
from PyNode cimport Node
from PyHeuristic cimport Heuristic

cdef extern from "../MiniMax/MiniMax.h":
	cppclass MiniMax:
		bool usefulTree
		bool moveOrder
		char originalPlayer
		char originalEnemy
		int kbest
		MiniMax(char player, char enemy, int kbest)

		void createTree(Node* n, int depth, char player, char enemy)	
		float evaluateState(Node* n, char player, char enemy)
		Node* createChildAndMove(Node* n, Point move, char player)
		move_score bestMove(Node* n, int depth, char player, char enemy)
		move_score miniMax(Node* n, int depth, char player, char enemy)

		void moveOrdering(Node* n, bool desc)

cdef extern from "../MiniMax/MiniMax.h":
	cppclass MiniMaxEndState(MiniMax):
		MiniMaxEndState(char player, char enemy, int kbest)
		float evaluateState(Node* n, char player, char enemy)

cdef extern from "../MiniMax/MiniMax.h":
	cppclass AlphaBeta_ES(MiniMaxEndState):
		AlphaBeta_ES(char player, char enemy, int kbest)
		move_score bestMove(Node* n, int depth, char player, char enemy)
		move_score alphaBeta(Node* n, int depth, move_score alpha, move_score beta, char player, char enemy)
		
cdef extern from "../MiniMax/MiniMax.h":
	cppclass AlphaBeta(MiniMax):
		AlphaBeta(char player, char enemy, int kbest)
		move_score bestMove(Node* n, int depth, char player, char enemy)
		move_score alphaBeta(Node* n, int depth, move_score alpha, move_score beta, char player, char enemy);
