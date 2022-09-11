#include <gtest/gtest.h>
#include "../Reversi/MCTS/MCTS_Hybrids.cpp"
#include "../Reversi/Utils/Node.cpp"
//#include "../Reversi/Utils/Helper.cpp"
//#include "../Reversi/MCTS/MCTS_Solver.cpp"


char** getHalfFull() {
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

/////// MCTS_MR ///////

TEST(MR_DefaultPolicyTest, ObviousWin) {
	MCTS* mr = new MCTS_MR('1', '2', 0.75);
	mr->mmDepth = 5;

	Game g;
	char** in = getHalfFull();
	in[0][0] = in[7][0] = '0';
	in[0][1] = in[1][0] = in[1][1] = '2';
	g.setMap(in);

	int actual = mr->defaultPolicy(g, false);
	ASSERT_EQ(1, actual);
}

TEST(MR_DefaultPolicyTest, ObviousLoss) {
	MCTS_MR* mr = new MCTS_MR('2', '1', 0.75);
	mr->mmDepth = 5;

	Game g;
	char** in = getHalfFull();
	in[0][0] = in[7][0] = '0';
	in[0][1] = in[1][0] = in[1][1] = '2';
	g.setMap(in);

	int actual = mr->defaultPolicy(g, false);
	ASSERT_EQ(0, actual);
}

TEST(MR_DefaultPolicyTest, Draw) {
	MCTS_MR* mr = new MCTS_MR('1', '2', 0.75);
	mr->mmDepth = 1;
	Game g;

	int actual = mr->defaultPolicy(g, false);
}

/////// MCTS_MS ///////
char** prepareArray2(char player, char enemy) {
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

TEST(MS_MiniMaxAndUpdateTest, DefaultState) {
	MCTS_MS* ms = new MCTS_MS('1', '2', 0.75, 3);
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');

	ms->miniMaxAndUpdate(n, '1', '2');

	ASSERT_EQ(0, n->score);
	ASSERT_EQ(0, root->score);
}

TEST(MS_MiniMaxAndUpdateTest, ObvoiusWin) {
	MCTS_MS* ms = new MCTS_MS('1', '2', 0.75, 3);
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	g.setMap(prepareArray2('1', '2'));
	Node* n = new Node(root, g, '1');
	n->score = 1;

	ms->miniMaxAndUpdate(n, '1', '2');

	ASSERT_EQ(2, n->score);
	ASSERT_EQ(1, root->score);
}

TEST(MS_MiniMaxAndUpdateTest, ObvoiusLoss) {
	MCTS_MS* ms = new MCTS_MS('1', '2', 0.75, 3);
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	g.setMap(prepareArray2('2', '1'));
	Node* n = new Node(root, g, '1');
	n->score = -1;
	ms->miniMaxAndUpdate(n, '1', '2');

	ASSERT_EQ(-2, n->score);
	ASSERT_EQ(-1, root->score);
}

TEST(MS_TreePolicyTest, LimitNotReachedExpand) {
	MCTS_MS* ms = new MCTS_MS('1', '2', 0.75, 3);
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	n->g.calcPossibleMoves('1');

	std::vector<Point> vec = n->g.getMoves();
	Node* result = ms->treePolicy(n, vec);
	ASSERT_TRUE(result->parent == n);
}

TEST(MS_TreePolicyTest, LimitReached) {
	MCTS_MS* ms = new MCTS_MS('1', '2', 0.75, 3);
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	g.setMap(prepareArray2('1', '2'));
	Node* n = new Node(root, g, '1');
	n->visits = 10;

	std::vector<Point> vec = n->g.getMoves();
	Node* result = ms->treePolicy(n, vec);
	ASSERT_EQ(0, n->score);
	ASSERT_EQ(0, root->score);
}

TEST(MS_TreePolicyTest, Default) {
	MCTS_MS* ms = new MCTS_MS('1', '2', 0.75, 3);
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');

	n->visits = 10;
	std::vector<Point> vec = n->g.getMoves();
	Node* result = ms->treePolicy(n, vec);
	ASSERT_EQ(0, n->score);
	ASSERT_EQ(0, root->score);
}

/////// MCTS_MB ///////
TEST(MB_BackupTest, EnemyLostDefinetly) {
	Game g;
	g.setMap(prepareArray2('1', '2'));
	Node* root = new Node(NULL, Game(), '0');
	Node* first = new Node(root, g, '1');
	g.calcPossibleMoves('1');
	g.makeMoveTo(0, 0, '1');
	Node* firstEn = new Node(first, g, '2');
	Node* second = new Node(firstEn, g, '1');


	MCTS* mb = new MCTS_MB('1', '2', 0.75);
	mb->backup(second, 1);

	ASSERT_EQ(1000, second->score);
}

TEST(MB_BackupTest, EnemyLostDefinetlyOtherNodes) {
	Game g;
	g.setMap(prepareArray2('1', '2'));
	Node* root = new Node(NULL, Game(), '0');
	Node* first = new Node(root, g, '1');
	g.calcPossibleMoves('1');
	g.makeMoveTo(0, 0, '1');
	Node* firstEn = new Node(first, g, '2');
	Node* second = new Node(firstEn, g, '1');


	MCTS* mb = new MCTS_MB('1', '2', 0.75);
	mb->backup(second, 1);

	ASSERT_EQ(1000, root->score);
	ASSERT_EQ(-1000, firstEn->score);
}

TEST(MB_BackupTest, PlayerLostDefinetly) {
	Game g;
	g.setMap(prepareArray2('2', '1'));
	Node* root = new Node(NULL, Game(), '0');
	Node* first = new Node(root, g, '1');
	g.calcPossibleMoves('2');
	g.makeMoveTo(0, 0, '2');
	Node* firstEn = new Node(first, g, '2');
	Node* second = new Node(firstEn, g, '1');

	MCTS* mb = new MCTS_MB('1', '2', 0.75);
	mb->backup(second, -1);

	ASSERT_EQ(-1000, second->score);
}

TEST(MB_BackupTest, PlayerLostDefinetlyOtherNodes) {
	Game g;
	g.setMap(prepareArray2('2', '1'));
	Node* root = new Node(NULL, Game(), '0');
	Node* first = new Node(root, g, '1');
	g.calcPossibleMoves('2');
	g.makeMoveTo(0, 0, '2');
	Node* firstEn = new Node(first, g, '2');
	Node* second = new Node(firstEn, g, '1');

	MCTS* mb = new MCTS_MB('1', '2', 0.75);
	mb->backup(second, -1);

	ASSERT_EQ(-1000, root->score);
	ASSERT_EQ(1000, firstEn->score);

}

TEST(MB_BackupTest, NoWinForAnyOne) {
	Game g;
	Node* root = new Node(NULL, Game(), '0');
	Node* first = new Node(root, g, '1');
	Node* firstEn = new Node(first, g, '2');
	Node* second = new Node(firstEn, g, '1');

	MCTS* mb = new MCTS_MB('1', '2', 0.75);
	mb->backup(second, 1);

	ASSERT_EQ(1, root->score);
	ASSERT_EQ(1, second->score);
	ASSERT_EQ(-1, firstEn->score);

}

/////// MCTS_IPEM ///////
TEST(IPEM_MiniMaxAndUpdateTest, DefaultState) {
	MCTS_IPEM* ipem = new MCTS_IPEM('1', '2', 0.75, 3, 10);
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');

	ipem->miniMaxAndUpdate(n, '1', '2');

	ASSERT_EQ(0, n->score);
	ASSERT_EQ(0, root->score);
}

TEST(IPEM_MiniMaxAndUpdateTest, ObvoiusWin) {
	MCTS_IPEM* ipem = new MCTS_IPEM('1', '2', 0.75, 3, 10);
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	g.setMap(prepareArray2('1', '2'));
	Node* n = new Node(root, g, '1');

	ipem->miniMaxAndUpdate(n, '1', '2');

	ASSERT_EQ(1000, n->score);
	ASSERT_EQ(10, n->visits);
}

TEST(IPEM_MiniMaxAndUpdateTest, ObvoiusLoss) {
	MCTS_IPEM* ipem = new MCTS_IPEM('1', '2', 0.75, 3, 10);
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	g.setMap(prepareArray2('2', '1'));
	Node* n = new Node(root, g, '1');

	ipem->miniMaxAndUpdate(n, '1', '2');

	ASSERT_EQ(-1000, n->score);
	ASSERT_EQ(10, n->visits);
}

TEST(IPEM_TreePolicyTest, LimitNotReachedExpand) {
	MCTS_IPEM* ipem = new MCTS_IPEM('1', '2', 0.75, 3, 10);
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');
	n->g.calcPossibleMoves('1');

	std::vector<Point> vec = n->g.getMoves();
	Node* result = ipem->treePolicy(n, vec);
	ASSERT_TRUE(result->parent == n);
}

TEST(IPEM_TreePolicyTest, LimitReached) {
	MCTS_IPEM* ipem = new MCTS_IPEM('1', '2', 0.75, 3, 10);
	Node* root = new Node(NULL, Game(), '0');
	Game g;
	g.setMap(prepareArray2('1', '2'));
	Node* n = new Node(root, g, '1');
	n->visits = 10;

	std::vector<Point> vec = n->g.getMoves();
	Node* result = ipem->treePolicy(n, vec);
	ASSERT_EQ(1000, n->score);
	ASSERT_EQ(20, n->visits);
}

TEST(IPEM_TreePolicyTest, Default) {
	MCTS_IPEM* ipem = new MCTS_IPEM('1', '2', 0.75, 3, 10);
	Node* root = new Node(NULL, Game(), '0');
	Node* n = new Node(root, Game(), '1');

	n->visits = 10;
	std::vector<Point> vec = n->g.getMoves();
	Node* result = ipem->treePolicy(n, vec);
	ASSERT_EQ(0, n->score);
	ASSERT_EQ(0, root->score);
}

