/*
 * UniversalReference.cpp
 *
 *  Created on: May 20, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <vector>
#include <chrono>
#include <set>

/*
 *
	Things to Remember
	• If a function template parameter has type T&& for a deduced type T, or if an
	object is declared using auto&&, the parameter or object is a universal reference.
	• If the form of the type declaration isn’t precisely type&&, or if type deduction
	does not occur, type&& denotes an rvalue reference.
	• Universal references correspond to rvalue references if they’re initialized with
	rvalues. They correspond to lvalue references if they’re initialized with lvalues.

   void f(Widget&& param); // rvalue reference

   Widget&& var1 = Widget(); // rvalue reference

   auto&& var2 = var1; // not rvalue reference

   template<typename T>
   void f(std::vector<T>&& param); // rvalue reference

   template<typename T>
   void f(T&& param); // not rvalue reference
 */

TEST(UniversalReferencesUnitTest, definition_test)
{
	int x = 1;

	auto&& l_ref = x;
	static_assert( std::is_same< decltype(l_ref), int&>::value, "types are not the same" );

	auto&& u_ref = 7;
	static_assert( std::is_same< decltype(u_ref), int&&>::value, "types are not the same" );

	ASSERT_EQ(1, l_ref);
	ASSERT_EQ(7, u_ref);
}

namespace
{
	template<typename T>
	void f(std::vector<T>&& param) // param should be exactly T&&
	{
		(void)param;
	}

	template<typename T>
	void f_const(const T&& param) // const fail to define universal reference
	{
		(void)param;
	}

	template<class T>
	class MyVector
	{
	public:
		void push_back(T&&){} // here it's also not u_ref because there is no type deduction
	};
}

TEST(UniversalReferencesUnitTest, failed_to_define_universal_reference)
{
	std::vector<int> v;
	//f(v); not compiles: cannot bind ‘std::vector<int>’ lvalue to ‘std::vector<int>&&’
	f(std::vector<int>());

	int x = 0;
	f_const(0);
	//f_const(x); not compiles: cannot bind ‘int’ lvalue to ‘const int&&’

	MyVector<int> mv;
	//mv.push_back(x); not compiles: cannot bind ‘int’ lvalue to ‘int&&’
	mv.push_back(0);

	(void)x;
}

namespace
{
	int foo()
	{
		return 42;
	}
}

TEST(UniversalReferencesUnitTest, universal_references_in_lambda)
{
	auto timeFuncInvocation = [](auto&& func, auto&&... params) // here it is u_ref
	{
		//start timer;
		return std::forward<decltype(func)>(func)(	std::forward<decltype(params)>(params)... );
		//stop timer;
	};
	auto func_ptr = foo;
	ASSERT_EQ(42, timeFuncInvocation(func_ptr));
	ASSERT_EQ(42, timeFuncInvocation(std::move(func_ptr)));
}

namespace
{
	std::multiset<std::string> names;

	std::string nameFromIdx(int idx)
	{
		return "abc";
	}
}

namespace simple_overload
{
	template<typename T>
	void logAndAdd(T&& name)
	{
		//auto now = std::chrono::system_clock::now();
		//log(now, "logAndAdd");
		names.emplace(std::forward<T>(name));
	}

	void logAndAdd(int idx)
	{
		//auto now = std::chrono::system_clock::now();
		//log(now, "logAndAdd");
		names.emplace(nameFromIdx(idx));
	}
}

TEST(UniversalReferencesUnitTest, avoid_overloading_on_a_universal_ref_first_part)
{
	using namespace simple_overload;
	std::string str("abc");
	logAndAdd(str); // ok. copies str
	logAndAdd(std::string("abc")); // ok. moves temporary
	logAndAdd(5); // ok. constructs string in place
	short idx = 5;
	(void)idx;
	//logAndAdd(idx); not compiles because there is no ctor in std::string taking short !!!
}

namespace overload_problem
{
	class Person
	{
	public:
		template<typename T>
		explicit Person(T&& n) : name(std::forward<T>(n))
		{}

		// Person(const Person& rhs) {}; // copy ctor (compiler-generated)
		// Person(Person&& rhs); // move ctor (compiler-generated)
	private:
		std::string name;
	};

	class SpecialPerson: public Person
	{
	public:
		// this two compiles because it calls perfect forwarding ctor

		//SpecialPerson(const SpecialPerson& rhs) : Person(rhs)
		//{ }
		//SpecialPerson(SpecialPerson&& rhs) : Person(std::move(rhs))
		//{ }
	};
}

TEST(UniversalReferencesUnitTest, avoid_overloading_on_a_universal_ref_second_part)
{
	using namespace overload_problem;
	Person p("a");
	Person copy1 = p; // this compiles because there is no way to call perfect forwarding ctor
	//Person copy2(p); //not compiles because perfect forwarding ctor is a better match

	const Person b("b");
	// this compiles because now better match is compiler-generated copy ctor
	Person copy3(b);
}

namespace tag_dispatch
{
	template<typename T>
	void logAndAddImpl(T&& name, std::false_type)
	{
		//auto now = std::chrono::system_clock::now();
		//log(now, "logAndAdd");
		names.emplace(std::forward<T>(name));
	}

	template<typename T>
	void logAndAdd(T&& name);

	void logAndAddImpl(int idx, std::true_type)
	{
		logAndAdd(nameFromIdx(idx));
	}

	template<typename T>
	void logAndAdd(T&& name)
	{
		logAndAddImpl(	std::forward<T>(name),
						std::is_integral<typename std::remove_reference_t<T>>()
					  );
	}
}

TEST(UniversalReferencesUnitTest, tag_dispatch_is_a_good_way_to_avoid_overloading_on_a_universal_ref)
{
	using namespace tag_dispatch;
	std::string str("abc");
	logAndAdd(str); // ok. copies str
	logAndAdd(std::string("abc")); // ok. moves temporary
	logAndAdd(5); // ok. constructs string in place
	short idx = 5;
	logAndAdd(idx); // ok. Now is handled by int overload
}

namespace sfinae_technique
{
	class Person
	{
	public:
		template< typename T, typename = std::enable_if_t<
			!std::is_base_of<Person, std::decay_t<T>>::value
			&&
			!std::is_integral<std::remove_reference_t<T>>::value
														 >
				>
		explicit Person(T&& n) // ctor for std::strings and args convertible to std::strings
		: name(std::forward<T>(n))
		{
			// this helps to avoid big error messages
			// assert that a std::string can be created from a T object
			static_assert(	std::is_constructible<std::string, T>::value,
							"Parameter n can't be used to construct a std::string" );
		}

		explicit Person(int idx) // ctor for integral args
		: name(nameFromIdx(idx))
		{  }

		// copy and move ctors, etc.
	private:
		std::string name;
	};

	class SpecialPerson: public Person
	{
	public:
		// now compiles

		SpecialPerson(const SpecialPerson& rhs) : Person(rhs)
		{ }
		SpecialPerson(SpecialPerson&& rhs) : Person(std::move(rhs))
		{ }
	};
}

TEST(UniversalReferencesUnitTest, sfinae_technique_helps_to_avoid_overloading_on_a_universal_ref_in_ctors)
{
	using namespace sfinae_technique;
	Person p("a");
	Person copy1 = p; // this compiles because there is no way to call perfect forwarding ctor
	Person copy2(p);

	const Person b("b");
	// this compiles because now better match is compiler-generated copy ctor
	Person copy3(b);
	//Person pu(u"Konrad Zuse"); not compiles, but it shouldn't
}

namespace reference_collapsing
{
	template <typename T>
	void foo_lvalue(T&& var)
	{
		static_assert( std::is_same< decltype(var), int&>::value, "types are not the same" );
		static_assert( std::is_same< T, int&>::value, "types are not the same" );
	}

	template <typename T>
	void foo_rvalue(T&& var)
	{
		static_assert( std::is_same< decltype(var), int&&>::value, "types are not the same" );
		static_assert( std::is_same< T, int>::value, "types are not the same" );
	}
}

TEST(UniversalReferencesUnitTest, reference_collapsing)
{
	using namespace reference_collapsing;
	int x = 0;
	foo_lvalue(x);

	foo_rvalue(5);

	auto&  lvalref1 = x;
	auto&& lvalref2 = x;
	//auto&  lvalref3 = 0; not compiles. Reference to an rvalue is forbidden.
	auto&& rvalref  = 0;

	static_assert( std::is_same< decltype(lvalref1), int&>::value, "types are not the same" );
	static_assert( std::is_same< decltype(lvalref2), int&>::value, "types are not the same" );
	static_assert( std::is_same< decltype(rvalref),  int&&>::value, "types are not the same" );
}
