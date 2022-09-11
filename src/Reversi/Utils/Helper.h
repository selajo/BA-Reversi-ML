#include <iostream>
#include <fstream>
#include <chrono>
#include "Utils.h"

using namespace std;

/// <summary>Holds important values used for csv file. Realized as Singleton.</summary>
class Helper {
private:
	chrono::_V2::system_clock::time_point i_startTime;
	chrono::_V2::system_clock::time_point mini_startTime;

	double totalTime;
	int movesPlayer = 0;
	int movesEnemy = 0;
	double timePerMove = 0;
	int mmDepth;
	string bonus = "";
	int rollouts;
	int limits;
	int provedWin;
	int provedLoss;
	int edges;
	int keep;

	static Helper* instance;

	/**
	 * Creates instance
	 * @param pl Current player
	 */
	Helper(char pl) {
		totalTime = 0;
		mmDepth = 0;
		player = pl;
		rollouts = 0;
		limits = 0;
		provedWin = 0;
		provedLoss = 0;
		edges = 0;
		keep = 0;
	}

public:
	char player;

	/**
	 * Gets instance of own
	 * @param pl
	 * @return
	 */
	static Helper* getInstance(char pl='1') {
		if(instance == 0) {
			instance = new Helper(pl);
		}
		return instance;
	}

	/**
	 * Starts timer
	 */
	void startTime() {
		i_startTime = chrono::high_resolution_clock::now();
	}

	/**
	 * Ends timer
	 */
	void endTime() {
		auto ending = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> ms_double = ending - i_startTime;
		totalTime = ms_double.count();
	}

	/**
	 * Starts MiniTimer
	 */
	void startMiniTimer() {
		mini_startTime = chrono::high_resolution_clock::now();
	}

	/**
	 * Starts EndTimer
	 */
	void endMiniTimer() {
		auto ending = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> ms_double = ending - mini_startTime;
		timePerMove += ms_double.count();
	}

	/**
	 * Add 1 to moves made from player
	 */
	void addMovePlayer() {
		movesPlayer++;
	}

	/**
	 * Add 1 to moves made from enemy
	 */
	void addMoveEnemy() {
		movesEnemy++;
	}

	/**
	 * Adds time that was needed to calculate move
	 * @param t New time
	 */
	void addTimePerMove(double t) {
		timePerMove = (timePerMove + t) / 2.0;
	}

	/**
	 * Sets depth of minimax
	 * @param d Depth of minimax
	 */
	void setMMDepth(int d) {
		mmDepth = d;
	}

	/**
	 * Adds bonus. Can be anything.
	 * @param input
	 */
	void addBonus(string input) {
		bonus = input;
	}

	/**
	 * Adds value of rollout
	 * @param score
	 */
	void addRollout(int score) {
		rollouts += score;
	}

	/**
	 * Adds limit when reached
	 */
	void addLimit() {
		limits++;
	}

	/**
	 * Add proven win
	 */
	void addWin() {
		provedWin++;
	}

	/**
	 * Add proven loss
	 */
	void addLoss() {
		provedLoss++;
	}
	
	/**
	 * Adds value that was kept from MCTSKeep
	 * @param add Value from node
	 */
	void addkeep(int add) {
		keep += add;
	}

	/**
	 * Counts edges taken by player
	 * @param g Current game
	 */
	void countEdges(Game g) {
		int score = 0;
		char** map = g.getMap();
		for(int line = 0; line < 8; line+=7) {
			for(int col = 0; col < 8; col+=7) {
				if(map[line][col] == player)
					score += 1;
			}
		}

		edges = score;
	}

	/**
	 * Writes Values to csv file
	 * @param profile Current Algorithm
	 * @param endState 1: player won, 0 player lost or draw
	 * @param mctsDepth Depth used for UCTS-Search
	 * @param points Number of fields taken by player
	 */
	void writeToCSV(string profile, float endState, int mctsDepth, int points) {
		if(keep > 0) {
			bonus = std::to_string(keep);
		}
	
		string file = "log_" + profile + Utils::getCurrentDate() +".csv";
		std::ofstream myfile(file, std::ios::app);
		if(!ifstream(file)) {
			cout << "created csv" << endl;
			myfile << "MCTS-Tiefe;MM-Tiefe;#Zuege Player; #Zuege Gegner;D Zeit pro Zug (ms);Spielzeit (s);#Steine;Rollouts;Limits;ProvedWin;ProvedLoss;Edges;Bonus;Sieg;\n";
		}
		myfile << mctsDepth << ";" << mmDepth << ";";
		myfile << movesPlayer << ";" << movesEnemy << ";";
		myfile << timePerMove / (double)movesPlayer << ";" << totalTime << ";"
				<< points << ";" << rollouts << ";" << limits << ";" << provedWin
				<< ";" << provedLoss << ";" << edges << ";" << bonus << ";" << endState << "\n";

		myfile.close();
	}

	/**
	 * Deletes instance of Singleton object
	 */
	~Helper() {
		if(instance != 0) {
			delete(instance);
		}
	}
};

