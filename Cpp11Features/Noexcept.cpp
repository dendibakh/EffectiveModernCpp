/*
 * Noexcept.cpp
 *
 *  Created on: May 11, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <vector>

// generally noexcept functions could be better optimized than non-noexcept.
// remember that noexcept is a part of function signature.

class NoExTestClass1
{
public:
	NoExTestClass1()
	{
		x = 0;
	}
	NoExTestClass1(const NoExTestClass1& rhs)
	{
		x = 1;
	}
	NoExTestClass1(const NoExTestClass1&& rhs)
	{
		x = 2;
	}

	int x {0};
};

TEST(NoExceptUnitTest, usage_test1)
{
	std::vector<NoExTestClass1> first;
	first.reserve(1);
	NoExTestClass1 el;
	first.push_back(el);
	ASSERT_EQ(1, first[0].x); // was copy-constructed
	first.resize(first.capacity() + 1);
	ASSERT_EQ(1, first[0].x); // was copy-constructed after reallocation
}

class NoExTestClass2
{
public:
	NoExTestClass2()
	{
		x = 0;
	}
	NoExTestClass2(const NoExTestClass2& rhs)
	{
		x = 1;
	}
	NoExTestClass2(const NoExTestClass2&& rhs) noexcept
	{
		x = 2;
	}

	int x {0};
};

TEST(NoExceptUnitTest, usage_test2)
{
	std::vector<NoExTestClass2> first;
	first.reserve(1);
	NoExTestClass2 el;
	first.push_back(el);
	ASSERT_EQ(1, first[0].x); // was copy-constructed
	first.resize(first.capacity() + 1);
	ASSERT_EQ(2, first[0].x); // was moved-constructed after reallocation, because move constructor is noexcept
}
