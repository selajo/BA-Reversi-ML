from libcpp.vector cimport vector
from libcpp cimport bool
from PyGame cimport Game
from PyPoint cimport Point
from PyUtils cimport Utils

cdef extern from "../MiniMax/Heuristic.h":
	cppclass Heuristic:
		Heuristic(float boardWeight, float mobilityWeight, float cornersWeight, float scoreWeight);
		vector[vector[int]] getStaticBoard();

		int calcStaticBoard(char** board, char player);
		float playerScore(Game g, char player, char enemy);
		float mobility(Game g, char player, char enemy);
		float cornersCaptured(Game g, char player, char enemy);

		float calcHeuristic(Game g, char player, char enemy);
