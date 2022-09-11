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
from PyMiniMax cimport MiniMax
from PyMiniMax cimport MiniMaxEndState
from PyMiniMax cimport AlphaBeta_ES
from PyMiniMax cimport AlphaBeta

cdef extern from "../DecisionMaker/DecisionMaker.h":
	cppclass DecisionMaker:
		DecisionMaker()
		MCTS* setAlgorithm(string input, char player, char enemy, float cp, int limit, int prior)

