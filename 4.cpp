//lecture 4 initialisation and copy

/*
aggregate initialisation

struct Node {
	KeyT key_;
	Node *parent_, *left_, *right_;
	int height_;
}

c-style:
Node n = {key, nullptr, nullptr, nullptr, 0 };
Node n = {key};	//others are zeros

C++:
Node n { key };	//others are zeros

aggregate initialisation does not work if 
	at least one private field 
	or
	virtual function



struct Node {
	KeyT key_;
	Node *parent_ = nullptr, *left_ = nullptr, *right_ = nullptr;
	int height_ = 0;

Node(KeyT key) : key_(key) {}
};

Node n(key);	//direct old style
Node n{key}; 	//normal constructor (rules below), new style
Node k = key;	//copy-initialisation;

figure brackets rules:
	- if aggregate, {} are always aggregate init
	- if constructor have initializer list, {} are always initializer list
	- if normal constructor, {} are normal constructor
	
when use () constructor call can be interpreted as function declaration:

myclass_t m(list_t(), list_t());		//this is a function declaration!
myclass_t m{list_t(), list_t()};			//ok

initializer list order:

struct X{
	int a_;
	char b_;

	X(char new_b, int new_a) : b_(new_b), a_(new_a) {}	//a_, b_
	
in above, doesnt matter order in init list, order in class declaration matters


better this:
struct Node{
	S key_;
	Node(KeyT key = 1) : key_(key) {}
}

than this:
struct Node{
	S key_ = 1;
	Node() {}		//key_(1)
	Node(KeyT key) : key_(key) {}
}

because first way is shorter and nice





*/

#include <iostream>

using std::cout;
using std::endl;

using KeyT = int;

struct S{
	S() {cout<<"default S"<<endl;}
	S(KeyT key) {cout<<"direct S"<<endl;}
};

struct Node{
	S key_; int val_;
	
	Node(KeyT key, int val) { key_ = key; val_ = val; }
	Node() {};
};

struct Node_eff{
	S key_; int val_;

	Node_eff(KeyT key, int val) : key_(key), val_(val) {}
};

struct Node_const{
	S key_;
	const int c_val_;
	const char c_inside_class_ = 'c';

	Node_const(KeyT key, int val = 123) : key_(key), c_val_(val) { cout<<"const = "<<c_val_<<endl;}
};

const int DEFAULT_A = 1;
const int DEFAULT_B = 10;


//Delegating constructor
struct ComplexStruct{
	int complex_a = 0;
	int complex_b = 0;
	ComplexStruct(int a) : complex_a(a > 0 ? a : DEFAULT_A) {}	//first constructor
	ComplexStruct(int a, int b) : ComplexStruct(a) {
		complex_b = (b > 0 ? b : DEFAULT_B);
	}
		//second constructor, which enhances first. 
		//It uses first constructor, and then initializes other fields
};



class Empty{	
	/*done by compiler: */
	Empty() = default;
	~Empty() = default;
	Empty(const Empty&) = default;
	Empty& operator= (const Empty&) = default;
};
/*
	Despite it's empty, it has default ctor, dtor, operator=, copy-ctor, move-ctor
	Well-designed code usually not requires many non-trivial ctors
	
*/

class Copyable{
	
};

template<typename T>
class Point2D{
	T x_, y_;
	/* by compiler:
	Point2D() : default-init x_, default-init y_ {}   
	~Point2D() {}
	Point2D(const Point2D& rhs) : x_(rhs.x_), y_(rhs.y_) {}
	Point2D& operator=(const Point2D &rhs) {
		x_ = rhs.x_; y_ = rhs.y_; return *this;
	}
	
	it makes bitwise copy and assignment for simple types and aggregates
	-calls copy-constructor (if exist) for user types
	
	In well-designed classes usually there's no need to write copy-constructors by ourselves
	
	
	*/
};


//example of custom constructors, destructor and assignment:
class Buffer{
	int *p_;
	int n_;		//size
public:
	Buffer (int n) : p_(new int[n]), n_(n) {}		//custom constructor
	~Buffer() { delete[] p_ };					//custom dtor
	Buffer(const Buffer& rhs) : n_(rhs.n_), p_(new int[n_]) {		//custom copy ctor
		//now need to copy all contents of buffer into new buffer
		//use std::copy (it_first, it_last, output_it)
		std::copy(rhs.p_, rhs.p_ + n_, p_);
	}
	Buffer& operator= (const Buffer& rhs) {		//custom assignment operator
		if(this == &rhs)		//important check that not like (a = a)
			return *this;
		delete p_[];		//flush old buffer first
		n_ = rhs.n_;
		p_ = new int[n_];
		std::copy(rhs.p_, rhs.p_ + n_, p_);
		return *this;
	}
	
	
/*
RVO - return value optimization
	!!! Object after copy-constructor in all respects equal to object it copied from
	In some cases it's possible to avoid unnesesary copy
	In this case copy-constructor will not be called
	It's done not by optimiser, but by language frontend
	
	------
	Compiler recognizes copy-constructor by its form: 
		-main form is a const reference:   Copyable (const Copyable& c);
		-also may be non-const reference: Copyable (Copyable& c);
		-also cv-qualified reference (const / volatile)
	-------
	
	! Template constructor is never copy-constructor
	
		//this is ok:
	template<typename T> struct Coercible {
		Coercible(const Coercible &c) { cout<<"Hello!"<<endl; }
	};
		
		//but this is not OK (compiler will make its own default copy-ctor):
	template<typename T> struct Coercible {
		template<typename U> Coercible(const Coercible<U> &c) { cout<<"Hello!"<<endl; }
	};
	
	
	example rvo:
	
	struct foo {
		foo() { cout<<"foo()"<<endl; }
		foo(const foo&) { cout<<"foo(const &foo)"<<endl; }
		~foo() {cout<<"~foo()"<<endl; }
	};
	
	foo bar() { foo local_foo; return local_foo; }
	
	int main() {
		foo f = bar();
	}
	
	output:
	foo()
	~foo()

CV-QUALIFIERS:
	const : I cannot change value of this name
	volatile: It can change externally, and I can change it also
	cons volatile: It can change externally, but I cannot change it
	
	int S::foo() const { return 42; }  //it not changes internal state of its object
																		//this method can be called if object is const
																		
	int S::bar() volatile { return 42; }	//this method can be called if object volatile
	
	int S::buz() const volatile {return 42; }	//this menthod can be called if object ( const || volatile )

INITIALIZATION SEMANTICS (and keyword explicit):
	
	usual ctors define IMPLICIT type conversion:
	
	struct MyString {
		char *buf_; size_t len_;
		MyString(size_t len) : len_{len}, buf_{new char[len_]{}} {}
	};
	
	void foo(MyString);
	foo(42);			//OK, MyString implicitly constructed ( foo(MyString(42)) )
	
	it's called user type conversions
	
	sometimes it's not desired
	in this case keyword "explicit" to be used
	
	this will not compile: 
	...
	explicit MyString(size_t len) : len_{len}, buf_{new char[len_]{}} {}
	...
	foo(42);		//error: could not convert '42' from 'int' to 'MyString'
	
	in most cases explicit is extreme
	no need to put it just so
	
	
USER CONVERSIONS (continue)

	How to convert MyType into int (for example)
	Need to use 'operator type'
	
	struct MyType {
		int value_;
		string smthng_;
		operator int() { return value_; }
	};
	
	we can put explicit, to allow only explicit conversions
	
	AND we can convert between our types:
	
	struct MySecondType {
		int value_;
		string smthng_else_;
		operator MyType() { return MyType(value_, smthng_else_); }
	};
	
	we have two ways how to convert objects:
	constructor: 		Foo::Foo(const Bar&)
	operator type: 	Bar::operator Foo()
	
	better to use constructors
	
	when direct initialization, only constructors are checked
	when copy-initialization, both checked, operator wins
	
	Bar b;
	Foo f1{b};	//direct initialization, ctor
	Foo f2 = b;	//copy init, operator wins
	
	
*/

int main(){
	
	cout<<"Calling inefficient constructor:"<<endl;
	
	Node inefficient_constructor(1,2);
	
	cout<<"2 constructors of 'S' were called. First was default to initialize field key_ before enter body of constructor. Another was called in body to assign correct value to key_"<<endl;
	
	
	/* in this case both constructors of S will be called.
	this is happen because 
	! When constructor enters its body, all fields must be initialized !
	*/
	
	cout<<"Now call efficiently..."<<endl;
	Node_eff efficient_constructor{1,2};
	cout<<"Only one constructor was called using initializer list"<<endl;
	
	
	cout<<"Now we will initialize const field of class using initializer list..."<<endl;
	Node_const(1,2);
	cout<<"Done!"<<endl;
	cout<<"Note that c_inside_class_ was pushed into initializer_list implicitly"<<endl;
	
	cout<<endl;
	
	cout<<"Also, we can initialize by default const in constructor decl"<<endl;
	Node_const(1);
	
	//Initialization assimetry: no difference for classes with no args ctor
	Node n1;
	Node n2{};		//no difference between n1 & n2
	
	//But big difference for simple types and aggregates
	int n;	//default-init; n = garbage   (just move of stack pointer)
	int m{};	//value-init, m = 0; (move stack pointer and put zero (default init value)
	int *p = new int[5]{};	//also value-init
	
	cout<<"size of empty class = " <<sizeof(Empty) <<endl;
	
	Copyable a;
	Copyable b(a), c(a);	//direct construction (via copy-ctor)
	Copyable d = a;	//copy-construction
	
	a = b;			//assignment	
	d = c = a = b;	//chain assignment (right-associative)
	
	//assignment is not a constructor, because both objects are exist already
	
	return 0;
}