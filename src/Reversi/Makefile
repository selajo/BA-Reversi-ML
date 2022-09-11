all:
	cython3 -3 --cplus cpy/evaluate.pyx --embed
	
	g++ -g -O2 -c Utils/Utils.cpp -o cppUtilscode.o
	g++ -g -O2 -c Game/Point.cpp -o cppPointcode.o
	g++ -g -O2 -c Game/Game.cpp -o cppGamecode.o
	g++ -g -O2 -c Utils/Node.cpp -o cppNodecode.o
	g++ -g -O2 -c MiniMax/Heuristic.cpp -o cppHeuristiccode.o
	g++ -g -O2 -c MiniMax/MiniMax.cpp -o cppMiniMaxcode.o
	g++ -g -O2 -c MCTS/MCTS.cpp -o cppMCTScode.o
	g++ -g -O2 -c MCTS/MCTS_Solver.cpp -o cppMCTSSolvercode.o
	g++ -g -O2 -c MCTS/MCTS_Hybrids.cpp -o cppMCTSHybridscode.o
	g++ -g -O2 -c DecisionMaker/DecisionMaker.cpp -o cppDecisionMakercode.o
	g++ -g -O2 -c Network/Network.cpp -o cppNetworkcode.o
	g++ -g -O2 -c Utils/Helper.cpp -o cppHelpercode.o
	
	g++ cpy/evaluate.cpp cppUtilscode.o cppPointcode.o cppGamecode.o cppNodecode.o cppHeuristiccode.o cppMiniMaxcode.o cppMCTScode.o cppMCTSSolvercode.o cppMCTSHybridscode.o cppDecisionMakercode.o cppNetworkcode.o cppHelpercode.o -I /usr/include/python3.8  -lpython3.8 -o cpy/cpyReversi 
	#cpy/cpyReversi
