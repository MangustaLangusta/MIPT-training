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
	*/
}

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