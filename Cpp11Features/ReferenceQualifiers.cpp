/*
 * ReferenceQualifiers.cpp
 *
 *  Created on: May 11, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <vector>

class Data
{
public:
	using DataType = std::vector<int>;
	DataType& getData() & // overload if *this is lvalue.
	{
		data.emplace_back(1);
		return data;
	}
	DataType getData() && // overload if *this is rvalue.
	{
		data.emplace_back(2);
		return std::move(data);
	}
private:
	DataType data{};
};

Data makeData()
{
	return Data{};
}

TEST(RefQualifiersUnitTest, usage_test)
{
	Data d;
	ASSERT_EQ(1, d.getData()[0]);
	ASSERT_EQ(2, makeData().getData()[0]);
}


