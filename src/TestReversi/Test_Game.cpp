#include <gtest/gtest.h>
#include "../Reversi/Game/Game.cpp"

bool compArray(char** expected, char** actual) {
	bool result = true;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (expected[i][j] != actual[i][j])
				result = false;
		}
	}

	return result;
}

/////// GetPlayerCount ///////
TEST(GetPlayerCountTest, CountReturn) {
	Game g;
	int expected = g.getPlayerCount('1');
	ASSERT_EQ(2, expected);
}

/////// CheckForEnd ///////
TEST(CheckForEndTest, XNegative) {
	Game g;
	Point p = Point(-1, 0);
	bool actual = g.checkForEnd(p);
	ASSERT_TRUE(actual);
}

TEST(CheckForEndTest, YNegative) {
	Game g;
	Point p = Point(3, -5);
	bool actual = g.checkForEnd(p);
	ASSERT_TRUE(actual);
}

TEST(CheckForEndTest, XTooBig) {
	Game g;
	Point p = Point(8, 0);
	bool actual = g.checkForEnd(p);
	ASSERT_TRUE(actual);
}

TEST(CheckForEndTest, YTooBig) {
	Game g;
	Point p = Point(2, 8);
	bool actual = g.checkForEnd(p);
	ASSERT_TRUE(actual);
}

TEST(CheckForEndTest, InBounds) {
	Game g;
	Point p = Point(2, 4);
	bool actual = g.checkForEnd(p);
	ASSERT_FALSE(actual);
}

/////// CheckForEnemy ///////
TEST(CheckForEnemyTest, OwnStone) {
	Game g;
	Point p = Point(3, 3);
	bool actual = g.checkForEnemy(p, '1');
	ASSERT_FALSE(actual);
}

TEST(CheckForEnemyTest, Enemy) {
	Game g;
	Point p = Point(3, 3);
	bool actual = g.checkForEnemy(p, '2');
	ASSERT_TRUE(actual);
}

TEST(CheckForEnemyTest, Empty) {
	Game g;
	Point p = Point(0, 0);
	bool actual = g.checkForEnemy(p, '2');
	ASSERT_FALSE(actual);
}

/////// CheckForOwnPlayer ///////
TEST(CheckForOwnPlayerTest, Enemy) {
	Game g;
	Point p = Point(3, 3);
	bool actual = g.checkForOwnPlayer(p, '2');
	ASSERT_FALSE(actual);
}

TEST(CheckForOwnPlayerTest, OwnPlayer) {
	Game g;
	Point p = Point(3, 3);
	bool actual = g.checkForOwnPlayer(p, '1');
	ASSERT_TRUE(actual);
}

/////// TryPossibleMove ///////
TEST(TryPossibleMoveTest, ReachEndOfMapImmediatly) {
	Game g;
	Point p = Point(0, 0);
	int actual = g.tryPossibleMove(p, 7, '1');
	ASSERT_EQ(0, actual);
}

TEST(TryPossibleMoveTest, ReachOwnPlayerImmediatly) {
	Game g;
	Point p = Point(3, 2);
	int actual = g.tryPossibleMove(p, 4, '1');
	ASSERT_EQ(0, actual);
}

TEST(TryPossibleMoveTest, ReachEmptyFieldImmediatly) {
	Game g;
	Point p = Point(0, 0);
	int actual = g.tryPossibleMove(p, 4, '1');
	ASSERT_EQ(0, actual);
}

TEST(TryPossibleMoveTest, ReachEnemyAndEnd) {
	Game g;

	char** expected = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		expected[i] = new char[8];
		std::memset(expected[i], '0', 8 * sizeof(bool));
	}
	expected[0][0] = '2';

	g.setMap(expected);
	Point p = Point(0, 1);
	int actual = g.tryPossibleMove(p, 0, '1');
	ASSERT_EQ(0, actual);
}

TEST(TryPossibleMoveTest, ReachEnemyAndOwnPlayer) {
	Game g;

	char** expected = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		expected[i] = new char[8];
		std::memset(expected[i], '0', 8 * sizeof(bool));
	}
	expected[0][0] = '1';
	expected[1][0] = '2';

	g.setMap(expected);
	Point p = Point(0, 2);
	int actual = g.tryPossibleMove(p, 0, '1');
	ASSERT_EQ(1, actual);
}

TEST(TryPossibleMoveTest, ReachEnemyAndEmptySlot) {
	Game g;

	char** expected = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		expected[i] = new char[8];
		std::memset(expected[i], '0', 8 * sizeof(bool));
	}
	expected[0][0] = '0';
	expected[1][0] = '2';

	g.setMap(expected);
	Point p = Point(0, 2);
	int actual = g.tryPossibleMove(p, 0, '1');
	ASSERT_EQ(0, actual);
}

TEST(TryPossibleMoveTest, ReachEnemyAndColor) {
	Game g;

	char** expected = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		expected[i] = new char[8];
		std::memset(expected[i], '0', 8 * sizeof(bool));
	}
	expected[0][0] = '1';
	expected[1][0] = '2';
	expected[2][0] = '2';
	g.setMap(expected);

	Point p = Point(0, 3);
	int actual = g.tryPossibleMove(p, 0, '1', true);
	ASSERT_EQ(2, actual);

	expected[1][0] = '1';
	expected[2][0] = '1';

	char** _actual = g.getMap();

	ASSERT_TRUE(compArray(expected, _actual));
}

/////// GetMap ///////
TEST(GetMapTest, MapReturn) {
	Game g;

	char** expected = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		expected[i] = new char[8];
		std::memset(expected[i], '0', 8 * sizeof(bool));
	}

	expected[3][3] = '1';
	expected[3][4] = '2';
	expected[4][3] = '2';
	expected[4][4] = '1';

	char** actual = g.getMap();

	ASSERT_TRUE(compArray(expected, actual));
}

/////// SetMap ///////
TEST(SetMapTest, MapReturn) {
	Game g;
	bool result = true;

	char** expected = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		expected[i] = new char[8];
		std::memset(expected[i], '0', 8 * sizeof(bool));
	}

	expected[3][3] = '2';
	expected[3][4] = '2';
	expected[4][3] = '2';
	expected[4][4] = '2';

	g.setMap(expected);
	char** actual = g.getMap();

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (expected[i][j] != actual[i][j])
				result = false;
		}
	}

	ASSERT_TRUE(result);
}

/////// MapToString ///////
TEST(MapToStringTest, Return) {
	Game g;
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

	ASSERT_EQ(expected, g.mapToString());
}

/////// GetPossibleMoves ///////
TEST(GetPossibleMovesTest, IsOk) {
	Game g;
	g.calcPossibleMoves('1');
	auto actual = g.getMoves();

	int count = (int)std::count(actual.begin(), actual.end(), Point(2, 4));

	ASSERT_EQ(1, count);
}

TEST(GetPossibleMovesTest, IsNotOk) {
	Game g;
	g.calcPossibleMoves('1');
	std::vector<Point> actual = g.getMoves();

	int count = (int)std::count(actual.begin(), actual.end(), Point(0, 0));

	ASSERT_EQ(0, count);
}

TEST(GetPossibleMovesTest, SameGoalTwice) {
	Game g;

	char** in = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		in[i] = new char[8];
		std::memset(in[i], '0', 8 * sizeof(bool));
	}

	in[0][1] = '2';
	in[0][2] = '1';
	in[1][0] = '2';
	in[2][0] = '1';

	g.setMap(in);
	g.calcPossibleMoves('1');
	std::vector<Point> actual = g.getMoves();

	int count = (int)std::count(actual.begin(), actual.end(), Point(0, 0));

	ASSERT_EQ(1, count);
}

TEST(GetPossibleMovesTest, SameGoalsMore) {
	Game g;

	char** in = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		in[i] = new char[8];
		std::memset(in[i], '1', 8 * sizeof(bool));
	}

	in[3][3] = '0';

	in[3][2] = '2';
	in[3][4] = '2';
	in[2][3] = '2';
	in[4][3] = '2';
	in[4][4] = '2';
	in[4][2] = '2';
	in[2][2] = '2';
	in[2][4] = '2';

	g.setMap(in);
	g.calcPossibleMoves('1');
	std::vector<Point> actual = g.getMoves();

	int count = (int)std::count(actual.begin(), actual.end(), Point(3, 3));

	ASSERT_EQ(1, count);
}

TEST(GetPossibleMovesTest, GetEmpyt) {
	Game g;

	char** in = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		in[i] = new char[8];
		std::memset(in[i], '1', 8 * sizeof(bool));
	}

	g.setMap(in);
	g.calcPossibleMoves('1');
	std::vector<Point> actual = g.getMoves();

	ASSERT_TRUE(actual.empty());
}

/////// PossibleMovesToSting ///////
TEST(PossibleMovesToStringTest, Return) {
	Game g;
	g.calcPossibleMoves('1');
	std::string actual = g.possibleMovesToString('1');
	std::string expected = "Possible moves of player 1:\n"
			"Point(4, 2)\n"
			"Point(5, 3)\n"
			"Point(2, 4)\n"
			"Point(3, 5)\n";

	ASSERT_EQ(expected, actual);
}

/////// CheckForMoves ///////
TEST(CheckForMovesTest, PlayerHasMoves) {
	Game g;
	g.calcPossibleMoves('1');

	bool actual = g.checkForMoves('1');

	ASSERT_TRUE(actual);
}

TEST(CheckForMovesTest, PlayerHasNoMoves) {
	Game g;

	char** in = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		in[i] = new char[8];
		std::memset(in[i], '1', 8 * sizeof(bool));
	}

	g.setMap(in);
	g.calcPossibleMoves('1');

	bool actual = g.checkForMoves('1');

	ASSERT_FALSE(actual);
}

/////// CheckForEndOfGame ///////
TEST(CheckForEndOfGameTest, BothPlayersHaveMoves) {
	Game g;

	bool actual = g.checkForEndOfGame();

	ASSERT_FALSE(actual);
}

TEST(CheckForEndOfGameTest, NoOneHasMoves) {
	Game g;
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

	g.setMap(in);

	bool actual = g.checkForEndOfGame();

	ASSERT_TRUE(actual);
}

/////// MakeMoveTo ///////
TEST(MakeMoveToTest, NoLegalMove) {
	Game g;
	g.makeMoveTo(0, 0, '1');

	char** expected = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		expected[i] = new char[8];
		std::memset(expected[i], '0', 8 * sizeof(bool));
	}

	expected[3][3] = '1';
	expected[3][4] = '2';
	expected[4][3] = '2';
	expected[4][4] = '1';

	char** actual = g.getMap();

	ASSERT_TRUE(compArray(expected, actual));
}

TEST(MakeMoveToTest, LegalMove) {
	Game g;
	g.calcPossibleMoves('1');
	g.makeMoveTo(5, 3, '1');

	char** expected = new char* [8];
	for (int i = 0; i < 8; i++)
	{
		expected[i] = new char[8];
		std::memset(expected[i], '0', 8 * sizeof(bool));
	}

	expected[3][3] = '1';
	expected[3][5] = '1';
	expected[3][4] = '1';
	expected[4][3] = '2';
	expected[4][4] = '1';

	char** actual = g.getMap();

	ASSERT_TRUE(compArray(expected, actual));
}
