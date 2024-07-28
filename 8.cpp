//lecture 8 c++

/*

UNIONS in C++
	in C unions are safe type, in C++ they are hell
	in C++ unions have constructors, in which we can decide, what to make
	
	what is inheritance?
	
	class A {};
	class B :public A {};	
	
	that means that B contains part from A and additionaly something else
	and both of them will share same pointer
	so, we can travel from ptr to A to ptr to B using static_cast
	if we are sure that concrete A is also concrete B
	
	So, public inheritance is a relationship "IS-A" (B is also A)
	
	upcast derived -> parent is implicit 
	downcast parent -> derived via static_cast 
		(if we are sure that it's indeed the case)
		
	Open inheritance have two not binded meanings:
		-B enhances A
		-B is a particular case of A
	Only if these two meanings are met, we can inherit
	This is LSP - Liskov Substitution Principle
	
	If architecture is correct, We always can use
	Derived* instead of Base* (and Derived& instead of Base&)
	
SLICE BASE PROBLEM

	see below struct A and struct B
	
	Slice base is because assignment (=) is not polymorphic
	
	Polymorphic function behaves differently with different types of args

POLYMORPHISM

	We have classes Triangle and Polygon
	both of them have method square(), calculating their square
	can we make vector of random Triangles and Polygons so they have this method?
	Yes, we can. Need to make common interface:
	
	struct ISquare { void square(); };
	struct Triangle : public ISquare;	
	struct Polugon : publuc ISquare;
	
	std::vector<ISquare*> v; //keeps Triangle* and Polygon*
	
	in C++ there are virtual methods to handle this.
	
	if class have at least one virtual method, 
	vptr (virtual function table, vtable) added to this class
	
	ctor of base class allocates memory for this table
	
	ctor of every child initializes it with child's proper methods pointers
	
	
	during construction of child: 
		- base class object first (because vtable creates here also)
		- fields of child are constructed second
		- vtable completed and initalized after end of ctor body of child
		
	Static type: tyte known during compilation (for example, common
	interface type as argument in function - static type.
	when runtime, concrete types can appear instead of this interface 
	these concrete types are dynamic types)
	
	When we talk about virtual fuctions' polymorphism - it's dynamic polymorpfism
	template functions - static polymorphism
	
	example:
	struct Matrix {
		virtual void pow(int x); //exponentation of any matrix
	};
	
	struct SparceMatrix : public Matrix {
		void pow(long x);	//exponentation of sparce matrix - effective
	};
	
	in example below, since different types of args (int vs long)
	second function will never be called via pointer to base type
	because it was overloaded, not overrode.
	So, it's not in vtable of base class
	
	That was a mistake, but to avoid such mistakes, use keyword override
	that means "never overload - always override"
	
	In Base class - put virtual
	In Derived classes - put override
	
	Override guarantees that function will appear in Vtable
	
	---
	
	Pure virtual method.
	syntax: virtual void MyMethod() = 0;
	pure virtual methods prohibit creation of class objects
	(Abstract class)
	
	Every derived class have to override all pure virt. methods of base class
	
	If we need to make Abstract base class, but no methods to make pure virtual,
	We can make pure virtual destructor, BUT we have to write it's body anyway
	
	Pure virtual methods may have body. We cannot create object,
	but we still can call base pure virtual methods from derived classes'
	methods.
	
	---
	
	Virtual destructors
	
	they allow to destroy derived class object via base class pointer
	
	---
	
	Problem:
	How to make a copy of a derived class using pointer to base class?
	Because virtual ctor (including copy-ctor) is impossible
	
	-Factories?
	Yes, but... 
	Clone!
	
	struct ISquare {
		//something
		virtual ISquare* clone() const = 0;
	};
	
	struct Triangle : public ISquare{
		Triangle* clone() const override {
			return new Triangle{pts_}; 
		}
	};
	
	--- 
	
	Again slice base
	
	If we pass Base class as argument by value, 
	Derived part will be sliced off
	
	So, pass base class via reference or via pointer
	
	---
	
	keyword "final"
	
	means: we cannot inherit from this class
	if we try, it's compilation error
	
	struct Foo final {
		// content
	};
	
	---
	
	How to write classes:
	
	- Write virtual destructor
	- Put special word "final"
	(these two are "correct" ways)
	
	- Stateless (EBCO)
	- Protected destructor
	
	
	EBCO: Empty Base Class Optimisations
	Applies when base class is empty:
	
	class A {};
	class B : public A {};
	A a; assert(sizeof(a) == 1);
	B b; assert(sizeof(b) == 1);
	
	so, if we inherit from empty class, it's object "removed"
	because base class is empty
	But class with at least one virtual method is NOT empty
	These empty classes are used for mixins
	To derive from empty base classes is useful to get methods from them
	Or to use such stateless classes as tags
	
	
	Protected Dtor: 
	class cannot be destructed from outside, 
	but can be destroyed by derived class
	
	class PureBase {
		//something
	protected:
		~PureBase() {}
	};
	
	now derived class object cannot be deleted via pointer to Base class
	no need to make dtor virtual
	
	---
	How virtual functions affect performance?
		- Performance decreases?
			- Yes. Virt func called by pointer (at least one ptr)
	
	How they affect stable?
		- No affects if correctly used?
			- Actually, it creates a pile of new possible errors. So, affects

	---
	PVC: pure virtual call
	
	When we try to call pure virtual function
	
	struct Base {
		Base() { doIt(); }	//PVC 
		virtual void doIt() = 0;
	};
	
	struct Derived : public Base {
		void doIt() override {}
	};
	
	int main() {
		Derived d;		//PVC because doIt not yet overrode
									// (vtable done in the end of ctor)
	}
	
	in easy cases compiler can detect it, but if call over another func,
	program will compile, but crash during exec
	
	---
	Virtual functions in ctors and dtors:
	Even if they not lead to PVC, they works as NON-VIRTUAL!
	
	So, be careful when call methods in ctors/dtors 
	(maybe better to avoid using methods in ctors/dtors?)
	In ctor Vtable STILL not exist,
	In dtor Vtable ALREADY not exist
	
	---
	Late binding: (dynamic binding)
	virtual functions are late binded
	
	normal functions have static binding
	
	Interesting:
	Default args are also have static binding
	
	struct Base {
		virtual int foo(int a = 14) { return a; }
	};
	
	struct Derived : public Base {
		int foo(int a = 42) override { cout<<"derived\n"; return a; }
	};
	
	Base *pb = new Derived{};
	std::cout<< pb->foo() <<std::endl;		//derived 14 (called derived::foo, but arg from base)
	
	This is because Vtable do not have space for default args, right? :)
	
	Do not use default args with virtual functions
	
	---
	NVI: Non-virtual interface
	struct BaseNVI {
		int foo(int x = 14) { return foo_impl(x); }
		
	private:
		virual int foo_impl(int a) { return a; }
	};
	
	struct Derived : public Base {
		int foo_impl(int a) override { return a; }
	};
	
	private virtual function, but overrode as public - it's OK
	We can use foo with default args, and reimplement in Derived class
	
	One more time: override can "make private -> public"
	
	---
	Virtual functions overload
	
	Yes, it's possible, But:
	
	struct Matrix {
		virtual void pow(double x);
		virtual void pow(int x);
	};
	struct SparseMatric: public Matrix {
		using Matrix::pow;		//this is to inject Matrix::pow in SparseMartix scope
		void pow(int x) override;
	};
	
	SparseMatrix d;
	d.pow(1.5);	//we want Matrix::pow(double)
							//and if we added it into SparseMatrix scope, it works
							
	---
	Private inheritance 
	Models relation "part-of"
	
	Private inheritance:
	class Whole : private Part {
		//everything else
	};

	Composition:
	class Whole{
		//everything else
	private: Part p_;	
	};
	
	-Composition should be default choice
	-But we use private inheritance if:
		- We need to override virt functions of Base class
		- We need access to protected fields of base class
		- We need to use using and add names from base class into our scope
	
	
	---
	Case study
	
	class Array {
	protected:
		IBuffer *buf_;
	public:
		explicit Array(IBuffer* buf) : buf_(buf) {}
		//something
	};
	
	Need to write MyArray derived from Array, and using MyBuffer, derived from IBuffer
	
	If try this:
	class MyArray : public Array {
	protected:
		MyBuffer mbuf_;
	public:
		explicit MyArray(int size) : mbuf_(size), Array(&mbuf_) {}
		//...
	};
	
	Will not work since buffer can not be initialized before base class
	
	try this solution:
	class MyArray : protected MyBuffer, public Array {
	public: 
		explicit MyArray(int size) : MyBuffer(size), Array( ??? ) {}
			//What to put as Array arg???
	};
	
	To handle this problem, we will create proxy class:
	
	struct ProxyBuffer {
		MyBuffer buf;
		explicit ProxyBuffer(int size) : buf(size) {}
	};
	
	class MyArray : protected ProxyBuffer, public Array {
	public:
		explicit MyArray(int size) : ProxyBuffer(size), Array(&ProxyBuf::buf) {}
	};
	
	//Now it works
*/

#include <iostream>
#include <vector>
#include <string>

using std::cout;

union U{
	std::vector<int> v_;
	std::string s_;
	
	U(std::string s) { new(&s_) std::string(s); }	//*
	U(std::vector<int> v) { new(&v_) std::vector<int>(v); }
	~U() {} //here =default will not work (=default here means there is no dtor at all
	// ! Who will release memory?
	//-User of this union, which is comlicated and error-prone
	
	//*	see placement new 
	// new(&s_) string(s)  creates string in pre-allocated memory in addres 
	// passed as argument (in this case, &s_)
};



enum Node_t {		
	VALUE,
	BINOP
};

enum BinOp_t {
	ADD,
	DEDUCT
};

struct Node {		//base class
	Node *parent_;
	Node_t type_;
};
 
struct BinOp : public Node {	//derived class
	BinOp_t op_;
	Node *rhs_ = nullptr, *lhs_ = nullptr;
	
	BinOp(Node *parent, BinOp_t opcode) : Node{parent, Node_t::BINOP}, op_(opcode) { }
};

void foo(const Node &pn) { cout<<"inside foo\n"; }


//for slice problem
struct A {
	int a_;
	A(int a) : a_(a) {}
};

struct B {
	int b_;
	B(int b) : A(b/2), b_(b) {}
};


//Polymorohism
template <typename T>
struct Point {
	T x,y,z;
};


//below 3 mistakes
struct ISquare {
	virtual double square() const;	//should be = 0   to make ISquare pure virtual
	//should be virtual destructor ? Yes!
};

template <typename T> struct Triangle : public ISquare {
	Point<T> x,y,z;
	double square() const;	//should have attribute override? Yes
};



int main(){
	
	BinOp *b = new BinOp(nullptr, BinOp_t::ADD);
	foo(*b);	//ok, implicit upcast derived -> parent
	Node *pn = b;	//ok, upcast
	b = static_cast<BinOp*>(pn); //for downcast need to use static_cast
	
	//Slice Base problem
	B b1(10);  //b1.a_ = 5, b1.b_ = 10
	B b2(8);		//b2.a_ = 4, b2.b_ = 8
	A& a_ref = b2;	//implicit upcast B to A
	a_ref = b1;			//b2.a_ = 5, b2.b_ = 8  (A does not know about B)
	
	return 0;
}