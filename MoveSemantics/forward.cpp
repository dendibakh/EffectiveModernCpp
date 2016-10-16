/*
 * forward.cpp
 *
 *  Created on: May 19, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

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

	template <typename T>
	void callWithRvalue(T && uref)
	{
		testMoves moveObj = std::forward<T>(uref);
		ASSERT_EQ(3, moveObj.x);
	}

	template <typename T>
	void callWithLvalue(T && uref)
	{
		testMoves copyObj = std::forward<T>(uref);
		ASSERT_EQ(1, copyObj.x);
	}
}

TEST(ForwardUnitTest, forward_produce_rvalue_or_lvalue_ref_based_on_a_param)
{
	int x = 1;

	static_assert( std::is_same< decltype(std::forward<int>(7)), int&&>::value, "types are not the same" );
	static_assert( std::is_same< decltype(std::forward<int>(x)), int&&>::value, "types are not the same" );

	testMoves obj;
	callWithLvalue(obj);
	callWithRvalue(std::move(obj));
}

namespace
{
	template<typename T>
	T&& Myforward(std::remove_reference_t<T>& param)
	{
		return static_cast<T&&>(param);
	}

	template <typename T>
	void callWithRvalueWithMyforward(T && uref)
	{
		testMoves moveObj = std::forward<T>(uref);
		ASSERT_EQ(3, moveObj.x);
	}

	template <typename T>
	void callWithLvalueWithMyforward(T && uref)
	{
		testMoves copyObj = std::forward<T>(uref);
		ASSERT_EQ(1, copyObj.x);
	}
}

TEST(ForwardUnitTest, check_my_forward_version)
{
	int x = 1;

	// not compiles: can't convert int to int&
	//static_assert( std::is_same< decltype(Myforward<int>(7)), int&&>::value, "types are not the same" );
	static_assert( std::is_same< decltype(Myforward<int>(x)), int&&>::value, "types are not the same" );

	testMoves obj;
	callWithLvalueWithMyforward(obj);
	callWithRvalueWithMyforward(std::move(obj));
}
