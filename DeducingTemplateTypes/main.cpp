#include "gtest/gtest.h"
#include <type_traits>

template <typename T>
void test1(const T& rhs)
{
	static_assert( std::is_same< decltype(rhs), const int&>::value, "types are not the same" );
	static_assert( std::is_same< T, int>::value, "types are not the same" );
}

TEST(TypeDeductionUnitTest, 1)
{
	int x = 0;
	test1(x);

	//char c = 0;
   	//test1(c);
}

template <typename T, typename U, typename V>
void test2(T& rhs)
{
	static_assert( std::is_same< T, U>::value, "types are not the same" );
	static_assert( std::is_same< decltype(rhs), V>::value, "types are not the same" );
}

TEST(TypeDeductionUnitTest, 2_ParamType_is_a_reference)
{
	int x = 0;
	test2< decltype(x), decltype(x), decltype(x)& >(x);

	int& rx = x;
	test2< decltype(x), decltype(x), decltype(x)& >(rx);

	const int cx = x;
	test2< decltype(cx), std::add_const<decltype(x)>::type, std::add_const<decltype(x)>::type & >(cx);

	const int& rcx = x;
	test2< decltype(cx), std::add_const<decltype(x)>::type, std::add_const<decltype(x)>::type & >(rcx);

	// cannot call with rvalue
	//test2< decltype(27), decltype(27), std::add_const<decltype(27)>::type& >(27);
}

template <typename T, typename U, typename V>
void test3(const T& rhs)
{
	static_assert( std::is_same< T, U>::value, "types are not the same" );
	static_assert( std::is_same< decltype(rhs), V>::value, "types are not the same" );
}

TEST(TypeDeductionUnitTest, 3_ParamType_is_a_reference)
{
	int x = 0;
	test3< decltype(x), decltype(x), std::add_const<decltype(x)>::type & >(x);

	int& rx = x;
	test3< decltype(x), decltype(x), std::add_const<decltype(x)>::type & >(rx);

	const int cx = x;
	test3< decltype(x), decltype(x), std::add_const<decltype(x)>::type & >(cx);

	const int& rcx = x;
	test3< decltype(x), decltype(x), std::add_const<decltype(x)>::type & >(rcx);

	// cannot call with rvalue
	//test3< decltype(27), decltype(27), std::add_const<decltype(27)>::type& >(27);
}

template <typename T, typename U, typename V>
void test4(T&& rhs)
{
	static_assert( std::is_same< T, U>::value, "types are not the same" );
	static_assert( std::is_same< decltype(rhs), V>::value, "types are not the same" );
}

TEST(TypeDeductionUnitTest, 4_ParamType_is_a_universal_reference)
{
	int x = 0;
	test4< decltype(x) &, decltype(x) &, decltype(x) & >(x);

	int& rx = x;
	test4< decltype(x) &, decltype(x) &, decltype(x) & >(rx);

	const int cx = x;
	test4< std::add_const<decltype(x)>::type &, std::add_const<decltype(x)>::type &, std::add_const<decltype(x)>::type & >(cx);

	const int& rcx = x;
	test4< std::add_const<decltype(x)>::type &, std::add_const<decltype(x)>::type &, std::add_const<decltype(x)>::type & >(rcx);

	test4< decltype(27), decltype(27), decltype(27)&& >(27);
}

template <typename T, typename U, typename V>
void test5(T rhs)
{
	static_assert( std::is_same< T, U>::value, "types are not the same" );
	static_assert( std::is_same< decltype(rhs), V>::value, "types are not the same" );
}

TEST(TypeDeductionUnitTest, 5_ParamType_by_value)
{
	int x = 0;
	test5< decltype(x), decltype(x), decltype(x) >(x);

	int& rx = x;
	test5< decltype(x), decltype(x), decltype(x) >(rx);

	const int cx = x;
	test5< decltype(x), decltype(x), decltype(x) >(cx);

	const int& rcx = x;
	test5< decltype(x), decltype(x), decltype(x) >(rcx);

	test5< decltype(27), decltype(27), decltype(27) >(27);

	const char* const ptr = "ha";
	test5< decltype(ptr), decltype(ptr), decltype(ptr) >(ptr);
	test5< const char* const, const char* const, const char* const >(ptr);
}

template <typename T, typename U, typename V>
void test6_array_by_value(T rhs)
{
	static_assert( std::is_same< T, U>::value, "types are not the same" );
	static_assert( std::is_same< decltype(rhs), V>::value, "types are not the same" );
}

template <typename T, typename U, typename V>
void test6_array_by_reference(T& rhs)
{
	static_assert( std::is_same< T, U>::value, "types are not the same" );
	static_assert( std::is_same< decltype(rhs), V>::value, "types are not the same" );
}

TEST(TypeDeductionUnitTest, 6_ParamType_is_array)
{
	const char x[] = "hi";
	test6_array_by_value< const char*, const char*, const char* >(x);
	test6_array_by_reference< const char[3], const char[3], const char(&)[3] >(x);
}

template <typename T, size_t N>
constexpr std::size_t arraySize(T (&)[N])
{
	return N;
}

TEST(TypeDeductionUnitTest, 7_determine_array_size)
{
	char x1[] = "";
	static_assert(arraySize(x1) == 1, "wrong size returned");

	char x2[] = "abc";
	static_assert(arraySize(x2) == 4, "wrong size returned");
}

template <typename T, typename U, typename V>
void test8_func_ptr_by_value(T rhs)
{
	static_assert( std::is_same< T, U>::value, "types are not the same" );
	static_assert( std::is_same< decltype(rhs), V>::value, "types are not the same" );
}

template <typename T, typename U, typename V>
void test8_func_ptr_by_reference(T& rhs)
{
	static_assert( std::is_same< T, U>::value, "types are not the same" );
	static_assert( std::is_same< decltype(rhs), V>::value, "types are not the same" );
}

void somefunc(int){}

TEST(TypeDeductionUnitTest, 8_ParamType_is_func_ptr)
{
	test8_func_ptr_by_value< void (*)(int), void (*)(int), void (*)(int) >(somefunc);
	test8_func_ptr_by_reference< void (&)(int), void (&)(int), void (&)(int) >(somefunc);
}
