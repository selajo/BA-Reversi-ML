from libcpp.string cimport string
from libc.stdint cimport uint32_t

from PyGame cimport Game
from PyPoint cimport Point
from PyUtils cimport Utils
from PyNode cimport Node
from PyMCTS cimport Algorithm

cdef extern from "../Network/Network.h":
	cppclass Network:
		Game g
		int GameLoop(string ipadress, int port, Algorithm* mcts)
		int evalMessage(char buffer[], Point &answer, Algorithm* mcts)
