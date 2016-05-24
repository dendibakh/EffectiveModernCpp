#include "gtest/gtest.h"
#include <type_traits>
#include <utility>

class emptyClass
{

};

TEST(SpecMemFuncGenUnitTest, empty_class_has_everything)
{
	emptyClass a;
	emptyClass b(a);
	a = b;
	emptyClass c(std::move(a));
	b = std::move(c);
}

class classWithCopyCtor
{
public:
	classWithCopyCtor() {}
	classWithCopyCtor(const classWithCopyCtor&)	{ x = 1;}
	int x {0};
};

TEST(SpecMemFuncGenUnitTest, copy_ctor_do_not_disable_assignment_but_disable_moves)
{
	classWithCopyCtor a;
	classWithCopyCtor b(a);
	a = b;
	classWithCopyCtor c(std::move(a)); // here copy is performed !!!
	ASSERT_EQ(1, c.x);
	b = std::move(c); // here copy is performed !!!
}

class classWithAssignmentOperator
{
public:
	classWithAssignmentOperator() {}
	classWithAssignmentOperator& operator=(const classWithAssignmentOperator&) { x = 1; return *this; }
	int x {0};
};

TEST(SpecMemFuncGenUnitTest, assignment_operator_do_not_disable_copy_ctor_but_disable_moves)
{
	classWithAssignmentOperator a;
	classWithAssignmentOperator b(a);
	a = b;
	classWithAssignmentOperator c(std::move(a)); // here copy is performed !!!
	b = std::move(c); // here copy is performed !!!
	ASSERT_EQ(1, b.x);
}

class classWithMoveCtor
{
public:
	classWithMoveCtor() {}
	classWithMoveCtor(classWithMoveCtor&& rhs) { }
};

TEST(SpecMemFuncGenUnitTest, move_ctor_disable_move_operator_and_copy_operations)
{
	classWithMoveCtor a;
	//classWithMoveCtor b(a); do not compiles
	classWithMoveCtor b;
	//a = b; do not compiles
	classWithMoveCtor c(std::move(a));
	//b = std::move(c); do not compiles
}

class classWithMoveOperator
{
public:
	classWithMoveOperator() {}
	classWithMoveOperator& operator=(classWithMoveOperator&& rhs) { return *this; }
};

TEST(SpecMemFuncGenUnitTest, move_operator_disable_move_ctor_and_copy_operations)
{
	classWithMoveOperator a;
	//classWithMoveCtor b(a); do not compiles
	classWithMoveOperator b;
	//a = b; do not compiles
	// classWithMoveOperator c(std::move(a));do not compiles
	b = std::move(a);
}

// Be carefull here!!!
// Declaring user destructor prevent compiler from generating
// move operations. For example if class would have std::string
// it will be copied - not moved.

class classWithDestructor
{
public:
	classWithDestructor() {}
	classWithDestructor(const classWithDestructor&)	{ x = 1;}
	classWithDestructor& operator=(const classWithDestructor&) { x = 1; return *this; }
	~classWithDestructor() {}
	int x {0};
};

TEST(SpecMemFuncGenUnitTest, declared_destructor_do_not_disable_copy_but_disable_moves)
{
	classWithDestructor a;
	classWithDestructor b(a);
	a = b;
	classWithDestructor c(std::move(a)); // here copy is performed !!!
	b = std::move(c); // here copy is performed !!!
}

class classWithDestructorAndMove
{
public:
	classWithDestructorAndMove() {}
	classWithDestructorAndMove(const classWithDestructorAndMove&)	{ x = 1;}
	classWithDestructorAndMove& operator=(const classWithDestructorAndMove&) { x = 2; return *this; }
	classWithDestructorAndMove(classWithDestructorAndMove&&)	{ x = 3;}
	classWithDestructorAndMove& operator=(classWithDestructorAndMove&&) { x = 4; return *this; }
	~classWithDestructorAndMove() {}
	int x {0};
};

TEST(SpecMemFuncGenUnitTest, proper_way_to_impl_class)
{
	classWithDestructorAndMove a;
	ASSERT_EQ(0, a.x);
	classWithDestructorAndMove b(a);
	ASSERT_EQ(1, b.x);
	a = b;
	ASSERT_EQ(2, a.x);
	classWithDestructorAndMove c(std::move(a));
	ASSERT_EQ(3, c.x);
	b = std::move(c);
	ASSERT_EQ(4, b.x);
}

// General rule:
// move operations are generated for classes (when needed) only if these three things are true:
// • No copy operations are declared in the class.
// • No move operations are declared in the class.
// • No destructor is declared in the class.

/*
The C++11 rules governing the special member functions are thus:
• Default constructor: Same rules as C++98. Generated only if the class contains
no user-declared constructors.
• Destructor: Essentially same rules as C++98; sole difference is that destructors
are noexcept by default (see Item 14). As in C++98, virtual only if a base class
destructor is virtual.
• Copy constructor: Same runtime behavior as C++98: memberwise copy con‐
struction of non-static data members. Generated only if the class lacks a user-
declared copy constructor. Deleted if the class declares a move operation.
Generation of this function in a class with a user-declared copy assignment oper‐
ator or destructor is deprecated.
• Copy assignment operator: Same runtime behavior as C++98: memberwise
copy assignment of non-static data members. Generated only if the class lacks a
user-declared copy assignment operator. Deleted if the class declares a move
operation. Generation of this function in a class with a user-declared copy con‐
structor or destructor is deprecated.
• Move constructor and move assignment operator: Each performs memberwise
moving of non-static data members. Generated only if the class contains no user-
declared copy operations, move operations, or destructor.
*/
