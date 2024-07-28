//lecture 5 C++

/*
	Memory is only one type of resources
	
	RAII: Resource Acquisition Is Initialisation
	
	in Linux kernel: 
		cleanup idiome (centralized exit from functions)
	{
		S *p = new S();
		int result = SUCCESS;
		//some code ...
		if (condition){
			result = FAILURE;
			goto cleanup;
		}
		//some code ...
		cleanup:
			delete p;
			return result;
	}
	
	Why goto is bad?
		-spahetti code
		-makes graph from code (not tree, like normally) - so harder to analyse
		-goto arc can pass beginning of object's life
		
	but, for example, 'break', 'continue', 'switch' are also goto-masking constructions
	
	tip: use explicit blocks to be on safe side when working with goto-like constructions:
	switch(cond){
		case 0: {
			X x;
		}
		case 1: {
			return x.smth;		//error! control flow arc passed creation of x!
		}
	}
	
	in c++ we use destructors for freeing of resources instead of goto
	So, constructor takes ownership of object, and dtor releases it
	
	
MOVE SEMANTICS
	
	recap:
	we have lvalues and rvalues 
	lvalues are objects which have Location in memory (i.e. x)
	rvalues are expRessions (i.e. x+1)
	
	We want to assign a new name to expression (make lvalue from rvalue)
	reference to rvalue: 
		take expression->
		bind expression and name->
		materialize result of expression
	
	int &&y = x + 1;				//and now y is lvalue (it has location)
	
	can we use x as rvalue?
	yes:
	int&& y = std::move(x);	//"move y to x"
	
	now name y is binded to location of value of x.
	(two different names of this data: x and y),
	we can change this data using both of these names
	
	easier to think about it as about simple type conversion (lvalue ref -> rvalue ref)
	
	some rules:
		-rvalue cannot be binded with lvalue 
			int x = 1;
			int &&y = x + 1; 	//OK
			int &&b = x;			//fail, not rvalue
		-non-const lvalue reference cannot be binded with rvalue
			int &c = x + 1;		//fail, not l value
			const int &d = x + 1;			//OK, lifetime prolonged till end of scope
		-rvalue reference after initialisation becomes lvalue by itself
			int &&y = x + 1;
			int &f = y;			//OK
			int &&e = y; 		//fail! y is lvalue already (have location in memory)
			
	
	Methods can be called for rvalue-expressiions:
	
	struct S{
		int n = 0;
		int& access() { return n; }
	};
	
	S x;
	int &y = x.access();	//ok
	int& z = S{}.access(); 	//ok, S is tmp object, but z dangles
	
	reminder: tmp object lives till end of full expression
	
	methods can be ref-annotated. 
	they overloads regarding they called for rvalue or lvalue
	
	struct S {
		int foo() &;	//1
		int foo() &&;	//2
	};
	
	extern S bar();
	S x{};
	
	x.foo();	// case 1 because x is lvalue;
	bar().foo();		//case 2 because rvalue created;
	
	
	----
	example of annotation:
	class X {
		vector<char> data_;
	public:
		X() = default;
		vector<char> const & data() const & { return data_; };	//called for lvalue
		vector<char> && data() && { return std::move(data_); };	//called for tmp object (rvalue) 
																									// so no need to worry about it
	};
	
	X obj;
	vector<char> a = obj.data();	//copy
	vector<char> b = X().data();	//move
	-----
	
	be careful with return of rvalue references
	only return rvalues when method is rvalue-annotated
	
	---
	examples:
		const int& bar(const int& x) {return x; }	//can dangle if x is rvalue
																			//lifetime extended till end of scope only
		
		int&& buz(int&& x) { return std::move(x); };	//always dangle! parrot is dead, 
																					//object maybe still on stack, but it can be owerwritten
	---
	
	So, we have a good way to distinguish valuable things (lvalue) from non-valuable things (rvalue)
	
MOVE CONSTRUCTORS AND ASSIGNMENTS:
	See realisation of move ctor and move assignment in code below
	
	! move operator= lefts its right side consistent, but not guaranteed predictable state
	
	! move on result is not nessecary if return by value (it's rvalue expression anyway)
	T foo(args) {
		T x = something;
		return std::move(x);		//not mistake, but why?
	}
	
	Move on result may kill RVO.
	Only use move on result in case return rvalue reference, when method is rval-annotated (see above)
	
	Move constructors and move assignments may be defined implicitly by compiler
	In this case both (copy, move ctors) of them make bitwise copy
	
RULE OF FIVE:
	If class requires at least one of following five methods:
		-Copy-ctor,
		-Copy-assignment,
		-Move-ctor,
		-Move-assignment,
		-Destructor
	to be non-trivial, better to define non-trivially all of them
	
RULE OF ZERO:
	If class requires non-trivial methods from rule of five,
	it should not be any other methods in this class 
	(SOLID, SRP (single responsibility principle))
	
MOVE FROM CONST CORNER CASE:
	const MyPointer<int> y{new int{10}};
	MyPointer<int> b = std::move(y);				//copy-constructor will be called
		//because move ctor have signature: MyPointer(MyPointer&&)
		//and in fact we have const MyPointer&&
		//so, it will be converted to const MyPointer&
		//and copy constructor will be called
	
MATRIXES

	in math first index = row, second = column
	a_11 a_12 a_13
	a_21 a_22 a_23
	a_31 a_32 a_33
	
	in C read matrixes like books  (11 12 13 21 22 23)
	
	row-major means that first changes the outmost index
	int one[5];						//5 columns
	int two[1][5];				//1 row, 5 columns
	int three[1][1][5];   //1 layer, 1 row, 5 columns
	above arrays use same amount of memory
	
	how to locate memory cell?
	int a[7][9];	//declaration
	int elt = a[2][3];		//3rd column of 2nd row
	//address offset = (2 * 9 + 3) * sizeof(int);
	
	two ways of solving of addresses:
	
		-array-like style:
	
			int first[FX][FY];
			first[x][y] = 3;		// -> *( &first[0][0] + x * FY + y ) = 3;
			
			int (*second)[SY];
			second[x][y] = 3;		// -> *( &second[0][0] + x * SY + y ) = 3;
			
		-pointer-like style:
		
			int *third[TX];
			third[x][y] = 3;		// -> *( *(third + x) + y ) = 3;
			
			int **fourth;
			fourth[x][y] = 3;		// -> *( *(fourth + x) + y ) = 3;
			
		Note that no need to know size of array in pointer-like style (jagged vector)
	
	Arrays initialisation:
	float flt[2][3] = {{1.0, 2.0, 3.0}, {4.0, 5.0}};		//ok, 0 will be added in second elem
	float flt[][3] = {{1.0, 2.0, 3.0}, {4.0, 5.0}};		//ok, can be resolved by initialisation
	float flt[][] = {{1.0, 2.0, 3.0}, {4.0, 5.0}};		//fail! cannot be resolved
	
	General rule: only most folded index can be omitted
*/

#include <iostream>

using std::cout;
using std::endl;

//Class for RAII pointer
template<typename T>
class MyPointer{
	T* ptr_;
public:
	/* explicit */ MyPointer(T *ptr = nullptr) : ptr_(ptr) { cout<<"ctor\n"; }
	~MyPointer() { delete ptr_; cout<<"dtor\n"; }
	
	//here we assume that template type T is copyable, but this is strong assumption...
	MyPointer(const MyPointer& rhs) : ptr_{ new T{*rhs} } { cout<<"copy ctor\n"; }
	
	MyPointer& operator=(const MyPointer& rhs){
		cout<<"assingment\n";
		if(&rhs == this)
			return *this;
		delete ptr_;
		ptr_ = new T{*rhs};	//operator* overloaded below
		return *this;
	}
	
	//Move constructor
	MyPointer(MyPointer&& rhs) : ptr_(rhs.ptr_) { rhs.ptr_ = nullptr; cout<<"Move ctor\n"; }
	
	//Move assignment
	MyPointer& operator=(MyPointer&& rhs){
		cout<<"move assingment\n";
		if(&rhs == this)
			return *this;			//still need to check situation (a = std::move(a));
		//variant 1:
		delete ptr_;
		ptr_ = rhs.ptr_;
		rhs.ptr_ = nullptr;
		return *this;
		//end of variant 1.   rhs left in consistent state
		//variant 2:
		std::swap(ptr_, rhs.ptr_);
		return *this;
		//end of variant 2			rhs will be destructed after this scope anyway
		//variant 3:
		ptr_ = std::move(rhs.ptr_);		//??
		return *this;
		//end of variant 3
	}
	
	//we want to use dereference operator for convenience ((*my_pointer).x)
	T& operator*() { return *ptr_; }
	const T& operator*() const { return *ptr_; }
	
	//but also we want to use operator-> (my_pointer->x)
	T* operator->() { return ptr_; };
	const T* operator->() const { return ptr_; }
	//it's a bit not intuitive.
	//my_pointer->x === (my_pointer.operator->())->x   and so on
	//arrow "diving into" as much as it can (drill down behaviour)
	//it's special semantic of arrow
	
	
};


void foo(MyPointer<int> my_pointer){
	cout<<"inside foo"<<endl;
}

int main() {
	
	//interesting: we can pass normal pointer in function that requires MyPtr
	//and it implicitly convert into our type:
	{
		int *pi = new int{5};
		foo(pi);
		//if we make explicit constructor, it will not work
	}
	{
	
	MyPointer a{new int{1}}, b{new int{2}};
	cout<<"\nswap\n";
	cout<<"a = " << *a <<"   b = "<<*b<<endl;
	std::swap(a, b);
	cout<<"end of swap\n";
	cout<<"a = " << *a <<"   b = "<<*b<<endl;
	}
	
	//move semantics
	int x = 4;
	int &&y = x+1;
	cout<<&x<<" "<<&y<<endl;
	
	int &&z = std::move(x);
	z = z + 1;	//we change z, but it binded to same location as x, so we also change x
	y = y + 1;	//y is separate object already
	cout<< x << " " << y <<endl;
	
	
	return 0;
}