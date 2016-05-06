/*
 * main.cpp
 *
 *  Created on: May 5, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

template<typename T>
class TD;

TEST(DecltypeDeductionUnitTest, auto_and_decltype)
{
	auto x = 27;
	const int& rcx = x;

	auto arcx1 = rcx;
	decltype(auto) arcx2 = rcx;

	static_assert( std::is_same< decltype(arcx1), int>::value, "types are not the same" );
	static_assert( std::is_same< decltype(arcx2), const int&>::value, "types are not the same" );

	// This is how type can be displayed.
	// TD<decltype(arcx2)> arcx2Type;
}

// C++11 version
template<typename T>
auto accessCpp11(T&& container, size_t index) -> decltype(std::forward<T>(container)[index])
{
	return std::forward<T>(container)[index];
}

template<typename T>
decltype(auto) accessCpp14(T&& container, size_t index)
{
	return std::forward<T>(container)[index];
}

TEST(DecltypeDeductionUnitTest, example_of_decltype_usage)
{
	std::vector<int> v(1, 0);
	ASSERT_EQ(0, accessCpp11(v, 0u));
	ASSERT_EQ(0, accessCpp14(v, 0u));
}

TEST(DecltypeDeductionUnitTest, funny_decltype)
{
	int x = 0;
	static_assert( std::is_same< decltype(x), int>::value, "types are not the same" );
	static_assert( std::is_same< decltype((x)), int&>::value, "types are not the same" );
}
