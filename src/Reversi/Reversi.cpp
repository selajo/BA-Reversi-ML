// Reversi.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.

#include "DecisionMaker/DecisionMaker.h"
//#include "DecisionMaker/DecisionMaker.cpp"
#include "Game/Game.h"
#include "MCTS/MCTS.h"
#include "MCTS/MCTS_Hybrids.h"
#include "Network/Network.h"
#include "Utils/Utils.h"
#include "Utils/Helper.h"
//#include "Utils/Helper.h"
#include <iostream>
#include <cstring>

using namespace std;


int main(int argc,char* argv[])
{
	int limit = 0, prior = 0, kBest = 0;
	double cpV = 1.0;
	char player = '0';
	int eva;
	int mctsDepth = 0;
	bool moveOrdering = false;
	bool keepTree = false;

	string ip = "", profile = "";
	int port = 0;
	vector<string> all_args;
	vector<float> heurWeights = vector<float>();
	Algorithm* mcts;
	DecisionMaker dm;
	Heuristic * heu = new Heuristic();

	if(argc > 1) {
		all_args.assign(argv + 1, argv + argc);
	}

	if(all_args.empty() || all_args.front() == "--h") {
		Utils::printHelp();
		return 0;
	}

	Utils::readArgs(all_args, port, ip, profile, player, limit, prior, cpV, mctsDepth, kBest, moveOrdering, keepTree, heurWeights);

	if(ip == "" || port == 0 || player == '0') {
		cout << "Port, ip and player number need to be specified" << endl;
		return -1;
	}
	if(profile == "") {
		cout << "No profile for player was given. Using default MCTS.\n";
	}

	char enemy = (player == '1') ? '2' : '1';

	mcts = dm.setAlgorithm(profile, player, enemy, cpV, limit, prior);

	if(!heurWeights.empty()) {
		heu->boardWeight = heurWeights[0];
		heu->cornersWeight = heurWeights[1];
		heu->mobilityWeight = heurWeights[2];
		heu->scoreWeight = heurWeights[3];
	}


	mcts->h = heu;
	mcts->mctsDepth = mctsDepth;
	mcts->keepTree = keepTree;



	Network nw;

	cout << "You are player " << player << endl;
	nw.g.setPlayer(player);

	Helper* helper = Helper::getInstance(player);
	if(mcts->h->boardWeight != 0) {
		helper->addBonus("BoardW:" + to_string(mcts->h->boardWeight) + ";MobW:" + to_string(mcts->h->mobilityWeight)
				+ ";ConerW:" + to_string(mcts->h->cornersWeight) + ";ScoreW:" + to_string(mcts->h->scoreWeight));
	}

	//Game loop
	try {
		helper->startTime();

		eva =  nw.GameLoop(ip, port, mcts);
		if(eva == 7){
			cout << "Got disqualified..." << endl;
			return 0;
		}
		else if(eva == 8 || eva == 9) {
			cout << "Reached end of game" << endl;
		}
		else{
			cout << "Some error while connecting occured..." << endl;
			return -1;
		}
	}
	catch (const std::exception &ex) {
		cout << "Exception occured: " << ex.what() << endl;
	}
	catch(const string &s) {
		cout << "Error: " << s << endl;
	}

	helper->endTime();

	float state = 0.0;
	int points = nw.g.getPlayerCount(player);

	if (points > 32) {
		cout << "I won!" << endl;
		state = 1.0;
	}
	else if (points < 32) {
		cout << "I lost..." << endl;
	}
	else {
		cout << "It's a draw!" << endl;
		state = 0;
	}

	helper->countEdges(nw.g);
	helper->writeToCSV(profile, state, mctsDepth, points);

	delete(helper);
	return 0;
}
