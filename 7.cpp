//lecture 7 c++

/*

OPERATOR() AND FUNCTORS

	it's impossible to inline function pointer
	
	for example, std::sort. 
	
	std::sort(myarr.begin(), myarr.end());
		-effective sort ( predicat < by default )
		
	bool gtf(int x, int y) { return x > y; }
	std::sort(myarr.begin(), myarr.end(), &gtf);
		- not so effective, because every time call by pointer
	
	how to solve it?
	
	we can make a class with implicit cast to function which takes two values, 
	compares properly and returns result as bool
	see implementation below
	
	we want easier
	
	easier - is to make this class a function-like object explicitly, via overloading 
	of operator().
	
	when we overload operator(), we create explicit function-like behaviour
	object of such a class, called as function, will act as function
	It calls FUNCTOR
	
	see another example below
	
	We can make such a functor in-place, it calles lambda-expression
	
	see example below
	
PIMPL
		
	PImpl - Pointer To Implementation
	
	class Ifacade {
		CImpl *impl_;
	public:
		Ifacade() : impl_(new CImpl) {}
		// methods
	};
	
	ABI is stable, implementation can be changed, but this example is RAII
	We want to use unique_ptr as controller of resource to avoid writing big 5
	but we cant:
	
	class MyClass;
	
	struct MySafeWrapper {
		unique_ptr<MyClass> c;	//this will not compile: unique_ptr requires deleter
		MySafeWrapper() : c(nullptr) {};
	}
	
	How to solve this? Add user deleter with overloaded operator()

	class MyClass;		//declaration
	
	struct MyClassDeleter {
		void operator()(MyClass*);		//defined somewhere else
	};

	struct MySafeWrapper {
		unique_ptr<MyClass, MyClassDeleter> c;
		MySafeWrapper() : c(nullptr) {};	//now OK
	};
	
	it works because unique_ptr takes any function-like deleter
	
POST-INCREMENT AND PRE-INCREMENT
	int x = 42, y, z;
	y = ++x;	//x = 43, y = 43    increments and returns new value_comp
	z = x++;	//x = 44, z = 43		increments and returns old value
	
	Quat<T>& Quat<T>::operator++();	//pre-increment
	Quat<T> Quat<T>::operator++(int);	//post-increment (returns old value)
							//since it's not possible to overload 
							//by return value, added false argument int (not being used)

	Usually, post-increment defined in terms of pre-increment:
	
	template<typename T> 
	struct Quat{
		T x_, y_, z_, w_;
		
		Quat<T>& operator++() { x_ += 1; return *this; }
		Quat<T> operator++(int) { 
			Quat<T> tmp{*this};
			++(*this);
			return tmp;
	};

	exactly same for pre-decrement and post-decrement
	
	
PROFESSIONAL STYLE OF CODING: PASSING THROUGH CONTAINER
	
	using itt = typename my_container<int>::iterator;
	
	for(itt it = cont.begin(), ite = cont.end(); it != ite; ++it) {
		//smthg
	}
	
	it was not called cont.end() each iteration (only called once),
	and pre-increment used (no tmp values*)
	* for int there is no difference, though)
	
CHAIN OPERATORS: += -= *= /=
AND BINARY OPERATOR +

	all of them are right-associative. 
	
	they return reference to self to arrange a chain:
	
	class Quat{
		Quat& operator+=(const Quat& rhs) { 
			//some ops
			return *this;
		}		
		
		//following will not work properly in all cases 
		//Quat t = x + 2; - OK
		//Quat t = 2 + x;	- FAIL, int dont have operator+, inmpilit cast will not work
		Quat operator+ const(const Quat& rhs) {	//note: another arg is "this"
			Quat tmp = *this;
			tmp += rhs;
			return tmp;
		}
	}
	
	To solve this assimmetry, define binary operators outside class:
	Quat operator+ (const Quat& lhs, const &Quat rhs){
		Quat tmp = lhs;
		tmp += rhs;
		return tmp;
	}
	
	Above will work, and even no need to be friend (re-used operator +=)
	
	Note: implicit casts do not work through template substitution
	
EQUALITY
	
	Good operator== have three following properties:
	- assert (a == a)    //reflection
	- assert ((a == b) == (b == a))	//simmetry
	- assert ( (a != b) || ((a == b) && (b == c)) == ( a == c) )		//transitivity
	above called "equality relations"
	
TWO- AND THREE- VALENT COMPARASIONS
SPACESHIP OPERATOR

	in C there are tri-valent commparations:
	strcmp(p, q);	//returns -1, 0, 1
	
	in C++ two-valent comparasions
	if (p > q)		//if (strcmp(p,q) == 1)
	if (p >=q)		//if (strcmp(p,q) != -1)
		
	starting from c++20 there is tri-valent comparasion operator:
	"spaceship operator"
	
	Note: if spaceship operator is overloaded, 
	no need to overload any other comparasion operators
	(only need to write operator==)
	
	type of operator<=>  : std::strong_ordering
	
	std::strong_ordering operator<=>(const MyClass& rhs) {	//"this" as first arg
		return x_ <=> rhs.x_;
	}
	
ORDERING TYPES:

	struct S {
		"ordering_type" operator<=>(const S& rhs) const
	}
	
	strong_ordering			-as for int    (if a !< b  and b !< a, a == b)  no inf, no NaN etc...
	weak_ordering				- between other two (different equal values possible, but no inf, NaN)
	partial_ordering		- as for double (+0, -0 are equal, but different) possible inf, NaN

DEFAULT SPACESHIP OPERATOR:
	
	struct MyInt{
		int x_;
		MyInt(int x = 0) : x_{x} {}
		auto operator<=>(const MyInt& rhs) = default;
	};
	
	default operator<=> is very useful: it makes logical comparasions,
	based on fields of class. type of ordering also based on fields
	Nothing non-trivial in comparasions.
	As a bonus, == and != also will be defined automatically, based on == and !=
	for types of fields
	
EXOTIC

	generally, we dont know if operator& overloaded or not,
	so to take address we can use std::addressof()
	
	IMPORTANT and INTERESTING
	
	pointer to method of class:
	struct MyClass {
		int DoIt(int a, int b) const;
	};
	
	using constif_t = int (MyClass::*)(int,int) const;	//signature of method
	
	how to call? 
	constif_t ptr = &MyClass::DoIt;
	
	MyClass c; 
	(c.*ptr)(1,1);	//first way
	
	MyClass *pc = &c;
	(pc->*ptr)(2,2);	//second way			this operator->* can be overloaded
	
	OPERATOR,
	
	meaning: do all expressions, return result of last one;
	result = foo(), bar();		//will return bar();
	
	another example:
	buz(1, (2, 3), 4);		//here in brackets operator,.  will call buz(1,3,4)
	
	Note: operator, guarantee order of execution
	till it's overloaded
	
	do not overload || and && because they lose lazy behaviour
	do not overload + unary, because positive hack lost
	
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

using std::cout;
using std::endl;
using std::vector;

struct MyStrangePredicat{
	static bool gtf(int x, int y) { return x > y; }	//this is predicat function
	using gtfptr_t = bool (*) (int, int);	//for clear code
	operator gtfptr_t() const {	return gtf; 	}	//this is implicit cast to 
									//proper predicat function
};

struct MyFunctionLikeClass{
	bool operator() (int x, int y) { return x < y; }
};


struct VoidDeleterInt{
	void operator() (void* vptr) { 
		int *iptr = static_cast<int*>(vptr);
		cout<<"deleted using deleter\n";
		delete iptr;
	}
};
	

template<typename T>
T& operator->*(std::pair<T,T> &l, bool r) {
	return r ? l.first : l.second;
}
	

int main() {
	
	
	std::unique_ptr<void, VoidDeleterInt> p(new int{42});
	
	vector<int> myarr{1,3,2,5,4,6,9,8,7};
	
	//strange and not very obvious construction:
	std::sort(myarr.begin(), myarr.end(), MyStrangePredicat{});
	/* So, we created an object MyStrangePredicat, which is implicitly casted to
		predicat function which is static method of class MyStrangePredicat
	*/
	for(auto it : myarr)
		cout<<it<<" ";
	cout<<"\n";
	
	//explicit function-like class with overloaded operator()
	MyFunctionLikeClass my_predicat;
	std::sort(myarr.begin(), myarr.end(), my_predicat/* or MyFunctionLikeClass{}*/);
	for(auto it : myarr)
		cout<<it<<" ";
	cout<<"\n";
	
	//using lambda-expression
	auto my_lambda_predicat = [](auto x, auto y) { return x > y; };
	std::sort(myarr.begin(), myarr.end(), my_lambda_predicat);
	//or std::sort(myarr.begin(), myarr.end(),[](auto x, auto y) { return x > y; };);
	for(auto it : myarr)
		cout<<it<<" ";
	cout<<"\n";
	
	//spaceship operator (tri-state comparasions)
	int a = 4, b = 4, c = 5;
	cout << std::boolalpha;
	cout<<(a == b)<<endl;
	cout<<((a <=> c) < 0)<<endl;
	
	//example of operator->* overload for getting member of pair
	//see declaration before main()
	std::pair<int,int> y{42,34};
	cout << (y ->* true) << " " << (y ->* false) <<endl;
	return 0;
}