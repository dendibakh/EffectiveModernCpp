#include <gtest/gtest.h>
#include <type_traits>

using namespace ::testing;

namespace
{
	template <class T>
	int foo(T a)
	{
		return a;
	}

	template <>
	int foo<short>(short a)
	{
		return a + 1;
	}

	template <class T>
	struct A
	{
		int foo(T a)
		{
			return a;
		}
	};

	template<>
	struct A<short>
	{
		int foo(short a)
		{
			return a + 1;
		}
	};
}

TEST(TemplSpec, 1)
{
	int a = 1;
	short b = 1;
	EXPECT_EQ(1, foo(a));
	EXPECT_EQ(2, foo(b));

	A<int> aa;
	A<short> bb;
	EXPECT_EQ(1, aa.foo(a));
	EXPECT_EQ(2, bb.foo(b));
}

namespace
{
	template <int N>
	struct fact
	{
		static const int val = fact< N - 1 >::val * N;
	};

	template <>
	struct fact<0>
	{
		static const int val = 1;
	};
}

static_assert(fact<1>::val == 1, "");
static_assert(fact<2>::val == 2, "");
static_assert(fact<3>::val == 6, "");
static_assert(fact<4>::val == 24, "");
static_assert(fact<5>::val == 120, "");
