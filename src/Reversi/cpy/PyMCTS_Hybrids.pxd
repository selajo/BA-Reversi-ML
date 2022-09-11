from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool
from libc.stdint cimport uint32_t

from PyGame cimport Game
from PyPoint cimport Point
from PyUtils cimport Utils
from PyNode cimport Node
from PyMCTS cimport Algorithm
from PyMCTS cimport MCTS
from PyMCTS cimport MCTS_Solver
from PyMiniMax cimport MiniMax
from PyMiniMax cimport MiniMaxEndState
from PyMiniMax cimport AlphaBeta_ES
from PyMiniMax cimport AlphaBeta

cdef extern from "../MCTS/MCTS_Hybrids.h":
	cppclass MCTS_MR(MCTS_Solver):
		int depth
		int defaultPolicy(Game g, bool enemyMove)
		void makeMove(Game& g, Point p, char _player)
		MCTS_MR(char play, char enem, double cp)

cdef extern from "../MCTS/MCTS_Hybrids.h":
	cppclass MCTS_MS(MCTS_Solver):
		int depth
		int limit
		MCTS_MS(char play, char enem, double cp, int lim)
		Node* treePolicy(Node* n, vector[Point]& moves)
		void miniMaxAndUpdate(Node* n, char player, char enemy)

cdef extern from "../MCTS/MCTS_Hybrids.h":
	cppclass MCTS_MB(MCTS_Solver):
		int depth
		void backToParent(Node* n)
		MCTS_MB(char play, char enem, double cp)
		void backup(Node* n, int won)

cdef extern from "../MCTS/MCTS_Hybrids.h":
	cppclass MCTS_IPEM(MCTS_Solver):
		int prior
		MCTS_IPEM(char play, char enem, double cp, int lim, int pri)
		void miniMaxAndUpdate(Node* n, char player, char enemy)
