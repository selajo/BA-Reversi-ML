#pragma once
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <algorithm>
#include "../Game/Point.h"
#include "../MiniMax/Heuristic.h"


using namespace std;

/// <summary>Holds methods that can be used by all classes</summary>
class Utils {
public:
	static string convertToString(const char* a, int size);
	static string getCurrentDate();
	static string getCurrentTime();

	static int hextodec(uint32_t hexa);

	static void printHelp();
	static void readArgs(vector<string> all_args, int &port, string &ip, string &profile, char &player, int &limit, int &prior, double &cpV, int &mDepth, int &kBest, bool &moveOrdering, bool &keepTree, vector<float> &heurWeights);

	template <typename T>
	static T getRandomFromVector(vector<T> vec) {
		if (vec.empty()) {
			throw string("No moves available. Can not expand");
		}

		srand((unsigned int)time(NULL));
		T random = vec[rand() % vec.size()];
		return random;
	}

	static void removeDup(vector<Point> &v);

	~Utils() {};

};
/*
class Timer {

};
*/
/// <summary>Holds point with corresponding value</summary>
struct move_score {
public:
	Point p;
	int score;

	move_score(Point _p, int _score) {
		score = _score;
		p = _p;
	};
	move_score() {
		p = Point();
		score = 0;
	};

	void setter(Point _p, int _score) {
		this->p = _p;
		this->score = _score;
	}

	static move_score max(move_score a, move_score b) {
		return a.score > b.score ? a : b;
	}

	static move_score min(move_score a, move_score b) {
		return a.score < b.score ? a : b;
	}
};
