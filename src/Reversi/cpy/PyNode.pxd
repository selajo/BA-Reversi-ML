from libcpp.vector cimport vector
from libcpp cimport bool
from PyGame cimport Game
from PyPoint cimport Point
from PyUtils cimport Utils


cdef extern from "../Utils/Node.h":
	cppclass Node:
		int visits
		float score
		char player
		float psa
		
		Game g

		Point lastUsed 
		Point lastUsedEnemy

		Node* parent
		vector[Node*] children

		Node(Node* parent, Game _g, char player)


		void addChild(Node* child)

		bool operator < (const Node& n)
		bool operator > (const Node& n)

		void deleteFromRoot()
		void printFromParent()
