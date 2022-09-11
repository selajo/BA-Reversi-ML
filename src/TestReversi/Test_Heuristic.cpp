#include <gtest/gtest.h>
#include "../Reversi/MiniMax/Heuristic.cpp"

/////// Constructor ///////

TEST(GetStaticVectorTest, ValuesInStaticBoard_first) {
	Heuristic h;
	auto actual = h.getStaticBoard();

	vector<int> expected = {4, -3, 2, 2, 2, 2, -3, 4};

	ASSERT_EQ(expected, actual[0]);
	ASSERT_EQ(expected, actual[7]);
}

TEST(GetStaticVectorTest, ValuesInStaticBoard_second) {
	Heuristic h;
	auto actual = h.getStaticBoard();

	vector<int> expected = {-3, -4, -1, -1, -1, -1, -4, -3};

	ASSERT_EQ(expected, actual[1]);
	ASSERT_EQ(expected, actual[6]);
}

TEST(GetStaticVectorTest, ValuesInStaticBoard_third) {
	Heuristic h;
	auto actual = h.getStaticBoard();

	vector<int> expected = {2, -1, 1, 0, 0, 1, -1, 2};

	ASSERT_EQ(expected, actual[2]);
	ASSERT_EQ(expected, actual[5]);
}

TEST(GetStaticVectorTest, ValuesInStaticBoard_fourth) {
	Heuristic h;
	auto actual = h.getStaticBoard();

	vector<int> expected = {2, -1, 0, 1, 1, 0, -1, 2};

	ASSERT_EQ(expected, actual[3]);
	ASSERT_EQ(expected, actual[4]);
}

/////// CalcStaticBoard ///////

TEST(CalcStaticBoardTest, DefaultGame) {
	Game g;
	Heuristic h;

	int actual = h.calcStaticBoard(g.getMap(), '1');

	ASSERT_EQ(2, actual);
}

TEST(CalcStaticBoardTest, OneMoveMade) {
	Game g;
	g.calcPossibleMoves('1');
	g.makeMoveTo(4, 2, '1');
	Heuristic h;

	int actual = h.calcStaticBoard(g.getMap(), '1');

	ASSERT_EQ(3, actual);
}

TEST(CalcStaticBoardTest, SomeConersCaptured) {
	Game g;
	Heuristic h;

	char** map = g.getMap();
	map[0][0] = map[0][7] = '1';
	g.setMap(map);

	int actual = h.calcStaticBoard(g.getMap(), '1');

	ASSERT_EQ(10, actual);
}

TEST(CalcStaticBoardTest, BadSpots) {
	Game g;
	Heuristic h;

	char** map = g.getMap();
	map[1][1] = map[6][7] = '1';
	g.setMap(map);

	int actual = h.calcStaticBoard(g.getMap(), '1');

	ASSERT_EQ(-5, actual);
}

TEST(CalcStaticBoardTest, CriticalState) {
	Game g;
	Heuristic h;

	char** map = g.getMap();
	map[0][0] = map[1][1] = '1';
	g.setMap(map);

	int actual = h.calcStaticBoard(g.getMap(), '1');

	ASSERT_EQ(2, actual);
}


/////// Mobility ///////

TEST(MobilityTest, DefaultState) {
	Game g;
	Heuristic h;

	float actual = h.mobility(g, '1', '2');

	ASSERT_EQ(0.0, actual);
}

TEST(MobilityTest, OneMoveMade) {
	Game g;
	g.calcPossibleMoves('1');
	g.makeMoveTo(4, 2, '1');
	Heuristic h;

	float actual = h.mobility(g, '1', '2');

	ASSERT_EQ(0.0, actual);
}

TEST(MobilityTest, CornerPositionPos) {
	Game g;
	Heuristic h;

	char** map = g.getMap();
	map[0][0] = '1';
	map[0][1] = map[1][1] = map[1][0] = '2';
	g.setMap(map);

	float actual = h.mobility(g, '1', '2');

	ASSERT_EQ(27, (int)actual);
}

TEST(MobilityTest, CornerPositionNeg) {
	Game g;
	Heuristic h;

	char** map = g.getMap();
	map[0][0] = '2';
	map[0][1] = map[1][1] = map[1][0] = '1';
	g.setMap(map);

	float actual = h.mobility(g, '1', '2');

	ASSERT_EQ(-27, (int)actual);
}

/////// CornersCaptured ///////
TEST(CornersCapturedTest, NoCornerCaptured) {
	Game g;
	Heuristic h;

	float actual = h.cornersCaptured(g, '1', '2');

	ASSERT_EQ(0.0, actual);
}

TEST(CornersCapturedTest, AllCornersCaptured) {
	Game g;
	Heuristic h;

	char** map = g.getMap();
	map[0][0] = map[0][7] = map[7][0] = map[7][7] = '1';
	g.setMap(map);

	float actual = h.cornersCaptured(g, '1', '2');

	ASSERT_EQ(100.0, actual);
}

TEST(CornersCapturedTest, AllCornersCapturedEnemy) {
	Game g;
	Heuristic h;

	char** map = g.getMap();
	map[0][0] = map[0][7] = map[7][0] = map[7][7] = '2';
	g.setMap(map);

	float actual = h.cornersCaptured(g, '1', '2');

	ASSERT_EQ(-100.0, actual);
}

/////// CalcHeuristic ///////
TEST(CalcHeuristicTest, DefaultState) {
	Game g;
	Heuristic h;

	float actual = h.calcHeuristic(g, '1', '2');

	ASSERT_EQ(50, (int)(actual*100));
}

TEST(CalcHeuristicTest, CornerPositionPos) {
	Game g;
	Heuristic h = Heuristic(1.0, 2.0, 3.0);

	char** map = g.getMap();
	map[0][0] = '1';
	map[0][1] = map[1][1] = map[1][0] = '2';
	g.setMap(map);

	float actual = h.calcHeuristic(g, '1', '2');

	ASSERT_EQ(83, (int)actual);
}

TEST(CalcHeuristicTest, CornerPositionNeg) {
	Game g;
	Heuristic h = Heuristic(1.0, 2.0, 3.0);

	char** map = g.getMap();
	map[0][0] = '2';
	map[0][1] = map[1][1] = map[1][0] = '1';
	g.setMap(map);

	float actual = h.calcHeuristic(g, '1', '2');

	ASSERT_EQ(-84, (int)actual);
}
