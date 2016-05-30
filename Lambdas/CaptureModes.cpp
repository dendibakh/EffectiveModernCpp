/*
 * CaptureModes.cpp
 *
 *  Created on: May 26, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <vector>
#include <functional>
#include <memory>

namespace
{
	using filters = std::vector<std::function<bool(int)>>;

	void addLambdaReferenceMode(filters& f)
	{
		int divisor = 5;
		f.emplace_back([&](int x) { return x % divisor; } );

		// more explicitly:
		//f.emplace_back([&divisor](int x) { return x % divisor; } );
		//
		// Long-term, it’s simply better software engineering to explicitly list
		// the local variables and parameters that a lambda depends on.
	}

	void addLambdaValueMode(filters& f)
	{
		int divisor = 5;
		f.emplace_back([divisor](int x) { return x % divisor; } );
	}
}

TEST(LambdaUnitTest, avoid_default_by_reference_capture_mode)
{
	filters f;
	addLambdaReferenceMode(f); // undefined behaviour: reference to divisor is dangling
	addLambdaValueMode(f);
}

namespace
{
	class WidgetA
	{
	public:
		void addFilter(filters& f) const
		{
			f.emplace_back([=](int x) { return x % divisor; } );
		}
	private:
		int divisor{5};
	};

	// Rule: Captures apply only to non-static local variables (including parameters)
	//       visible in the scope where the lambda is created

	class WidgetB
	{
	public:
		void addFilter(filters& f) const
		{
			auto copyDivisor = divisor;
			f.emplace_back([copyDivisor](int x) { return x % copyDivisor; } );
		}
	private:
		int divisor{5};
	};

	class WidgetCpp14
	{
	public:
		void addFilter(filters& f) const
		{
			f.emplace_back([divisor = divisor](int x) { return x % divisor; } );
		}
	private:
		int divisor{5};
	};

	void addLambdaStaticValueMode(filters& f)
	{
		static int divisor = 0;
		divisor++;
		f.emplace_back([=](int x) { return x % divisor == 0; } );
	}
}

TEST(LambdaUnitTest, avoid_default_by_value_capture_mode)
{
	filters f;
	{
		WidgetA a;
		a.addFilter(f);
	} // inside addFilter() pointer to this (a) was captured. Now it's dangling.

	{
		WidgetB b;
		b.addFilter(f); // that's fine because this pointer was not captured
	}

	{
		WidgetCpp14 b;
		b.addFilter(f); // that's also fine.
	}

	filters f2;
	addLambdaStaticValueMode(f2);
	addLambdaStaticValueMode(f2);
	// inside addLambdaStaticValueMode reference to divisor was captured
	// so, all lambdas inside container will have the same divisor
	// could be fixed by generalized capture mode.

	ASSERT_TRUE(f2[0](2));
	ASSERT_TRUE(f2[1](2));
}

namespace
{
	class Widget
	{
	public:
		bool isValidated() { return true;}
		bool isArchived() { return false;}
	};
}

TEST(LambdaUnitTest, moving_objects_into_closures)
{
	auto funcCpp14 = [pw = std::make_unique<Widget>()]()
					 { return pw->isValidated() && pw->isArchived();};

	// In c++11 you can emulate it with std::bind.
	// Here first the member inside bind object is move-constructed (!)
	// After that it is passed to the closure by const reference
	auto funcCpp11 = std::bind( [](const std::unique_ptr<Widget>& pw)
								 { return pw->isValidated() && pw->isArchived(); },
								 std::make_unique<Widget>()
								 );
}
