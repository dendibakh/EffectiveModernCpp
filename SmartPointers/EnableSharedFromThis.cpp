/*
 * EnableSharedFromThis.cpp
 *
 *  Created on: May 16, 2016
 *      Author: bakhvalo
 */

#include "gtest/gtest.h"
#include <type_traits>
#include <vector>
#include <memory>
#include <algorithm>

namespace with_error
{
	int delete_calls_count = 0;
	std::vector<void *> obj_ptrs;

	class processData
	{
	public:
		void process(std::vector<std::shared_ptr<processData>>& data)
		{
			data.emplace_back(this);
		}
		static void operator delete (void *ptr)
		{
			if (std::find(obj_ptrs.begin(), obj_ptrs.end(), ptr) == obj_ptrs.end())
			{
				obj_ptrs.push_back(ptr);
				::delete (char*)ptr;
			}
			++delete_calls_count;
		}
		~processData() {}
	};
}

TEST(EnableSharedFromThisUnitTest, this_is_how_object_can_be_deleted_2_times)
{
	using namespace with_error;
	{
		std::shared_ptr<processData> external_ptr1(new processData());

		std::vector<std::shared_ptr<processData>> data;
		external_ptr1->process(data);
	}
	ASSERT_EQ(2, delete_calls_count); // object was deleted 2 times !!!
}

namespace fixed
{
	int delete_calls_count = 0;
	std::vector<void *> obj_ptrs;

	/*
	 *  std::enable_shared_from_this defines a member function that creates a
		std::shared_ptr to the current object, but it does it without duplicating control
		blocks. The member function is shared_from_this, and you use it in member func‐
		tions whenever you want a std::shared_ptr that points to the same object as the
		this pointer. Here’s a safe implementation of Widget::process:
	 */
	class processData : public std::enable_shared_from_this<processData>
	{
	public:
		void process(std::vector<std::shared_ptr<processData>>& data)
		{
			data.emplace_back(shared_from_this());
		}
		/*
		Internally, shared_from_this looks up the control block for the current object, and
		it creates a new std::shared_ptr that refers to that control block. The design relies
		on the current object having an associated control block. For that to be the case, there
		must be an existing std::shared_ptr (e.g., one outside the member function calling
		shared_from_this) that points to the current object. If no such std::shared_ptr
		exists (i.e., if the current object has no associated control block), behavior is unde‐
		fined, although shared_from_this typically throws an exception.
		 */
		static void operator delete (void *ptr)
		{
			if (std::find(obj_ptrs.begin(), obj_ptrs.end(), ptr) == obj_ptrs.end())
			{
				obj_ptrs.push_back(ptr);
				::delete (char*)ptr;
			}
			++delete_calls_count;
		}
		~processData() {}
	};
}

TEST(EnableSharedFromThisUnitTest, fix_a_bug)
{
	using namespace fixed;
	{
		std::shared_ptr<processData> external_ptr1(new processData());

		std::vector<std::shared_ptr<processData>> data;
		external_ptr1->process(data);
	}
	ASSERT_EQ(1, delete_calls_count);
}

TEST(EnableSharedFromThisUnitTest, shared_from_this_can_throw)
{
	using namespace fixed;
	try
	{
		processData dat;

		std::vector<std::shared_ptr<processData>> data;
		dat.process(data);
		FAIL() << "Exception should be thrown";
	}
	catch (std::exception&)
	{

	}
}
