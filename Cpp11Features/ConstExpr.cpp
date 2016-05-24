/*
 * ConstExpr.cpp
 *
 *  Created on: May 12, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <array>

// constexpr is a part of functions signature

TEST(ConstExprUnitTest, constexpr_produces_const_objects)
{
	constexpr int a = 0;
	(void) a;
	// a = 1; won't compile
}

TEST(ConstExprUnitTest, constexpr_can_be_used_as_compileTime_constant)
{
	constexpr int a = 0;
	std::array<int, a> arr;
	(void) arr;
}

constexpr int constexpr_foo(int val)
{
	return val * 2;
}

TEST(ConstExprUnitTest, constexpr_function_can_be_invoked_in_compileTime)
{
	static_assert( 4 == constexpr_foo(2), "error" );

	constexpr int a = 1;
	int x = constexpr_foo(a); // constexpr_foo(a) is computed in compile-time
	ASSERT_EQ(2, x);
}

TEST(ConstExprUnitTest, constexpr_function_can_be_invoked_in_runTime)
{
	int a = 1;
	int x = constexpr_foo(a); // constexpr_foo(a) is computed in run-time
	ASSERT_EQ(2, x);
}
