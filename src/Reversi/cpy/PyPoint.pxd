from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "../Game/Point.h":
	cppclass Point:
		int x
		int y
		int direction
		Point(int x, int y)
		Point()
		int getDeltaY(int direction)
		int getDeltaX(int direction)
		void addDirection(int direction)
		bool operator==(Point p)
		string toString()
