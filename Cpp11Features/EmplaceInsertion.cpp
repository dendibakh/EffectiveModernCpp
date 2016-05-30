/*
 * EmplaceInsertion.cpp
 *
 *  Created on: May 30, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <vector>
#include <string>
#include <set>
#include <list>
#include <memory>
#include <regex>

// General idea for using emplacement methods is to avoid excessive temporaries.
// And avoid calling constructor/destructor for this temporary.
// Those tests show when there is advantage and where is not in using emplacement.

TEST(EmplaceInsertionUnitTest, value_being_added_is_constructed_into_the_container_not_assigned)
{
	std::vector<std::string> vs;
	// Here vector will move-assign the new value into place.
	// But move assignment requires an object to move from, and that means that a
	// temporary object will need to be created to be the source of the move
	// Meaning no gain in using emplacement.
	vs.emplace(vs.begin(), "xyzzy");
}

TEST(EmplaceInsertionUnitTest, the_argument_type_being_passed_differ_from_the_type_held_by_the_container)
{
	std::vector<std::string> vs;

	vs.emplace_back("xyzzy");				// this is much faster
	vs.emplace_back(std::string("xyzzy"));	// than this.
}

TEST(EmplaceInsertionUnitTest, the_container_is_unlikely_to_reject_the_new_value_as_a_duplicate)
{
	/*
	The reason this matters is that in order to detect whether a value
	is already in the container, emplacement implementations typically create a node
	with the new value so that they can compare the value of this node with existing
	container nodes. If the value to be added isn’t in the container, the node is linked
	in. However, if the value is already present, the emplacement is aborted and the
	node is destroyed, meaning that the cost of its construction and destruction was
	wasted
	*/
	std::set<std::string> set;
	set.emplace("abc");
	set.emplace("abc"); // here performance is the same as for simple insert
}

namespace
{
	class Widget
	{

	};
}

TEST(EmplaceInsertionUnitTest, do_not_use_emplacement_for_resource_management_elements)
{
	std::list<std::unique_ptr<Widget>> ptrs;

	ptrs.push_back(std::unique_ptr<Widget>(new Widget)); // this is ok.
	ptrs.emplace_back(new Widget); // potential memory leak.

	/*
	   1. The raw pointer resulting from “new Widget” is perfect-forwarded to the point
		inside emplace_back where a list node is to be allocated. That allocation fails,
		and an out-of-memory exception is thrown.
 	   2. As the exception propagates out of emplace_back, the raw pointer that was the
		only way to get at the Widget on the heap is lost. That Widget (and any resources
		it owns) is leaked.
	 */
}

TEST(EmplaceInsertionUnitTest, emplacement_is_impacted_by_type_conversions_in_explicit_constructors)
{
	std::vector<std::regex> regexes;

	//regexes.push_back(nullptr);		// not compiles copy init forbids use of that ctor

	//regexes.emplace_back(nullptr);	// compiles. Which is bad! Direct init permits
										// use of explicit std::regex ctor taking a pointer
										//  Fails in runtime.

	// because:
	// push_back performs
	// std::regex r1 = nullptr; // error! won't compile. (Constructor is explicit).
	// but emplace performs
	// std::regex r2(nullptr); // compiles
}
