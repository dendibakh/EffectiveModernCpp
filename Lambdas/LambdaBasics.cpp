/*
 * LambdaBasics.cpp
 *
 *  Created on: May 26, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

TEST(LambdaUnitTest, lambdas_can_be_copied)
{
	int x = 0;
	auto c1 = [x](int y) { return x * y > 55; };

	auto c2 = c1;
	auto c3 = c2;

	// they all are instances of the same closure class.
	static_assert( std::is_same< decltype(c2), decltype(c3)>::value, "types are not the same" );
	static_assert( std::is_same< decltype(c1), decltype(c2)>::value, "types are not the same" );
	static_assert( std::is_same< decltype(c1), decltype(c3)>::value, "types are not the same" );
}

TEST(LambdaUnitTest, auto_can_be_used_in_lambda_parameters)
{
	int x = 0;
	auto c1 = [x](auto y) { return x * y > 55; };
	auto result = c1(5);
	(void)result;
}

TEST(LambdaUnitTest, Cpp14_has_generalized_capture_mode)
{
	int x = 0;
	// x = x below copies x into closure
	auto c1 = [x = x](auto y) { return x * y > 55; };
	auto result = c1(5);
	(void)result;
}

namespace
{
	void working_func()
	{}
}

TEST(LambdaUnitTest, perfect_forwarding_lambda)
{
	// because of reference collapsing rule
	// decltype(params) will yield T& for lvalues
	// and T for rvalues (as expected).
	auto f = [](auto&&... params)
		{
			return working_func(std::forward<decltype(params)>(params)...);
		};

	f();
}
