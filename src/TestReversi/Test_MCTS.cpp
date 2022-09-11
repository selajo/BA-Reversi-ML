#include <gtest/gtest.h>
#include "../Reversi/MCTS/MCTS.cpp"
#include "../Reversi/Utils/Helper.cpp"

char** prepareArray(char player, char enemy) {
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

TEST(BackupTest, NodeIsRoot) {
	Node* root = new Node(NULL, Game(), '1');
	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(root, 0);

	ASSERT_EQ(1, root->visits);
	ASSERT_EQ(0, root->score);
}

TEST(BackupTest, RootHasChild_AssertRoot) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	Node* n2 = new Node(n, Game(), '2');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n2, 1);

	ASSERT_EQ(1, root->visits);
	ASSERT_EQ(1, root->score);
}

TEST(BackupTest, ManyLeafs_AssertRoot) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	Node* n2 = new Node(n, Game(), '2');
	Node* n3 = new Node(n, Game(), '2');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n2, 1);
	mc->backup(n3, 1);

	ASSERT_EQ(2, root->visits);
	ASSERT_EQ(2, root->score);

}

TEST(BackupTest, ManyLeafs_AssertN) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	Node* ne = new Node(n, Game(), '2');
	Node* n2 = new Node(ne, Game(), '1');
	Node* n3 = new Node(ne, Game(), '1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n2, 1);
	mc->backup(n3, 1);

	ASSERT_EQ(2, n->visits);
	ASSERT_EQ(2, n->score);

}

TEST(BackupTest, ManyLeafs_AssertEnemy) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	Node* ne = new Node(n, Game(), '2');
	Node* n2 = new Node(ne, Game(), '1');
	Node* n3 = new Node(ne, Game(), '1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n2, 0);
	mc->backup(n3, 1);

	ASSERT_EQ(2, ne->visits);
	ASSERT_EQ(-1, ne->score);
}


TEST(BackupTest, ManyLeafs_AssertN2) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	Node* ne = new Node(n, Game(), '2');
	Node* n2 = new Node(ne, Game(), '1');
	Node* n3 = new Node(ne, Game(), '1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n2, 0);
	mc->backup(n3, 1);

	ASSERT_EQ(1, n2->visits);
	ASSERT_EQ(0, n2->score);
}

TEST(BackupTest, ManyLeafs_AssertN3) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	Node* ne = new Node(n, Game(), '2');
	Node* n2 = new Node(ne, Game(), '1');
	Node* n3 = new Node(ne, Game(), '1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n2, 0);
	mc->backup(n3, 1);

	ASSERT_EQ(1, n3->visits);
	ASSERT_EQ(1, n3->score);
}

/////// CalcUCT ///////

TEST(CalcUCTTest, OneChildOneWin) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	Node* ne = new Node(n, Game(), '2');
	Node* n2 = new Node(ne, Game(), '1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n2, 1);

	double actual = mc->calcUCT(root, n, 1);

	ASSERT_EQ(1.0, actual);
}

TEST(CalcUCTTest, OneChildZeroWins) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	Node* ne = new Node(n, Game(), '2');
	Node* n2 = new Node(ne, Game(), '1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n2, 0);

	double actual = mc->calcUCT(root, n, 1);

	ASSERT_EQ(0.0, actual);
}

TEST(CalcUCTTest, ManyChildren) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	Node* n2 = new Node(n, Game(), '2');
	Node* n3 = new Node(n, Game(), '2');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n2, 0);
	mc->backup(n3, 1);

	double actual = mc->calcUCT(root, n, (double)0.5);

	//Actual ~0.91627, but is infinite
	ASSERT_EQ(91627, (int)(actual*100000));
}

/////// BestChild ///////

TEST(BestChildTest, NoChildren) {
	Node* root = new Node(NULL, Game(), '0');
	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(root, 0);

	Node* actual = mc->bestChild(root, (double)0.5);

	ASSERT_TRUE(actual == root);
}

TEST(BestChildTest, OneChild) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	n->lastUsed = Point(1, 1);

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n, 1);

	Node* actual = mc->bestChild(root, (double)0.5);

	ASSERT_TRUE(actual == n);
}

TEST(BestChildTest, TwoChildren) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	n->lastUsed = Point(1, 1);
	Node* n2 = new Node(root, Game(), '1');
	n2->lastUsed = Point(1, 1);

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n, 0);
	mc->backup(n2, 1);

	Node* actual = mc->bestChild(root, (double)0.5);

	ASSERT_TRUE(actual == n2);
}

TEST(BestChildTest, ThreeChildren) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	n->lastUsed = Point(1, 1);
	Node* n2 = new Node(root, Game(), '1');
	n2->lastUsed = Point(1, 1);
	Node* n3 = new Node(root, Game(), '1');
	n3->lastUsed = Point(3, 3);

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n, 0);
	mc->backup(n2, 0);
	mc->backup(n3, 1);

	Node* actual = mc->bestChild(root, (double)0.5);

	ASSERT_TRUE(actual == n3);
}

/////// MakeRandomMove ///////
//TODO: Mocks


////// MakeRandomUnusedMove ///////
//TODO: Mocks


/////// Expand ///////
TEST(ExpandTest, RemoveFromVector) {
	std::vector<Point> actions;
	actions.push_back(Point(1, 1));
	actions.push_back(Point(3, 4));
	actions.push_back(Point(5, 5));

	actions.erase(std::remove(actions.begin(), actions.end(), Point(5, 5)), actions.end());

	int count = (int)std::count(actions.begin(), actions.end(), Point(5, 5));
	ASSERT_EQ(0, count);

	count = (int)std::count(actions.begin(), actions.end(), Point(1, 1));
	ASSERT_EQ(1, count);

}

TEST(ExpandTest, LastUsedPoint) {
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	char** set = prepareArray('1', '2');
	g.setMap(set);
	g.calcPossibleMoves('1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	root->g = g;
	auto moves = g.getMoves();

	Node* actual = mc->expand(root, moves);

	ASSERT_TRUE(Point(0, 0) == actual->lastUsed);
}

TEST(ExpandTest, Game) {
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	char** set = prepareArray('1', '2');
	g.setMap(set);
	g.calcPossibleMoves('1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	root->g = g;
	auto moves = g.getMoves();

	Node* actual = mc->expand(root, moves);

	g.makeMoveTo(0, 0, '1');
	ASSERT_EQ(g.mapToString(), actual->g.mapToString());
}

TEST(ExpandTest, MoveErased) {
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	char** set = prepareArray('1', '2');
	g.setMap(set);
	g.calcPossibleMoves('1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	root->g = g;
	auto moves = g.getMoves();

	Node* actual = mc->expand(root, moves);

	ASSERT_TRUE(moves.size() == 0);
}

TEST(ExpandTest, MoveEmpty) {
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	char** set = prepareArray('1', '2');
	g.setMap(set);
	g.calcPossibleMoves('1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	root->g = g;

	try {
		std::vector<Point> vec;
		Node* actual = mc->expand(root, vec);
	}
	catch (std::string& ex) {
		ASSERT_EQ(std::string("No moves available. Can not expand"), ex);
	}
}

/////// TreePolicy ///////

TEST(TreePolicyTest, AlreadyEndOfGame) {
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	char** set = prepareArray('1', '2');
	set[0][0] = '2';
	g.setMap(set);

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	root->g = g;
	auto moves = g.getMoves();

	Node* actual = mc->treePolicy(root, moves);

	ASSERT_TRUE(root == actual);
}

TEST(TreePolicyTest, Expand) {
	Game g;
	char** set = prepareArray('1', '2');
	g.setMap(set);
	g.calcPossibleMoves('1');

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);

	Node* root = new Node(NULL, g, '0');
	auto moves = g.getMoves();

	Node* actual = mc->treePolicy(root, moves);

	char** exp = prepareArray('1', '2');
	exp[0][0] = '1';
	exp[0][1] = '1';
	Game g2;
	g2.setMap(exp);
	ASSERT_EQ(g2.mapToString(), actual->g.mapToString());
}

TEST(TreePolicyTest, CheckForParent) {
	Node* root = new Node(NULL, Game(), '0');
	MCTS* mc = new MCTS_Solver('1', '2', 1.0);

	root->g.calcPossibleMoves('1');
	std::vector<Point> vec = root->g.getMoves();
	Node* actual = mc->treePolicy(root, vec);

	ASSERT_EQ(0, actual->visits);
	ASSERT_TRUE(actual->parent == root);
}

TEST(TreePolicyTest, CheckRoot) {
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	n->lastUsed = Point(1, 1);
	Node* n2 = new Node(root, Game(), '1');
	n2->lastUsed = Point(1, 1);
	Node* n3 = new Node(root, Game(), '1');
	n3->lastUsed = Point(3, 3);

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);
	mc->backup(n, 0);
	mc->backup(n2, 0);
	mc->backup(n3, 1);

	std::vector<Point> vec;
	Node* actual = mc->treePolicy(root, vec);

	ASSERT_TRUE(root == actual->parent->parent);
}

/////// DefaultPolicy ///////

TEST(DefaultPolicyTest, PlayerWon) {
	Game g;
	char** set = prepareArray('1', '2');
	g.setMap(set);

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);

	int actual = mc->defaultPolicy(g, false);
	ASSERT_EQ(1, actual);
}

TEST(DefaultPolicyTest, PlayerLost) {
	Game g;
	char** set = prepareArray('2', '1');
	g.setMap(set);

	MCTS* mc = new MCTS_Solver('1', '2', 1.0);

	int actual = mc->defaultPolicy(g, false);
	ASSERT_EQ(-1, actual);
}

/////// UCTSearch ///////

TEST(UCTSearchTest, OneStep) {
	MCTS* mc = new MCTS_Solver('1', '2', 0.75);
	Game g;
	g.calcPossibleMoves('1');
	auto moves = g.getMoves();

	auto actual = mc->uctSearch(g, 1);

	ASSERT_TRUE(std::find(moves.begin(), moves.end(), actual) != moves.end());
}

TEST(UCTSearchTest, MoreSteps) {
	MCTS* mc = new MCTS_Solver('1', '2', 0.75);
	Game g;
	g.calcPossibleMoves('1');
	auto moves = g.getMoves();

	auto actual = mc->uctSearch(g, 2);

	ASSERT_TRUE(std::find(moves.begin(), moves.end(), actual) != moves.end());
}
