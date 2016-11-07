#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	struct A
	{
		bool b;
	};

	struct B
	{
		bool b;
		char c;
	};

	struct C
	{
		char c;
		bool b;
	};

	struct D
	{
		bool b;
		int i;
	};

	struct E
	{
		int i;
		bool b;
	};

	struct F
	{
		int i;
		short s;
		bool b;
		// s and b are packed in 4 bytes.
	};

	struct G
	{
		int i;
		short s;
		bool b;
		char c;
		// We can pack one more byte.
	};

	struct H
	{
		int i;
		short s;
		bool b;
		char c;
		bool b1; // This b1 will be in next 4 bytes.
	};

	struct J
	{
		int i;
		short s;
		bool b;
		bool b1; // It's no matter, that b and b1 are only 1 bit - they take the whole byte.
		char c;
	};

	struct I
	{
		bool a;
		bool b;
		bool c;
		bool d;
		bool e;
	};

	struct K
	{
		short s;
		int i;
		char c;
		float f;
		double d;
	};

	struct L
	{
		short s;
		char c;
		int i;
		float f;
		double d;
	};

	struct M
	{
		double d;
		short s;
		char c;
		int i;
		float f;
	};

	struct alignas(32) O
	{
		double d;
		short s;
		char c;
		int i;
		float f;
	};
}

TEST(Alignment, 1)
{
	EXPECT_EQ(1, sizeof(A));
	EXPECT_EQ(2, sizeof(B));
	EXPECT_EQ(2, sizeof(C));
	EXPECT_EQ(8, sizeof(D));
	EXPECT_EQ(8, sizeof(E));

	EXPECT_EQ(8, sizeof(F));
	EXPECT_EQ(4, offsetof(F, F::s));
	EXPECT_EQ(6, offsetof(F, F::b));

	EXPECT_EQ(8, sizeof(G));
	EXPECT_EQ(4, offsetof(G, G::s));
	EXPECT_EQ(6, offsetof(G, G::b));
	EXPECT_EQ(7, offsetof(G, G::c));

	EXPECT_EQ(12, sizeof(H));
	EXPECT_EQ(12, sizeof(J));
	EXPECT_EQ(5, sizeof(I));
	EXPECT_EQ(24, sizeof(K));
	EXPECT_EQ(24, sizeof(L));
	EXPECT_EQ(24, sizeof(M));
	EXPECT_EQ(32, sizeof(O));
}
