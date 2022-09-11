from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool
from libc.stdint cimport uint32_t

from PyPoint cimport Point

cdef extern from "../Utils/Utils.h":
	cppclass Utils:
		@staticmethod
		string convertToString(const char* a, int size)
		
		@staticmethod
		string getCurrentDate()
		
		@staticmethod
		string getCurrentTime()

		@staticmethod
		int hextodec(uint32_t hexa)

		@staticmethod
		void printHelp()
		
		@staticmethod
		void readArgs(vector[string] all_args, int &port, string &ip, string &profile, char &player, int &limit, int &prior, double &cpV, int &mDepth, int &kBest, bool &moveOrdering)

		@staticmethod
		T getRandomFromVector[T](vector[T] vec)

		@staticmethod
		void removeDup(vector[Point] &v)

cdef extern from "../Utils/Utils.h":
	cppclass move_score:
		Point p
		int score
		move_score(Point _p, int _score)
		move_score()
		void setter(Point _p, int _score)
		
		@staticmethod
		move_score max(move_score a, move_score b)
		
		@staticmethod
		move_score min(move_score a, move_score b)
