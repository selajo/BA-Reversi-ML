#include <cstring>
#include <string.h>
#include <iostream>

#include "../MCTS/MCTS.h"
#include "../Utils/Utils.h"
#include "../Game/Game.h"


using namespace std;

/// <summary>Creates and holds connection to server</summary>
class Network {
private:
	int _socket;
	int bufferSize = 150;
public:
	Game g = Game();
	Network();

	int GameLoop(string ipadress, int port, Algorithm* mcts=nullptr);
	int evalMessage(char buffer[], Point &answer, Algorithm* mcts=nullptr);
};
