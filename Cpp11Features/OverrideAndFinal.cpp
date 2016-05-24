/*
 * Override&Final.cpp
 *
 *  Created on: May 11, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>

//override
class Base
{
	virtual void foo(int, double) const {};
};

class Derived : public Base
{
	//void foo(int) const override {}; won't compile
	void foo(int, double) const override {};
};

//final
class Base_with_final
{
	virtual void foo(int, double) const final {};
};

class Derived_from_final : public Base_with_final
{
	//void foo(int, double) const{}; won't compile (overriding final function)
};


class Base_final final
{

};

// won't compile. Can't derive from final base class
/*
class Derived_final : public Base_final
{

};
*/
