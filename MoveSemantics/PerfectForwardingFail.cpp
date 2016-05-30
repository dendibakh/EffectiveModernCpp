/*
 * PerfectForwardingFail.cpp
 *
 *  Created on: May 25, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <vector>
#include <chrono>
#include <set>

namespace
{
	void f(int)
	{

	}

	void f(const std::vector<int>&)
	{

	}

	template<typename... Ts>
	void fwd(Ts&&... params)
	{
		f(std::forward<Ts>(params)...);
	}
}

TEST(PerfectForwardingFailuresUnitTest, braced_init)
{
	f({1, 2, 3});
	//fwd({1, 2, 3}); // not compiles. Compiler can't deduce std::intializer_list for calling fwd.
	fwd(std::vector<int>({1, 2, 3}));

	auto il = { 1, 2, 3 };
	fwd(il); // now fine, because there is no need to deduce the type
}

namespace
{
	struct A
	{
		static const int val = 18;
	};

	struct B
	{
		static const int val = 18;
	};

	const int B::val;
}

TEST(PerfectForwardingFailuresUnitTest, static_int_consts)
{
	//fwd(A::val); //not links. Because A::val declaration is missing. Address of A::val couldn't be retrieved.
	fwd(B::val);
}

namespace
{
	void fp(int pf(int))
	{
		(void)pf;
	}

	int processVal(int value) {return 0;}
	int processVal(int value, int priority) {return 0;}

	template<typename T>
	T workOnVal(T param)
	{ return T{}; }

	template<typename... Ts>
	void fwdtoFunc(Ts&&... params)
	{
		fp(std::forward<Ts>(params)...);
	}
}

TEST(PerfectForwardingFailuresUnitTest, function_pointers_and_template_funcs)
{
	(void)processVal(5,5);
	fp(processVal);
	//fwdtoFunc(processVal); // not compiles. Which processval to call?

	fp(workOnVal);
	//fwdtoFunc(workOnVal); //not compiles. Which instanstiation?

	using ProcessFuncType =	int (*)(int);
	ProcessFuncType func_pointer = processVal;
	fwdtoFunc(func_pointer);

	fwdtoFunc(static_cast<ProcessFuncType>(workOnVal));
}

namespace
{
	struct BitField
	{
		char c:4;
	};

	void foo(int)
	{

	}

	template<typename... Ts>
	void fwdBitField(Ts&&... params)
	{
		foo(std::forward<Ts>(params)...);
	}
}

TEST(PerfectForwardingFailuresUnitTest, bitfields)
{
	BitField bf {0};
	foo(bf.c);
	// fwdBitField(bf.c); //not compiles. References to bitfields are forbidden.
}
