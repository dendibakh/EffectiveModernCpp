#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	struct A
	{
		int x;
	};

	struct B : public A
	{
		int y;
	};

	void foo(A a)
	{
		(void)a;
	}
}

TEST(Slicing, 1)
{
	B b;
	foo(b);
}
