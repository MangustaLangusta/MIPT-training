// lecture 3: names and entities

/*
	module is a context, class is a context, function is a context
	
	malloc & free does not know about constructors and destructors
	
	new & delete vs new[] & delete[]
	
	do not mix them (and new/delete and malloc/free)
	
	scope - all places in program from where we can address to specific name
	
	lifetime - all time when state of object is valid. Starts AFTER finish of initialisation
	
	i.e.:   int a = a;			// UB! a declared, but lifetime not yet started
	
	pointer which is refers to object with expired lifetime is called dangling
	
	void foo() {
		int b = 6;
		int *pc;
		if(b > 5){
			int c = b + 5;
			pc = &c;
		}	//scope of c end; c lifetime end. pc dangles
		
		b += *pc; //this is a late parrot
	}	//b scope end; b lifetime end
	
	dangling reference: 
	for exmple, reference to deleted memory:
	
	int *p = new int[5];
	int &x = p[3];
	delete [] p;
	x += 1;		//dangling reference!!!
	
	also: reference to tmp value:
	
	int& foo() {
		int x = 42;
		return x;
	}
	int x = foo();	//also dangling reference!
	(correct value still can be on stack, though, so hard to detect)
	
	const lvalue references prolongate lifetime of tmp objects:
	
	const int &lx = 34;
	int x = lx;		//ok
	
	int foo();
	const int &ly = 42 + foo();
	int y = ly;			//ok
	
	const lvalue ref prolongates lifetime of tmp obj on stack
	
	tmp object lives till end of full expression:
	struct S {
		int x;
		const int& y;
	};
	
	S x{1, 2};	// ok, lifetime extended (x is not tmp object, it's on stack frame)
	S *p = new S{1, 2};	//ref is dangling because tmp object ends when end of expression;
	
	It's complicated. Better not to use references as fields in class at all
	
	non-const lvalue references do not create tmp objects and refuse to refer to literals (rvalues)
	
	int foo(int &x);
	foo(1);			//compilation error
	
	int &x = 1;	//also compile error
	
	here is everything good:
	int foo(const int &t) {
		return t;
	}
	
	it's safe to take refs as args. It's not safe to return them (sometimes)
	
	decaying - when object behaves like another, simplier object
	i.e., array decays to pointer to first element when used as rvalue:
	void foo(int*);
	int arr[5];
	int *t = arr + 3; //ok
	foo(arr);	//ok
	arr = t;	//fail
	
	rvalue / lvalue are properties of expressions
	
	"l" stands for "location" (expression which have location in memory)
	
	
	cdecl again...
	int *x[20];		//array of pointers ( -> <- ^ )
	int (*y)[20];	//pointer to array of int[20]
		
		
	char *(*(&c)[10])(int *&p);	//c is... 
		//reference to array of 10 pointers to function 
		//taking reference to pointer to int 
		//and returning pointer to char
	
	void (*bar(int x, void (*func)(int&))) (int&);
		we can use typedefs (c-style)
	typedef void (*ptr_to_fref) (&int);
	ptr_to_fref bar(int x, ptr_to_fref func);
	
	c++ style: 
	using prt_to_fref = void (*) (int&);
	ptr_to_fref bar(int x, ptr_to_fref func);
	
	template<typename T>
	using ptr_to_fref = void (*) (T&);
	
	C gives guarantees regarding names: no name-mangling
	C++ do not gives such guarantees. That's why it's possible to overload functions in c++
	
	in C all assembly labels are same
	in C++ they can be distorted by type instance
	
	if we want have same guarantees in c++, we use
	extern "C" double sqrt(double);			//for example. no overload possible
	
	overload rules:
	
	- exact match (int->int)
	- exact match with template (int->T)
	- standart conversion (int->char, float->unsigned short, etc)
	- user conversions
	- variable arguments
	- incorrect linked refs (literal -> &int, ...)
	
	
	namespaces
	it's bad habit to spoil global namespace
	
	namespaces not affect macros
	
	better not to use "using namespace std"
	
	common rule:
		-if there is no object of some type in program, 
		such type should not exist at all
		
	
	Good practice rules for namespaces:
		-do not spoil global namespace
		-do not write "using namespace ..." in headers
		-use anonymous namespaces instead of static functions
		-do not use anonymous namespaces in headers
		
	
*/

#include <iostream>

using std::cout;
using std::endl;

//global namespace:
int x = 5;

int foo(){
	return ::x;	//returns x grom global namespace
}


//namespaces can envelope into each other
//classes and structs also creates namespaces
namespace MyNamespace{
	struct MyStruct{
		struct MyEnvelopedStruct{
			
		}
	}
}	//MyNamespace

MyNamespace::MyStruct::MyEnvelopedStruct n;

using std::vector;		//enter std::vector in current namespace as vector
using namespace std;	//namespace std opened in current namespace. use with caution

//anonymous namespaces
namespace {
	
}	//end of anonymous namespace

//it's good way to replace static functions
//it creates namespace with unique (and hidden) name and opens it in current namespace
//we can be sure that these functions can be used only within this module



int main(){
	int ai[20] = {0};
	int *api[20] = {nullptr};
	int (*pai)[20] = &ai;
	int (&rai)[20] = ai;
	cout << api << " + 1 = " << api + 1 << endl;
	cout << pai << " + 1 = " << pai + 1 <<endl;
	
	rai[2] = 40;
	(*pai)[2] += 2;
	
	cout<< ai[2] << endl;
	
	
	return 0;
}