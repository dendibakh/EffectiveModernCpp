/*
 * AliasDeclarations.cpp
 *
 *  Created on: May 10, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <memory>
#include <string>

// C++98 style:

typedef std::unique_ptr<std::string> Cpp98UptrStr;
using Cpp11UptrStr = std::unique_ptr<std::string>;

TEST(AliasDeclarationUnitTest, aliases_are_nicer)
{
	static_assert( std::is_same< Cpp98UptrStr, Cpp11UptrStr>::value, "types are not the same" );
}

typedef void (*FP98)(int, double);
using FP11 = void (*)(int, double);

TEST(AliasDeclarationUnitTest, aliases_are_more_readable)
{
	static_assert( std::is_same< FP98, FP11>::value, "types are not the same" );
}

template <typename T>
struct A98
{
	typedef std::unique_ptr<T> type;
};

template <typename T>
using A11 = std::unique_ptr<T>;

TEST(AliasDeclarationUnitTest, aliases_are_less_typing)
{
	static_assert( std::is_same< A98<int>::type, A11<int> >::value, "types are not the same" );
}

template <typename T>
class A
{
	typename A98<T>::type x;
	//A98<T>::type y; not compiles
	A11<T> z;
};

TEST(AliasDeclarationUnitTest, aliases_require_no_typename)
{

	A<int> a;
}

