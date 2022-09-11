#define _CRT_SECURE_NO_WARNINGS

#include "Utils.h"
/// <summary>
/// Converts an char array into a std::string.
/// </summary>
/// <param name="a"></param>
/// <param name="size">(of array a)</param>
/// <returns>Std::string converted from given array</returns>
string Utils::convertToString(const char* a, int size) {
	string s = "";
	for (int i = 0; i < size; i++) {
		try {
			s += a[i];
		}
		catch (...)
		{
			break;
		}
	}
	return s;
}

/// <summary>
/// Calculates the current date in form of yyyymmdd.
/// </summary>
/// <returns>Current date</returns>
string Utils::getCurrentDate() {

	time_t now = std::time(0);
	tm* ltm = std::localtime(&now);

	string date = "";
	date += to_string(1900 + ltm->tm_year);
	date += to_string(1 + ltm->tm_mon);
	if (ltm->tm_mday < 10) {
		date += "0" + to_string(ltm->tm_mday);
	}
	else {
		date += to_string(ltm->tm_mday);
	}

	return date;
}

//https://en.cppreference.com/w/cpp/chrono/c/tm

/// <summary>
/// Calculates the current time in form of h m s.
/// </summary>
/// <returns>Current time</returns>
std::string Utils::getCurrentTime() {
	time_t now = time(0);
	tm* ltm = localtime(&now);

	string time = "";
	time += to_string(ltm->tm_hour) + "h";
	time += to_string(ltm->tm_min) + "m";
	time += to_string(ltm->tm_sec) + "s";

	return time;
}

/**
 * Converts hexa to decimale
 * @param hexa Hex value
 * @return Decimal of hexa
 */
int Utils::hextodec(uint32_t hexa) {
	stringstream stream;
	stream << hexa;
	stream >> hex >> hexa;
	return hexa;
}

/**
 * Prints help of command line parameters
 */
void Utils::printHelp() {
	string help = "These commands can be used:\n";
	help += "--i     Set ip-address\n";
	help += "--h     Prints help\n";
	help += "--p     Set port\n";
	help += "--pl    Setting player number\n";
	help += "--pr    Profile player should use\n\n";
	help += "Profiles:\n";
	help += "MR      MCTS with MiniMax in rollout\n";
	help += "MS      MCTS with MiniMax in selection & expansion\n";
	help += "  --lim      Limit of visits\n";
	help += "MB      MCTS with MiniMax in backpropagation\n";
	help += "IPEM     MCTS with MiniMax and informed priors\n";
	help += "  --lim      Limit of visits\n";
	help += "  --prior    Prior for backpropagation\n";
	help += "\n";
	cout << help << endl;
}

/**
 * Reads command line parameters
 * @param all_args
 * @param port
 * @param ip
 * @param profile
 * @param player
 * @param limit
 * @param prior
 * @param cpV
 * @param mDepth
 * @param kBest
 * @param moveOrdering
 * @param keepTree
 * @param heurWeights
 */
void Utils::readArgs(vector<string> all_args, int &port, string &ip, string &profile, char &player,  int &limit, int &prior, double &cpV, int &mDepth, int &kBest, bool &moveOrdering, bool &keepTree, vector<float> &heurWeights) {
	for(int i = 0; i < all_args.size(); i+=2) {
		if(all_args[i] == "--p") {
			port =  atoi(all_args[i + 1].c_str());
			cout << "Got Port: " << port << endl;
		}
		else if(all_args[i] == "--i") {
			ip = all_args[i + 1];
			cout << "Got IP: " << ip << endl;
		}
		else if(all_args[i] == "--pr") {
			profile = all_args[i + 1];
			limit =  atoi(all_args[i + 2].c_str());
			prior =  atoi(all_args[i + 3].c_str());
			cpV = atof(all_args[i + 4].c_str());
			i += 3;

			cout << "Profile: " << profile << " limit " << limit;
			cout << " prior " << prior << " cp " << cpV << endl;
		}
		else if(all_args[i] == "--pl") {
			player = all_args[i + 1].c_str()[0];
		}
		else if(all_args[i] == "--mmDepth") {
			mDepth =  atoi(all_args[i + 1].c_str());
			cout << "Got mctsDepth: " << mDepth << endl;
		}
		else if(all_args[i] == "--kBest") {
			kBest =  atoi(all_args[i + 1].c_str());
			cout << "Got kBest: " << kBest << endl;
		}
		else if(all_args[i] == "--moveOrdering") {
			if(all_args[i + 1] == "true")
				moveOrdering = true;
			cout << "Got moveOrdering: " << moveOrdering << endl;
		}
		else if(all_args[i] == "--keepTree") {
			if(all_args[i + 1] == "true")
				keepTree = true;
			cout << "Got keepTree: " << keepTree << endl;
		}
		else if(all_args[i] == "--heur") {
			heurWeights.push_back(atof(all_args[i + 1].c_str()));
			heurWeights.push_back(atof(all_args[i + 2].c_str()));
			heurWeights.push_back(atof(all_args[i + 3].c_str()));
			heurWeights.push_back(atof(all_args[i + 4].c_str()));

			cout << "Got weights for heuristic " << heurWeights[0] << endl;
			i += 3;
		}
	}
}

/**
 * Removes duplicates of vector
 * @param v vector holding moves
 */
void Utils::removeDup(vector<Point> &v)
{
    auto end = v.end();
    for (auto it = v.begin(); it != end; ++it) {
        end = std::remove(it + 1, end, *it);
    }

    v.erase(end, v.end());
}


