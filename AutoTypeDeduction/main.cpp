/*
 * main.cpp
 *
 *  Created on: May 5, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

TEST(AutoDeductionUnitTest, works_as_templates)
{
	auto x = 27;
	static_assert( std::is_same< decltype(x), int>::value, "types are not the same" );

	auto& rx = x;
	static_assert( std::is_same< decltype(rx), int&>::value, "types are not the same" );

	const auto cx = x;
	static_assert( std::is_same< decltype(cx), const int>::value, "types are not the same" );

	const auto& rcx = x;
	static_assert( std::is_same< decltype(rcx), const int&>::value, "types are not the same" );
}

TEST(AutoDeductionUnitTest, universal_references_works_as_for_templates)
{
	auto x = 27;
	const auto cx = x;

	auto&& rrx = x;
	static_assert( std::is_same< decltype(rrx), int&>::value, "types are not the same" ); //because x is lvalue

	auto&& rrcx = cx;
	static_assert( std::is_same< decltype(rrcx), const int&>::value, "types are not the same" );

	auto&& rrv = 27;
	static_assert( std::is_same< decltype(rrv), int&&>::value, "types are not the same" ); // 27 is rvalue

	const auto&& crrv = 27;
	static_assert( std::is_same< decltype(crrv), const int&&>::value, "types are not the same" ); // 27 is rvalue
}

TEST(AutoDeductionUnitTest, arrays_works_as_for_templates)
{
	const char name[] = "str";

	auto arr1 = name;
	static_assert( std::is_same< decltype(arr1), const char*>::value, "types are not the same" );

	auto& arr2 = name;
	static_assert( std::is_same< decltype(arr2), const char (&)[4]>::value, "types are not the same" );
}

void someFunc(int) {}

TEST(AutoDeductionUnitTest, func_ptr_works_as_for_templates)
{
	auto func1 = someFunc;
	static_assert( std::is_same< decltype(func1), void (*)(int)>::value, "types are not the same" );

	auto& func2 = someFunc;
	static_assert( std::is_same< decltype(func2), void (&)(int)>::value, "types are not the same" );
}

TEST(AutoDeductionUnitTest, brace_init_yelds_initializer_list)
{
	auto x1 = 27;
	auto x2(27);
	auto x3 = {27};
	auto x4{27};

	static_assert( std::is_same< decltype(x1), int>::value, "types are not the same" );
	static_assert( std::is_same< decltype(x2), int>::value, "types are not the same" );
	static_assert( std::is_same< decltype(x3), std::initializer_list<int> >::value, "types are not the same" );
	static_assert( std::is_same< decltype(x4), std::initializer_list<int> >::value, "types are not the same" );
}

#include <vector>

std::vector<bool> getVect()
{
	return std::vector<bool>(10, 0);
}

TEST(AutoDeductionUnitTest, deducing_proxy_types)
{
	auto v = getVect()[5];
	static_assert( std::is_same< decltype(v), std::vector<bool>::reference >::value, "types are not the same" );
	// this leads to undefined behaviour, because v refer to temporary object that was destroyed.

	// this is correct
	auto u = static_cast<bool>(getVect()[5]);
	static_assert( std::is_same< decltype(u), bool >::value, "types are not the same" );
}
