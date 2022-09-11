#include <gtest/gtest.h>
#include "../Reversi/Utils/Utils.cpp"

/////// Move_score::Max ///////
TEST(MoveScore_MaxTest, Greater) {
	move_score a = move_score(Point(), 10);
	move_score b = move_score(Point(), 5);

	ASSERT_EQ(10, move_score::max(a, b).score);
}

TEST(MoveScore_MaxTest, Equal) {
	move_score a = move_score(Point(), 10);
	move_score b = move_score(Point(), 10);

	ASSERT_EQ(10, move_score::max(a, b).score);
}


/////// Move_score::Min ///////
TEST(MoveScore_MinTest, Greater) {
	move_score a = move_score(Point(), 10);
	move_score b = move_score(Point(), 5);

	ASSERT_EQ(5, move_score::min(a, b).score);
}

TEST(MoveScore_MinTest, Equal) {
	move_score a = move_score(Point(), 10);
	move_score b = move_score(Point(), 10);

	ASSERT_EQ(10, move_score::min(a, b).score);
}
