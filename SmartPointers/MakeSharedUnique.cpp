/*
 * MakeSharedUnique.cpp
 *
 *  Created on: May 16, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <memory>

/*
 * Reasons for using make_* functions:
 * 1. Less typing.
 * 2. Exception safety.
 * 3. std::make_shared allows compilers to generate smaller, faster code. Because only one allocation takes place.
 */

class someClass
{

};

template <typename T>
void foo(T a, int b)
{

}

int bar()
{
	//if (sizeof(int) == 4)
	//	throw 5;
	return 0;
}

TEST(MakeSharedUniqueUnitTest, unique_ptr_usage)
{
	std::unique_ptr<someClass> un_ptr1(new someClass);
	auto un_ptr2(std::make_unique<someClass>());

	try
	{
		foo(std::unique_ptr<someClass>(new someClass), bar()); // possible memory leak
		foo(std::make_unique<someClass>(), bar());
	}
	catch (int)
	{

	}
}

TEST(MakeSharedUniqueUnitTest, shared_ptr_usage)
{
	std::shared_ptr<someClass> sh_ptr1(new someClass);
	auto sh_ptr2(std::make_shared<someClass>());

	try
	{
		foo(std::shared_ptr<someClass>(new someClass), bar()); // possible memory leak
		foo(std::make_shared<someClass>(), bar());
	}
	catch (int)
	{

	}
}
