/*
 * move.cpp
 *
 *  Created on: May 19, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <iostream>

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
		testMoves() { std::cout << "testMoves()" << '\n'; }
		testMoves(const testMoves& o)				{ std::cout << "testMoves(&)" << '\n';  x = 1; }
		testMoves& operator=(const testMoves& o) 	{ std::cout << "operator=(&)" << '\n';  x = 2; return *this; }
		testMoves(testMoves&& o)					{ std::cout << "testMoves(&&)" << '\n'; x = 3; o.x = 3;}
		testMoves& operator=(testMoves&& o) 		{ std::cout << "operator=(&&)" << '\n'; x = 4; o.x = 4; return *this; }
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

template <class T>
class TD;

TEST(MoveUnitTest, move_cant_produce_rvalue_from_const_object)
{
	const testMoves a;
	static_assert( std::is_same< decltype(std::move(a)), const testMoves&&>::value, "types are not the same" );

	fooConstParam(std::move(a));
	// const object cannot be moved out.
	// because it will not match the function signatire (testMoves&&, not const testMoves&&).

	testMoves b;
	fooParam(std::move(b));
}

namespace
{
	class A
	{
	public:
		A() {}
		A(const A& rhs) = delete;
		A& operator= (const A& rhs) = delete;
		A(A&& rhs) {}
		A& operator=(A&& rhs) {return *this;}
	};

	void bar(A&& b)
	{
		static_assert( std::is_same< decltype(b), A&&>::value, "types are not the same" );
	}

	void foo(A&& a)
	{
		static_assert( std::is_same< decltype(a), A&&>::value, "types are not the same" );

		//bar(a);
		bar(std::move(a));

		//A b(a); // not compiles because it calls copy-constructor
		A b(std::move(a)); // calls move-constructor
	}
}

TEST(MoveUnitTest, we_need_to_moved_rvalue_ref_if_it_is_function_parameter)
{
	/*
	In some cases, you’ll want to use the object bound to an rvalue reference or a universal reference more than once
	in a single function, and you’ll want to make sure that it’s not moved from until
	you’re otherwise done with it. In that case, you’ll want to apply std::move (for rvalue
	references) or std::forward (for universal references) to only the final use of the
	reference
	*/
	A a;
	//foo(a);
	foo(std::move(a));
}
