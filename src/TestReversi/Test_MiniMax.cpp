#include <gtest/gtest.h>
#include "../Reversi/MiniMax/MiniMax.cpp"

char** prepareArray_3(char player, char enemy) {
	char** in = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		in[i] = new char[8];
		std::memset(in[i], player, 8 * sizeof(bool));
	}
	in[0][0] = '0';
	in[0][1] = enemy;

	return in;
}

/////// MiniMax //////
TEST(MiniMaxTest, ZeroDepth) {
	Node* n = new Node(NULL, Game(), '1');

	MiniMax mm = MiniMax('1', '2', new Heuristic());

	auto actual = mm.miniMax(n, 0, '1', '2');

	ASSERT_EQ(0, actual.score);
}

TEST(MiniMaxTest, DefaultDraw) {
	Node* n = new Node(NULL, Game(), '1');
	MiniMax mm = MiniMax('1', '2', new Heuristic());

	auto actual = mm.miniMax(n, 3, '1', '2');

	ASSERT_EQ(26, actual.score);
}

TEST(MiniMaxTest, ObviousWin) {
	Node* n = new Node(NULL, Game(), '1');
	n->g.setMap(prepareArray_3('1', '2'));

	MiniMax mm = MiniMax('1', '2', new Heuristic());

	auto actual = mm.miniMax(n, 1, '1', '2');

	ASSERT_EQ(50, actual.score);
}

TEST(MiniMaxTest, ObviousLoss) {
	Node* n = new Node(NULL, Game(), '1');
	n->g.setMap(prepareArray_3('2', '1'));

	MiniMax mm = MiniMax('1', '2', new Heuristic());

	auto actual = mm.miniMax(n, 1, '1', '2');

	ASSERT_EQ(-50, actual.score);
}

/////// BestMove ///////
TEST(BestMoveTest, ObviousWin) {
	Node* n = new Node(NULL, Game(), '1');
	n->g.setMap(prepareArray_3('1', '2'));

	MiniMax mm = MiniMax('1', '2', new Heuristic());

	move_score actual = mm.bestMove(n, 1, '1', '2');

	ASSERT_EQ(0, actual.p.x);
	ASSERT_EQ(0, actual.p.y);
}

TEST(BestMoveTest, DefaultDraw) {
	Node* n = new Node(NULL, Game(), '1');

	MiniMax mm = MiniMax('1', '2', new Heuristic());

	move_score actual = mm.bestMove(n, 3, '1', '2');

	ASSERT_EQ(4, actual.p.x);
	ASSERT_EQ(2, actual.p.y);
}

/////// AlphaBetaPruning ///////
TEST(AlphaBetaTest, ObviousWin) {
	Node* n = new Node(NULL, Game(), '1');
	n->g.setMap(prepareArray_3('1', '2'));

	AlphaBeta_ES* mm = new AlphaBeta_ES('1', '2', new Heuristic());

	mm->createTree(n, 6, '1', '2');
	Point p;
	auto actual = mm->alphaBeta(n->g, 5, move_score(Point(), -1000), move_score(Point(), 1000), '1', '2',p);

	ASSERT_EQ(10, actual.score);
}

TEST(AlphaBetaTest, ObviousLoss) {
	Node* n = new Node(NULL, Game(), '1');
	n->g.setMap(prepareArray_3('2', '1'));

	AlphaBeta_ES* mm = new AlphaBeta_ES('1', '2', new Heuristic());

	mm->createTree(n, 5, '1', '2');

	//n->printFromParent();
	//cout << endl;
	Point p;
	auto actual = mm->alphaBeta(n->g, 5, move_score(Point(), -1000), move_score(Point(), 1000), '1', '2', p);

	ASSERT_EQ(-5000, actual.score);
}

TEST(AlphaBetaTest, ZeroDepth) {
	Node* n = new Node(NULL, Game(), '1');

	AlphaBeta_ES* mm = new AlphaBeta_ES('1', '2', new Heuristic());

	mm->createTree(n, 3, '1', '2');
	Point p;
	auto actual = mm->alphaBeta(n->g, 1, move_score(Point(), -1000), move_score(Point(), 1000), '1', '2', p);

	ASSERT_EQ(0, actual.score);
}

TEST(AlphaBetaTest, DefaultDraw) {
	Node* n = new Node(NULL, Game(), '1');

	AlphaBeta_ES* mm = new AlphaBeta_ES('1', '2', new Heuristic());

	mm->createTree(n, 6, '1', '2');
	Point p;
	auto actual = mm->alphaBeta(n->g, 5, move_score(Point(), -1000), move_score(Point(), 1000), '1', '2',p);

	ASSERT_EQ(-10, actual.score);
}

/////// BestMovePruning ///////
TEST(BestMovePruningTest, ObviousWin) {
	Node* n = new Node(NULL, Game(), '1');
	n->g.setMap(prepareArray_3('1', '2'));

	AlphaBeta_ES* mm = new AlphaBeta_ES('1', '2', new Heuristic());

	move_score actual = mm->bestMove(n->g, 1, '1', '2');

	ASSERT_EQ(0, actual.p.x);
	ASSERT_EQ(0, actual.p.y);
}

TEST(BestMovePruningTest, DefaultDraw) {
	Node* n = new Node(NULL, Game(), '1');

	AlphaBeta_ES* mm = new AlphaBeta_ES('1', '2', new Heuristic());

	move_score actual = mm->bestMove(n->g, 3, '1', '2');

	ASSERT_EQ(3, actual.p.x);
	ASSERT_EQ(5, actual.p.y);
}

/////// MiniMaxEndState ///////
char** prepareArrayWin(char player) {
	char** in = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		in[i] = new char[8];
		std::memset(in[i], player, 8 * sizeof(bool));
	}

	return in;
}

char** getHalfFull_2() {
	char** in = new char* [8];
	for (int i = 0; i < 4; i++)
	{
		in[i] = new char[8];
		std::memset(in[i], '1', 8 * sizeof(bool));
	}
	for (int i = 4; i < 8; i++)
	{
		in[i] = new char[8];
		std::memset(in[i], '2', 8 * sizeof(bool));
	}
	return in;
}

/////// EvaluateState ///////
TEST(EvaluateStateTest, ObviousWin) {
	Game g;
	g.setMap(prepareArrayWin('1'));
	Node* n = new Node(NULL, g, '1');

	MiniMax* mm = new MiniMaxEndState('1', '2', new Heuristic());
	int actual = mm->evaluateState(n, '1', '2');

	ASSERT_EQ(50, actual);
}

TEST(EvaluateStateTest, ObviousLoss) {
	Game g;
	g.setMap(prepareArrayWin('1'));
	Node* n = new Node(NULL, g, '1');

	MiniMax* mm = new MiniMaxEndState('2', '1', new Heuristic());
	int actual = mm->evaluateState(n, '2', '1');

	ASSERT_EQ(-50, actual);
}

TEST(EvaluateStateTest, ObviousDraw) {
	Game g;
	Node* n = new Node(NULL, g, '1');

	MiniMax* mm = new MiniMaxEndState('2', '1', new Heuristic());
	int actual = mm->evaluateState(n, '2', '1');

	ASSERT_EQ(0, actual);
}

/////// BestMove ///////
TEST(End_BestMoveTest, ObviousDraw) {
	Game g;
	Node* n = new Node(NULL, g, '1');

	MiniMax* mm = new MiniMaxEndState('2', '1', new Heuristic());
	move_score actual = mm->bestMove(n, 3, '2', '1');

	ASSERT_EQ(-14, actual.score);
}

TEST(End_BestMoveTest, ObviousWin) {
	Game g;
	char** in = getHalfFull_2();
	in[0][0] = in[7][0] = '0';
	in[0][1] = in[1][0] = in[1][1] = '2';
	g.setMap(in);
	Node* n = new Node(NULL, g, '1');

	MiniMax* mm = new MiniMaxEndState('1', '2',  new Heuristic());
	move_score actual = mm->bestMove(n, 3, '1', '2');

	ASSERT_EQ(18, actual.score);
}

TEST(End_BestMoveTest, ObviousLoss) {
	Game g;
	char** in = getHalfFull_2();
	in[0][0] = in[7][0] = '0';
	in[0][1] = in[1][0] = in[1][1] = '2';
	g.setMap(in);
	Node* n = new Node(NULL, g, '1');

	MiniMax* mm = new MiniMaxEndState('2', '1',  new Heuristic());
	move_score actual = mm->bestMove(n, 3, '2', '1');

	ASSERT_EQ(-18, actual.score);
}

/////// MoveOrdering ///////
TEST(MoveOrderingTest, Descending) {
	Node* root = new Node(NULL, Game(), '1');
	Node* n = new Node(root, Game(), '1');
	n->score = 10;
	Node* n2 = new Node(root, Game(), '1');
	n2->score = 100;

	MiniMax mm = MiniMax('1', '2', new Heuristic());
	mm.moveOrdering(n2, true);

	ASSERT_EQ(root->children.front(), n2);
}

TEST(MoveOrderingTest, Ascending) {
	Node* root = new Node(NULL, Game(), '1');
	Node* n = new Node(root, Game(), '1');
	n->score = 100;
	Node* n2 = new Node(root, Game(), '1');
	n2->score = 50;

	MiniMax mm = MiniMax('1', '2', new Heuristic());
	mm.moveOrdering(n2, false);

	ASSERT_EQ(root->children.front(), n2);
}

////// k-best Pruning ///////
TEST(kBestest, DefaultStateWithKBest) {
	Node* n = new Node(NULL, Game(), '1');

	AlphaBeta_ES* mm = new AlphaBeta_ES('1', '2', new Heuristic(),2);

	mm->createTree(n, 3, '1', '2');
	Point p;
	auto result = mm->alphaBeta(n->g, 2, move_score(Point(), -1000), move_score(Point(), 1000), '1', '2', p);



	ASSERT_EQ(4, n->children.size());
	ASSERT_EQ(3, n->children.front()->children.size());
}

TEST(kBestest, DefaultStateWithKBest_Values) {
	Node* n = new Node(NULL, Game(), '1');

	AlphaBeta_ES* mm = new AlphaBeta_ES('1', '2',new Heuristic(), 2);
	mm->createTree(n, 3, '1', '2');
	Point p;
	mm->alphaBeta(n->g, 2, move_score(Point(), -1000), move_score(Point(), 1000), '1', '2', p);

	ASSERT_EQ(15, n->children[0]->score);
	ASSERT_EQ(15, n->children[1]->score);
	ASSERT_EQ(0, n->children.front()->children[0]->score);
}
/*
TEST(kBestest, DefaultStateWithoutKBest) {
	Node* n = new Node(NULL, Game(), '1');

	AlphaBeta_ES* mm = new AlphaBeta_ES('1', '2', 0);

	mm->createTree(n, 3, '1', '2');
	Point p;
	mm->alphaBeta(n->g, 2, move_score(Point(), -1000), move_score(Point(), 1000), '1', '2', p);

	ASSERT_EQ(4, n->children.size());
	ASSERT_EQ(3, n->children.front()->children.size());
}*/
