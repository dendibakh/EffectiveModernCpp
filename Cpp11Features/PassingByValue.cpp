/*
 * PassingByValue.cpp
 *
 *  Created on: May 28, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

namespace reference_overloading
{
	class Widget
	{
	public:
		void addName(const std::string& newName)
		{ names.push_back(newName); }

		void addName(std::string&& newName)
		{ names.push_back(std::move(newName)); }

	private:
		std::vector<std::string> names;
	};
}

TEST(PassingArgsUnitTest, ref_overload)
{
	using namespace reference_overloading;
	Widget w;

	// calling with lvalue
	std::string name("Bart");
	w.addName(name); // cost: one copy

	// calling with rvalue
	w.addName(name + "Jenne"); // cost: one move

	// Additionally:
	// 1. Here we've got two functions to maintain.
}

namespace perfect_forwarding
{
	class Widget
	{
	public:

		template<typename T>
		void addName(T&& newName)
		{ names.push_back(std::forward<T>(newName)); }

	private:
		std::vector<std::string> names;
	};

}

TEST(PassingArgsUnitTest, perfect_forwarding)
{
	using namespace perfect_forwarding;
	Widget w;

	// calling with lvalue
	std::string name("Bart");
	w.addName(name); // cost: one copy

	// calling with rvalue
	w.addName(name + "Jenne"); // cost: one move

	// Additionally:
	// 1. This approach is subject to greedly catching parameters problem.
	// 2. This works superfast if caller will pass const char* instead of std::string.
	//    Because there will be no copies or moves involved.
}

namespace pass_by_value
{
	class Widget
	{
	public:
		void addName(std::string newName)
		{ names.push_back(std::move(newName)); }

	private:
			std::vector<std::string> names;
	};
}

TEST(PassingArgsUnitTest, pass_by_value)
{
	using namespace pass_by_value;
	Widget w;

	// calling with lvalue
	std::string name("Bart");
	w.addName(name); // cost: one copy + one move

	// calling with rvalue
	w.addName(name + "Jenne"); // cost: two moves

	// Additionally:
	// 1. One extra move, but code is quite small.
	// 2. Consider pass by value for copyable parameters that are cheap to move	and always copied.
	//    - It's no sense to pass by value parameters that can be only moved.
	//      Because one overload taking rvalue ref will be enough.
	//    - Pass by value if you will always copy or move it!
	//      Because if you don't it was no sense to pay for additional copy (parameter).
	// 3. When doing copy-assignment (not construction) analysis is more complicated.
	//    It's possible that the cost will increase even more.
	// 4. Pass by value is subject to the slicing problem, so it’s typically inappropriate
	//    for base class parameter types.
}
