#include <gtest/gtest.h>
#include "../Reversi/Utils/Node.cpp"

/////// AddChild ///////
TEST(AddChildTest, EqualVisits) {
	Node* nod = new Node(NULL, Game(), '1');
	Node* toAdd = new Node(NULL, Game(), '2');

	nod->visits = 10;
	toAdd->visits = 50;

	nod->addChild(toAdd);

	Node* test = nod->children.front();

	ASSERT_EQ(50, test->visits);
}

TEST(AddChildTest, EqualWon) {
	Node* nod = new Node(NULL, Game(), '1');
	Node* toAdd = new Node(NULL, Game(), '2');

	nod->score = 2;
	toAdd->score = 1;

	nod->addChild(toAdd);

	Node* test = nod->children.front();

	ASSERT_EQ(1, test->score);
	ASSERT_EQ(2, test->parent->score);
}

TEST(AddChildTest, Parent) {
	Node* nod = new Node(NULL, Game(), '1');
	Node* toAdd = new Node(NULL, Game(), '2');

	nod->addChild(toAdd);

	Node* test = nod->children.front();

	ASSERT_TRUE(nod == test->parent);
}

/////// Constructor ///////

TEST(ConstructorTest, AddChild) {
	Node* root = new Node(NULL, Game(), '1');
	Node* child = new Node(root, Game(), '2');

	ASSERT_TRUE(child->parent == root);
	ASSERT_TRUE(root->children.front() == child);
}

TEST(ConstructorTest, Root) {
	Node* root = new Node(NULL, Game(), '1');

	ASSERT_TRUE(root->parent == NULL);
	ASSERT_EQ(0, (int)root->children.size());
}

TEST(ConstructorTest, AddChildGameInstance) {
	Game g, g2;
	Node* root = new Node(NULL, g, '1');
	g2 = g;
	g2.calcPossibleMoves('1');
	g2.makeMoveTo(3, 5, '1');

	Node* child = new Node(root, g2, '2');

	std::string expected = "Printing map\n  "
			"0 1 2 3 4 5 6 7 \n"
			"0 0 0 0 0 0 0 0 0 \n"
			"1 0 0 0 0 0 0 0 0 \n"
			"2 0 0 0 0 0 0 0 0 \n"
			"3 0 0 0 1 2 0 0 0 \n"
			"4 0 0 0 1 1 0 0 0 \n"
			"5 0 0 0 1 0 0 0 0 \n"
			"6 0 0 0 0 0 0 0 0 \n"
			"7 0 0 0 0 0 0 0 0 \n";

	ASSERT_EQ(expected, child->g.mapToString());
}

TEST(ConstructorTest, AddChildRootGameInstance) {
	Game g, g2;
	Node* root = new Node(NULL, g, '1');
	g2 = g;
	g2.calcPossibleMoves('1');
	g2.makeMoveTo(3, 5, '1');

	Node* child = new Node(root, g2, '2');

	std::string expected = "Printing map\n  "
			"0 1 2 3 4 5 6 7 \n"
			"0 0 0 0 0 0 0 0 0 \n"
			"1 0 0 0 0 0 0 0 0 \n"
			"2 0 0 0 0 0 0 0 0 \n"
			"3 0 0 0 1 2 0 0 0 \n"
			"4 0 0 0 2 1 0 0 0 \n"
			"5 0 0 0 0 0 0 0 0 \n"
			"6 0 0 0 0 0 0 0 0 \n"
			"7 0 0 0 0 0 0 0 0 \n";

	ASSERT_EQ(expected, root->g.mapToString());
}
