/*
 * nullptr.cpp
 *
 *  Created on: May 10, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

int foo(int)   {return 1;}
int foo(bool)  {return 2;}
int foo(void*) {return 3;}

TEST(NullptrUnitTest, NULL_calls_int_overload)
{
	ASSERT_EQ(1, foo(0));
	//ASSERT_EQ(1, foo(NULL)); not compiles: call is ambiguous
}

TEST(NullptrUnitTest, nullptr_calls_pointer_overload)
{
	ASSERT_EQ(3, foo(nullptr));
}
