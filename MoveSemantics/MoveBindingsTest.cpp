/*
 * MoveBindingsTest.cpp
 *
 *  Created on: Oct 14, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <iostream>

using namespace ::testing;

int foo(int &&x)
{
	return x;
}

TEST(MoveBindingsTest, binding_references)
{
	foo(5);

	int x = 5;
	// foo(x); compiler error - can't bind int to int&&.

	foo(std::move(x));

	int& y = x;
	// foo(y); compiler error - can't bind int to int&&.

	foo(std::move(y));
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
}

template <class T>
class TD;

// Returning an rvalue object by value.
// obj1 is a copy of obj. no magic here.
// Temporary is always created.

testMoves bar (testMoves&& obj)
{
	return obj;
}

TEST(MoveBindingsTest, taking_returned_object_by_value)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	testMoves obj1 = bar(std::move(obj));
	ASSERT_EQ(0, obj.x);
	ASSERT_EQ(1, obj1.x);
}

TEST(MoveBindingsTest, taking_returned_object_by_lvalue_ref_not_compiles)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

//	testMoves& obj2 = bar(std::move(obj));
//	ASSERT_EQ(0, obj.x);
//	ASSERT_EQ(1, obj2.x);
}

TEST(MoveBindingsTest, taking_returned_object_by_const_ref)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	const testMoves& obj2 = bar(std::move(obj));
	ASSERT_EQ(0, obj.x);
	ASSERT_EQ(1, obj2.x);
	static_assert( std::is_same< decltype(obj2), const testMoves&>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_returned_object_by_rvalue)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	testMoves&& obj2 = bar(std::move(obj)); // is it an undefined behavior?
	ASSERT_EQ(0, obj.x);
	ASSERT_EQ(1, obj2.x);
	static_assert( std::is_same< decltype(obj2), testMoves&&>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_returned_object_by_const_rvalue)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	const testMoves&& obj2 = bar(std::move(obj)); // is it an undefined behavior?
	ASSERT_EQ(0, obj.x);
	ASSERT_EQ(1, obj2.x);
	static_assert( std::is_same< decltype(obj2), const testMoves&&>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_returned_object_deduced_type_is_value)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	auto obj1 = bar(std::move(obj));
	ASSERT_EQ(0, obj.x);
	ASSERT_EQ(1, obj1.x);
	static_assert( std::is_same< decltype(obj1), testMoves>::value, "types are not the same" );
}

// Returning an rvalue object by rvalue.
// Here obj1 will be move-constructed from obj.

testMoves barMove (testMoves&& obj)
{
	return std::move(obj);
}

TEST(MoveBindingsTest, taking_moved_object_by_value)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	testMoves obj1 = barMove(std::move(obj));
	ASSERT_EQ(3, obj.x);
	ASSERT_EQ(3, obj1.x);
}

TEST(MoveBindingsTest, taking_moved_object_by_ref_not_compiles)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

//	testMoves& obj1 = barMove(std::move(obj));
//	ASSERT_EQ(3, obj.x);
//	ASSERT_EQ(3, obj1.x);
}

TEST(MoveBindingsTest, taking_moved_object_by_const_ref)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	const testMoves& obj1 = barMove(std::move(obj));
	ASSERT_EQ(3, obj.x);
	ASSERT_EQ(3, obj1.x);
}

TEST(MoveBindingsTest, taking_moved_object_by_rvalue_ref)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	testMoves&& obj1 = barMove(std::move(obj));
	ASSERT_EQ(3, obj.x);
	ASSERT_EQ(3, obj1.x);
}

TEST(MoveBindingsTest, taking_moved_object_by_const_rvalue_ref)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	const testMoves&& obj1 = barMove(std::move(obj));
	ASSERT_EQ(3, obj.x);
	ASSERT_EQ(3, obj1.x);
}

TEST(MoveBindingsTest, taking_moved_object_deduced_type_is_value)
{
	testMoves obj;
	ASSERT_EQ(0, obj.x);

	auto obj1 = barMove(std::move(obj));
	ASSERT_EQ(3, obj.x);
	ASSERT_EQ(3, obj1.x);
	static_assert( std::is_same< decltype(obj1), testMoves>::value, "types are not the same" );
}

// here is 100% RVO will take place.

testMoves barReturnLocal ()
{
	testMoves obj;
	EXPECT_EQ(0, obj.x);
	return obj;
}

TEST(MoveBindingsTest, taking_returned_local_object_by_value)
{
	testMoves obj = barReturnLocal();
	ASSERT_EQ(0, obj.x); // here it is 0, because RVO took place.
	static_assert( std::is_same< decltype(obj), testMoves>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_returned_local_object_by_ref_not_compiles)
{
//	testMoves& obj = barReturnLocal();
//	ASSERT_EQ(0, obj.x); // here it is 0, because RVO took place.
//	static_assert( std::is_same< decltype(obj), testMoves&>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_returned_local_object_by_const_ref)
{
	const testMoves& obj = barReturnLocal();
	ASSERT_EQ(0, obj.x); // here it is 0, because RVO took place.
	static_assert( std::is_same< decltype(obj), const testMoves&>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_returned_local_object_by_rvalue_ref)
{
	testMoves&& obj = barReturnLocal();
	ASSERT_EQ(0, obj.x); // here it is 0, because RVO took place.
	static_assert( std::is_same< decltype(obj), testMoves&&>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_returned_local_object_by_const_rvalue_ref)
{
	const testMoves&& obj = barReturnLocal();
	ASSERT_EQ(0, obj.x); // here it is 0, because RVO took place.
	static_assert( std::is_same< decltype(obj), const testMoves&&>::value, "types are not the same" );
}

// here RVO won't take place.
// First, local object will be created and them moved into the returned value.
// So, one additional move.

testMoves barMoveLocal ()
{
	testMoves obj;
	EXPECT_EQ(0, obj.x);
	return std::move(obj);
}

TEST(MoveBindingsTest, taking_moved_local_object_by_value)
{
	testMoves obj = barMoveLocal();
	ASSERT_EQ(3, obj.x); // here it is 0, because RVO took place.
	static_assert( std::is_same< decltype(obj), testMoves>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_moved_local_object_by_ref_not_compiles)
{
//	testMoves& obj = barMoveLocal();
//	ASSERT_EQ(0, obj.x); // here it is 0, because RVO took place.
//	static_assert( std::is_same< decltype(obj), testMoves&>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_moved_local_object_by_const_ref)
{
	const testMoves& obj = barMoveLocal();
	ASSERT_EQ(3, obj.x); // here it is 0, because RVO took place.
	static_assert( std::is_same< decltype(obj), const testMoves&>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_moved_local_object_by_rvalue_ref)
{
	testMoves&& obj = barMoveLocal();
	ASSERT_EQ(3, obj.x); // here it is 0, because RVO took place.
	static_assert( std::is_same< decltype(obj), testMoves&&>::value, "types are not the same" );
}

TEST(MoveBindingsTest, taking_moved_local_object_by_const_rvalue_ref)
{
	const testMoves&& obj = barMoveLocal();
	ASSERT_EQ(3, obj.x); // here it is 0, because RVO took place.
	static_assert( std::is_same< decltype(obj), const testMoves&&>::value, "types are not the same" );
}

// Attempt to break RVO.
// local object COPIED into returned object.

testMoves barMoveLocalCond1 (bool cond)
{
	testMoves obj1;
	testMoves obj2;
	EXPECT_EQ(0, obj1.x);
	EXPECT_EQ(0, obj2.x);
	return cond ? obj1 : obj2;
}

TEST(MoveBindingsTest, no_rvo_taking_moved_local_object_by_value)
{
	testMoves obj = barMoveLocalCond1(false);
	ASSERT_EQ(1, obj.x);
	static_assert( std::is_same< decltype(obj), testMoves>::value, "types are not the same" );
}

// Attempt to break RVO.
// local object MOVED into returned object.

testMoves barMoveLocalCond2 (bool cond)
{
	testMoves obj1;
	testMoves obj2;
	EXPECT_EQ(0, obj1.x);
	EXPECT_EQ(0, obj2.x);
	if (cond)
		return obj1;
	return obj2;
}

TEST(MoveBindingsTest, rvo_taking_moved_local_object_by_value)
{
	testMoves obj = barMoveLocalCond2(false);
	ASSERT_EQ(3, obj.x);
	static_assert( std::is_same< decltype(obj), testMoves>::value, "types are not the same" );
}

// Testing binding T&& to const T&

void bindRvalueRefToConstLValueRef (const testMoves& obj)
{
	(void) obj;
}

TEST(MoveBindingsTest, RValue_Ref_BINDS_to_const_LValue_Ref)
{
	testMoves obj;
	bindRvalueRefToConstLValueRef(std::move(obj));
}

// Testing binding T&& to T&

void bindRvalueRefToLValueRef (testMoves& obj)
{
	(void) obj;
}

TEST(MoveBindingsTest, RValue_Ref_does_NOT_bind_to_LValue_Ref)
{
	testMoves obj;
	//bindRvalueRefToLValueRef(std::move(obj)); not compiles
}

// Testing binding const T&& to T&

void bindConstRvalueRefToLValueRef (testMoves& obj)
{
	(void) obj;
}

TEST(MoveBindingsTest, Const_RValue_Ref_does_NOT_bind_to_LValue_Ref)
{
	const testMoves obj;
	//bindConstRvalueRefToLValueRef(std::move(obj)); not compiles
}

// Testing binding const T&& to const T&

void bindConstRvalueRefToConstLValueRef (const testMoves& obj)
{
	(void) obj;
}

TEST(MoveBindingsTest, Const_RValue_Ref_BINDS_to_Const_LValue_Ref)
{
	const testMoves obj;
	bindConstRvalueRefToConstLValueRef(std::move(obj));
}
