#include "Network.h"

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "../Utils/Helper.h"

/**
 * Creates instance
 */
Network::Network() {
	this->g = Game(8);
}

/**
 * Evaluates message from server.
 * According to: Peter Paulus, Johannes Schmid, Rebekka Seidenschwand
 *
 * @param buffer Message from server
 * @param answer Calculated move
 * @param mcts Algorithm to calculate move
 * @return type of message
 */
int Network::evalMessage(char buffer[], Point &answer, Algorithm* mcts) {
	Helper* helper = Helper::getInstance();
	int type = (int)buffer[0];
			uint32_t _length = uint8_t(buffer[1]) << 24 | uint8_t(buffer[2]) << 16 | uint8_t(buffer[3]) << 8 | uint8_t(buffer[4]);
			cout << "Type: " << type;
			cout << "; Length of msg: " << _length << endl;

	if(_length < 0 || _length > 1000)
		return 0;

	if(type== 2){
		cout << "Read map" << endl;
	}
	else if(type ==  3) {
		//Noch nicht gebraucht, aber Player zuweisen
		g.setPlayer('0' + buffer[5]);
		//return 3;
	}
	else if(type ==  4){
		//Zugaufforderung
		uint32_t time_ = uint8_t(buffer[5]) << 24 | uint8_t(buffer[6]) << 16 | uint8_t(buffer[7]) << 8 | uint8_t(buffer[8]);
		cout << "DOTO: set timer: " << time_ << endl;

		uint32_t steps;
		if(time_ == 0) {
			steps = uint8_t(buffer[9]);
			cout << steps << " steps available" << endl;
			helper->setMMDepth(steps);
		}
		helper->startMiniTimer();

		mcts->mmDepth = steps;
		Point p = mcts->uctSearch(g, mcts->mctsDepth);

		helper->endMiniTimer();
		//Point p = g.calculateMove();
		std::cout << "Calculated move: " << p.toString() << std::endl;
		answer = p;
		return 4;
	}
	else if(type ==  6){
		//Spielzug
		char player = '0' + buffer[10];

		std::cout << "Got player" << player << " to " << (int)buffer[6] << ", " << (int)buffer[8] << std::endl;

		if(player == helper->player)
			helper->addMovePlayer();
		else
			helper->addMoveEnemy();

		g.calcPossibleMoves(player);
		g.makeMoveTo(buffer[6], buffer[8], player);
		if(player != g.getPlayer())
			mcts->recentMoves.push_back(Point(buffer[6], buffer[8]));

		cout << g.mapToString() << endl;
	}
	else if(type ==  7){
		//Disqualifikation
		return 7;
	}
	else if(type==  8){
		//Enden Phase 1
		cout << "This is the end" << endl;
		return 8;
	}
	else if(type ==  9){
		//Spielende
		return 9;
	}
	return 0;
}

/**
 * Connect and holds connection to server. And sends moves calculated by Algorithm
 * @param ipadress IP of server
 * @param port Port for connection
 * @param mcts Algorithm to calculate move
 * @return -1: Connection failed, 8,9: End of game, 7: Disqualificaiton
 */
int Network::GameLoop(string ipadress, int port, Algorithm* mcts) {
	int status = 0;
	Helper* helper = Helper::getInstance();

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		return 1;
	}
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipadress.c_str(), &hint.sin_addr);

	// Connect to the server on socket

	int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if( connectRes == -1) {
		cout << "Can not connect to server" << endl;
		return 1;
	}
	char buffer[1000];

	uint8_t in[6] =  {0x1, 0x0, 0x0, 0x0, 0x1, 0x1};
	int sendRes = send(sock, in, 6, 0);
	if(sendRes == -1) {
		cout << "Could not send to server" << endl;
		return -1;
	}

	//Starting Game Loop
	while(true) {
		std::memset(buffer, 0, 1000);
		for(int i = 0; i < 1000; i++) {
			buffer[i] = 0;
		}

		int result =  read(sock, buffer, 1000);

		Point answer;
		int ret = evalMessage(buffer, answer, mcts);

		if(ret == 4) {
			uint8_t message[10] = {0x5, 0x0, 0x0, 0x0, 0x5};
			message[6] = answer.x;
			message[8] = answer.y;
			send(sock, (const void *)&message, 10, 0);
		}
		else if(ret == 7 || ret == 8 || ret == 9) {
			status = ret;
			break;
		}
		else {
			continue;
		}
	}
	cout << "Close connection..." << endl;
	//close Socket
	close(sock);

	return status;
}
