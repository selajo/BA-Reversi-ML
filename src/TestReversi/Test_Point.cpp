#include <gtest/gtest.h>
#include "../Reversi/Game/Point.cpp"

TEST(GetDeltaYTest, GoUp0) {
	Point p;
	int actual = p.getDeltaY(0);
	ASSERT_EQ(-1, actual);
}


TEST(GetDeltaYTest, GoUp1) {
        Point p;
        int actual = p.getDeltaY(1);
        ASSERT_EQ(-1, actual);
}

TEST(GetDeltaYTest, GoUp7) {
        Point p;
        int actual = p.getDeltaY(7);
        ASSERT_EQ(-1, actual);
}

TEST(GetDeltaYTest, GoRight2) {
        Point p;
        int actual = p.getDeltaY(2);
        ASSERT_EQ(0, actual);
}

TEST(GetDeltaYTest, GoLeft6) {
        Point p;
        int actual = p.getDeltaY(6);
        ASSERT_EQ(0, actual);
}

TEST(GetDeltaYTest, GoDown3) {
        Point p;
        int actual = p.getDeltaY(3);
        ASSERT_EQ(1, actual);
}

TEST(GetDeltaYTest, GoDown4) {
        Point p;
        int actual = p.getDeltaY(4);
        ASSERT_EQ(1, actual);
}

TEST(GetDeltaYTest, GoDown5) {
        Point p;
        int actual = p.getDeltaY(5);
        ASSERT_EQ(1, actual);
}

/////// GetDeltaX ///////

TEST(GetDeltaXTest, GoUp0) {
        Point p;
	int actual = p.getDeltaX(0);
        ASSERT_EQ(0, actual);
}

TEST(GetDeltaXTest, GoUp1) {
        Point p;
        int actual = p.getDeltaX(1);
        ASSERT_EQ(1, actual);
}

TEST(GetDeltaXTest, GoUp7) {
        Point p;
        int actual = p.getDeltaX(7);
        ASSERT_EQ(-1, actual);
}

TEST(GetDeltaXTest, GoRight2) {
        Point p;
        int actual = p.getDeltaX(2);
        ASSERT_EQ(1, actual);
}

TEST(GetDeltaXTest, GoLeft6) {
        Point p;
        int actual = p.getDeltaX(6);
        ASSERT_EQ(-1, actual);
}

TEST(GetDeltaXTest, GoDown3) {
        Point p;
        int actual = p.getDeltaX(3);
        ASSERT_EQ(1, actual);
}

TEST(GetDeltaXTest, GoDown4) {
        Point p;
        int actual = p.getDeltaX(4);
        ASSERT_EQ(0, actual);
}

TEST(GetDeltaXTest, GoDown5) {
        Point p;
        int actual = p.getDeltaX(5);
        ASSERT_EQ(-1, actual);
}

/////// AddDirection ///////

TEST(AddDirectionTest, Up0) {
	Point p = Point(1, 1);
	p.addDirection(0);
	ASSERT_EQ(1, p.x);
	ASSERT_EQ(0, p.y);
}

TEST(AddDirectionTest, Up1) {
        Point p = Point(1, 1);
        p.addDirection(1);
        ASSERT_EQ(2, p.x);
        ASSERT_EQ(0, p.y);
}

TEST(AddDirectionTest, Right2) {
        Point p = Point(1, 1);
        p.addDirection(2);
        ASSERT_EQ(2, p.x);
        ASSERT_EQ(1, p.y);
}

TEST(AddDirectionTest, Down3) {
        Point p = Point(1, 1);
        p.addDirection(3);
        ASSERT_EQ(2, p.x);
        ASSERT_EQ(2, p.y);
}

TEST(AddDirectionTest, Down4) {
        Point p = Point(1, 1);
        p.addDirection(4);
        ASSERT_EQ(1, p.x);
        ASSERT_EQ(2, p.y);
}

TEST(AddDirectionTest, Down5) {
        Point p = Point(1, 1);
        p.addDirection(5);
        ASSERT_EQ(0, p.x);
        ASSERT_EQ(2, p.y);
}

TEST(AddDirectionTest, Left6) {
        Point p = Point(1, 1);
        p.addDirection(6);
        ASSERT_EQ(0, p.x);
        ASSERT_EQ(1, p.y);
}

TEST(AddDirectionTest, Up7) {
        Point p = Point(1, 1);
        p.addDirection(7);
        ASSERT_EQ(0, p.x);
        ASSERT_EQ(0, p.y);
}

TEST(AddDirectionTest, UnknwonDirection) {
        Point p = Point(1, 1);
        try {
		p.addDirection(8);
	}
	catch (const std::string &ex) {
		ASSERT_EQ(std::string("Unknown direction found"), ex);
	}
}


















int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
