/*
 * ScopedEnums.cpp
 *
 *  Created on: May 10, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

namespace Color98
{
	enum Color { black, white, red};
	enum ColorBool { Black, White};
}
enum class Color11{ black, white, red};

TEST(ScopedEnumsUnitTest, UnscopedEnums_leak_names_in_upstream_namespace)
{
	using namespace Color98;
	//auto white = 10; white already exists
}

TEST(ScopedEnumsUnitTest, ScopedEnums_DONT_leak_names_in_upstream_namespace)
{
	auto white = Color11::white;
	(void)white;
}

TEST(ScopedEnumsUnitTest, UnscopedEnums_can_implicitly_convert_to_int)
{
	int a = Color98::white; (void)a;
	//int b = Color11::white; not compiles
	int c = static_cast<int>(Color11::white); (void)c;
}

enum class ColorBool11{ black, white}; // int by default
enum class ColorBool11_under_bool  : bool { black, white};
//enum class ColorBool11_under_bool_ : bool { black, white, red}; not compiles

TEST(ScopedEnumsUnitTest, For_scopedEnums_we_can_choose_underlying_type)
{
	static_assert( sizeof(int) == sizeof(Color98::Color), "sizes are not the same" );
	static_assert( sizeof(int) == sizeof(Color98::ColorBool), "sizes are not the same" );
	static_assert( sizeof(int) == sizeof(Color98::white), "sizes are not the same" );
	static_assert( sizeof(int) == sizeof(Color98::White), "sizes are not the same" );

	static_assert( sizeof(int) == sizeof(ColorBool11), "sizes are not the same" );
	static_assert( sizeof(bool) == sizeof(ColorBool11_under_bool), "sizes are not the same" );
}

//enum FwdEnum98; not compiles

enum class FwdEnum11;
int fEnum(FwdEnum11 e) {return static_cast<int>(e);}
enum class FwdEnum11 { black, white };

TEST(ScopedEnumsUnitTest, ScopedEnums_can_be_forward_declared)
{
	ASSERT_EQ(1, fEnum(FwdEnum11::white));
	ASSERT_EQ(0, fEnum(FwdEnum11::black));
}
