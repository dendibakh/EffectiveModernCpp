/*
 * move.cpp
 *
 *  Created on: May 19, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

template<typename T>
decltype(auto) myMove(T&& param)
{
	using ReturnType = std::remove_reference_t<T>&&;
	return static_cast<ReturnType>(param);
}

TEST(MoveUnitTest, move_always_produce_rvalue_ref)
{
	int x = 0;

	static_assert( std::is_same< decltype(myMove(7)), int&&>::value, "types are not the same" );
	static_assert( std::is_same< decltype(myMove(x)), int&&>::value, "types are not the same" );

	static_assert( std::is_same< decltype(std::move(7)), int&&>::value, "types are not the same" );
	static_assert( std::is_same< decltype(std::move(x)), int&&>::value, "types are not the same" );
}

namespace
{
	class testMoves
	{
	public:
		testMoves() {}
		testMoves(const testMoves&)				{ x = 1;}
		testMoves& operator=(const testMoves&) 	{ x = 2; return *this; }
		testMoves(testMoves&&)					{ x = 3;}
		testMoves& operator=(testMoves&&) 		{ x = 4; return *this; }
		~testMoves() {}
		int x {0};
	};

	void fooConstParam(testMoves byValue)
	{
		ASSERT_EQ(1, byValue.x);
	}

	void fooParam(testMoves byValue)
	{
		ASSERT_EQ(3, byValue.x);
	}
}

TEST(MoveUnitTest, move_cant_produce_rvalue_from_const_object)
{
	const testMoves a;
	fooConstParam(std::move(a)); // const object cannot be moved out.
	testMoves b;
	fooParam(std::move(b));
}
