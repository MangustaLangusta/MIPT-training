//lecture 6 C++

/*

What is any RAII class invariant?
	-it owns resource (only this class can create or destroy resource)
	
beside class, access to its resources have
	-its static methods
	-friend methods

Static function - function which works without implicit _this_

in RAII class should not be friends (they break its invariant)


NOEXCEPT 
	if you are sure that method is making only primitive operations on primitive types,
	you can annotate it as "noexcept"
	
	this annotation guarantee that it will be no exceptions thrown from this method
	

UNIQUE_PTR CONCEPT
	Unique_ptr - "ideal" RAII class
	Main idea - use move semantics for passing ownership
	Copy is prohibited
	
	How it deletes? If it doesn't know, what is inside
	
TYPES CONVERSIONS
	c-style casts looks same for various kinds of conversions, i.e.:
	- int -> double   //OK
	- const int* -> int*    //Doubtful, but OK
	- int* -> long 			//wat?
	
	all of above looks same:
	x = (T) y;
	
	We do not use c-style casts in C++ 
	-static_cast
	-const_cast
	-reinterpret_cast
	
	static_cast   - just normal safe casts
		int x;
		double y = 1.0;
		x = static_cast<int>(y);
		
	const_cast    - removing const or volatile
		const int* p = &x;
		int *q = const_cast<int *>(p);
		
	reinterpret_cast    - courage and stupidity
		long long uq = reinterpret_cast<long long>(q);
		
	! strict alliasing violation - if two pointers of different types ponts to 
	  same location, it's UB (but char* and similar types are not subject to this)
		so, this is complicated and too much deep into specs
		
	std::bit_cast used for this purpose:
		int m = std::bit_cast<int>(p);
		
		(it makes something like this: std::memcpy(&m, &p, sizeof(int)) )
	
	static_cast is explicit conversion. if we use explicit constructors, 
	we shall use static_cast if want to use these ctors:
	
	struct T {};
	struct S { explicit S(T) {} };
	
	void foo(S s) {}
	
	foo(T);		//fail
	foo(static_cast<S>(T)); //OK
	
	T x; S y = static_cast<S>(x);		//OK
		
		//see 6_1.cpp for this example to compile
		
C-STYLE CASTS
	int a = (int) y;	//c-style
	int b = int(y);		//same as previous, functional-style c-style cast
	
	int c = int{y};		//ctor, c++ style
	int d = S(x, y);	//ctor, if 2 or more args, never c-style cast
	
	better to use static_cast for explicit conversions
	
INTEGRAL PROMOTIONS
	
	all < int promotes to int
	mix int & long promotes to long
	mix unsigned and int   all unsigned becomes signed
	mix anything with floating type becomes floating type
	
implicit casts on initialisation:
	widetype x; narrowtype y;
	
	widetype z = y;		//always OK
	narrowtype v = x;		//ok if x fits v, othervise may be surprising
	
	void foo(double);
	foo(5);		//int promoted to double
	
UNARY PLUS (POSITIVE HACK)
	not matters for most simple types (2 == +2)
	even if not overloaded gives a legal way to cast to simple type:
	
	struct Foo { operator long() { return 42; }};
	
	void foo(int x);
	void foo(Foo x);
	
	Foo f;
	foo(f);	//call foo(Foo)
	foo(+f);	//call foo(int)
	
UNARY MINUS
	see example with class Quat below
	
	generally: we can overload operator-  (and other operators) inside class,
	or outside class.
		-  -a means "a.operator-()"		-inside class
		-  -a means "operator-(a)"		-outside class
		
	if both variants defined, inside class wins
	
*/

#include <iostream>

//just good examples of constructors of Matrix
template<typename T>
class Matrix(){
public:
	Matrix(int cols, int rows, T val = T{});	//constructor for matrix filled with value 
	template<typename It)
	Matrix(int cols, int rows, It start, It fin);		//create matrix from sequence using iterators
	
	//some other ways to make objects
	static Matrix eye(int m, int n);		//matrix filled with "1". Used static function
	
	//Big five (destructor, copy-ctor, copy-assign, move-ctor, move-assign)
	Matrix(const Matrix& rhs);	//copy-ctor
	Matrix& operator= (const Matrix& rhs);	//copy-assign
	
	Matrix(Matrix&& rhs) noexcept;			//move-ctor;
	Matrix& operator= (Matrix&& rhs) noexcept;			//move-assign
	
	~Matrix();		//dtor
	
	//Selectors - safe const methods giving some info
	//basic selectors
	int ncols() const;
	int mrows() const;
	//aggregate selectors
	T trace() const;	
	bool equal(const Matrix& other) const;
	bool less(const Mathix& other) const;
	void dump(std::ostream& os) const;
	
	Matrix& negate() &;		//dont forget to use ref-annotation if return reference to inner state
	Matrix& transpose() &;
	
	/* Matrix operator[](int) const; - This is my original variant of what to return */
	//But we will make a new proxy-class with [] overloaded also
	
	
	//here we will use proxy class ProxyRow to use with []
	private:
		struct ProxyRow {
			T* row;
			const T* operator[](int n) const { return row[n]; }
			T* operator[](int n) { return row[n]; }
		};
	public:
	
	ProxyRow operator[](int n);
};

//example of indexators
class MyVector {
	std::vector<int> v_;
public:
	int& operator[](int x) { return v_[x]; }
	const int operator[] (int x) const {return v_[x]; }
		//dont forget: only const qualified methods can be called for c-qualified objects
};


//use of unique_ptr
int foo (int x, double y) {
	auto res = std::make_unique<MyRes>(x, y);		//take ownership
	//  ....
	if (some_condition) {
		bar(std::move( res ));	//passing resource to another scope
		return 1;
	}
	//...
	return 0;		//frees in dtor
}

int buz (int x, double y) {
	const std::unique_ptr<MyRes> p{new MyRes(x,y)};	
		//! const unique_ptr even cannot be moved
	return 0;
}

template<typename T>
struct Quat {
	T x, y, z, w;
	
	//unary minus (it wins if in-class and out-class are defined):
	Quat operator-() const { return Quat{-x, -y, -z, -w}; }
};

//	another variant of unary minus, outside class
template<typename T> 
Quat<T> operator-(Quat<T> arg) {
	return Quat<T>{-arg.x, -arg.y, -arg.z, -arg.w};
}


int main(){
	
	Quat q{1, 2, 3, 4};
	Quat p = -q;		//unary minus: {-1, -2, -3, -4}
	
	return 0;
}