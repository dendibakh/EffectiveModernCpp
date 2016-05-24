/*
 * bracesInit.cpp
 *
 *  Created on: May 9, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

class bracesInit
{
public:
	int x {1};
	int y = 2;
	//	int z(0); not compiles.
};

TEST(BraceInitUnitTest, class_private_fields_init)
{
	bracesInit brIn;
	ASSERT_EQ(1, brIn.x);
	ASSERT_EQ(2, brIn.y);
}

// If in ctor or function parameters there is a std::initializer_list
// it will be greedily chosen by the caller to other overloads.
//
// example:
//
// foo (int i, double d);
// foo (std::initializer_list<long double>)
//
// foo({5, 10.5}) // calls 2nd overload.
//
// Rule: if there is a way to call overload with std::initializer it will be called.
//

// But empty braces means default construction:
class bracesDef
{
public:
	int x;
	bracesDef() {x = 1;}
	bracesDef(std::initializer_list<int>) {x = 2;}
};

TEST(BraceInitUnitTest, empty_braces_init)
{
	bracesDef brD1;
	ASSERT_EQ(1, brD1.x);

	//bracesDef brD1(); // most vexing parse

	bracesDef brD2{};
	ASSERT_EQ(1, brD2.x);
}

TEST(BraceInitUnitTest, if_you_want_to_call_inti_list)
{
	bracesDef brD1({});
	ASSERT_EQ(2, brD1.x);

	bracesDef brD2{{}};
	ASSERT_EQ(2, brD2.x);
}
