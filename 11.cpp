//lecture 11 C++

/*
	EXEPTIONS
	---------
	
	C-STYLE ERROR HANDLING:
	
	-define set of integer error codes
		enum error_t { E_OK = 0, E_NO_MEM, E_UNEXPECTED };
	
	-one of three:
		- return error code
		- use thread-local facility (i.e. GetLastError)
		- return error code in args list
	
	problem is that we return error code but we want return useful value
	and if GetLastError or arg, we forget to check errors (or too lazy)
	
	PROBLEM IN C++
	
	what if need to return error code from constructor?
	
	of course, we can use flags (i.e. BAD_BIT in iostream)
	actually, it's not convenient
	and, what will we do in destructor 
	if we have information that "something wrong"?
	
	What to do with operators outside classes? (operator+, i.e.)
	check for errno, but not convenient
	
	EXEPTIONS
	
	we have several types of "exeptions":
	- hardware exeptions (i.e. undefined instruction exeption)
	- OS exeptions (i.e. data page fault)
	- C++ exeptions
	
	C++ exeptions is our interest
	
	NOT exeptions:
		Errors: 
			runtime errors (i.e. segfault) program state CANNOT be restored; 
			function contract errors(i.e. assertion fails due to wrong args...)
		
	Exeptions:
		Program state should be restorable
		Exeption situation cannot be handled at level of occurency
			(sort program not obliged to know what to do if not enough mem
			for tmp buffer)
			
	How Throw works:
		-Creates exeption object
		-Begins stack unwinding
			
		examples:
			throw 1;
			throw new int(1);
			throw MyClass(1,1);
			
	Exeptions need to be catched
	
	Exeptions catching:
		try{
			//some code which may throw exeption
		}
		catch ( <exeption type> ){
			//exeption handling
		}
		
	exeption may pass several stack frames, untill it catched
		
RULES:
	- catch by exact type ( no conversion int -> long )
	- or exact type reference
	- or exact type pointer
	- or ref/ptr to base class (also by value, but will be base slice)
		(only one exeption to exact type: can be base class)
		
	i.e. try { throw 1; } catch {long l} {} // not caught (int vs long)

	We can make several catch bloks in raw to handle various exeptions:

	try{
		try{
			throw <expression>
		}
		catch( <expression> ){
			throw;	//this is calleed "rethrow" - same exeption is thrown further
		}
	}
	catch( <expression> ){
		//here previously rethrown exeption will be caught
	}

	see example below in main()
	
	Note: if we throw, but never catch (type in catch() doe not suit for exeption type)
	exeption will not unwind stack (it's like quantum uncertainty)

CLASSES  OF EXEPTIONS
	
	for more flexibility we use classes of exeptions
	
	class MathErr {}
	class Oferflow : public MathErr {}
	
	try { }
	catch (Overflow& o){}
	catch (MathErr& e){}
	
	Note: use pointers or references and catch from particular to general
	
STANDART METHODS OF HANDLING EXEPTIONS IN C++
	
	stndard header <stdexept>
	
	It's good practice to use standart exeptions or inherit from them
	
	base class: std::exeption
	
	derived classes (two groups):
			//first group (we usually do not inherit from this)
	- std::bad_alloc, std::bad_cast, 
		std::bad_exeption, std::bad_function_call,
		std::bad_typeid, std::bad_weak_ptr
			//second group (these are we use as base classes for ours)
	-	std::runtime_error, std::logic_error
	
	std::runtime_error and std::logic_error have their own derived classes...
	
	so, we catch exeptions by (std::exeption&)
	
	std::exeption have method 
		virtual const char* what()
	which return explanation in form of null-terminated string
	(it uses const char* because it should not throw exeptions by itself)
	
	
CATCH ALL EXCEPTIONS: catch(...) {}

	-it catches ALL exceptions (even not ours!)
	-it do not posess exception object
	
	Function is exception-neutral if it does not catch allien exceptions
	
	Good written function in properly designed code at least exception-neutral
	
FUNCTION LEVEL TRY-BLOCK

	we can wrap whole function in try-block (even constructors)
	if so, we can catch exceptions from initializer lists
	
	see example below

EXCEPTIONS GUARANTEES

	Basic exception guarantee:
		If exception thrown, no memory leaks,
		objects left in consistent, but not predictable state
	Strong exception guarantee:
		If exception thrown, everything restored as it was before
		(commit/rollback)
	No exception guarantee:
		No exceptions can be thrown
		
General note:
	If exceptions may be thrown, need to think carefully and predict all possible places wherex
	exceptions may be trown
	
"CALB LINE"
	
	When writing function with strong exception guarantee, draw imaginary line between 
	exception-prone zone and zone where object is changed (but no exceptions are thrown)
	
	example:
	void swap(const MyVector& rhs) noexcept;
	Myvector& operator= (const MyVector& rhs) {
		MyVector tmp(rhs);	//still may throw exceptions, but not change object
		// -------- 		Calb's Line
		swap(tmp);	//change object's state, but not throw exceptions
		return *this;
	}
	
EXCEPTIONS IN DESTRUCTORS

	Exceptions should not leave destructor
	All destructors implicitly marked noexcept
	
NOEXCEPT

	if function annotated noexcept, compiler assumes there will be no exceptions there
	and makes optimisations
	
	if func is noexcept-annotated, it should not throw exceptions
	
*/

#include <iostream>
#include <memory>
#include <stdexcept>

using std::cout;
using std::endl;

//c-style:
enum error_t { E_OK = 0, E_NO_MEM, E_UNEXPECTED };

//return error code
error_t open_file(const char* name, FILE **handle);

//use GetLastError
FILE* open_file(const char* name);

//return error_t in args list
FILE* open_file(const char* name, error_t* errcode);


class MyClass{
	int i_;
public:
	MyClass(int i) : i_(i) { cout<<"MyClass "<<i_<<" ctor"<<endl; }
	~MyClass() { cout<<"MyClass "<<i_<<" dtor"<<endl; }
};

void foo1();
void foo2();
void foo3();

void foo1(){
	cout<<"start of foo1. call foo2"<<endl;
	MyClass* my0_ptr = new MyClass{0};					
	//exeptions do not affect heap usage, only stack
	MyClass my1{1};
	foo2();
	delete my0_ptr;	//will never be called
	cout<<"end of foo1"<<endl;
}

void foo2(){
	cout<<"start of foo2. call foo3"<<endl;
	MyClass my2{2};
	auto my_unique = std::make_unique<MyClass>(42);
	foo3();
	cout<<"end of foo2"<<endl;
}

void foo3(){
	cout<<"start of foo3. throwing exeption"<<endl;
	MyClass my3{3};
	throw(1);
	MyClass my4{4};
	cout<<"end of foo3"<<endl;
}

// Classes of exeptions

struct BaseExep{
	static int general_id;
	int local_id;
	BaseExep() : local_id(general_id++) { cout<<"Base ctor id = "<<local_id<<endl; }
	BaseExep(BaseExep&) : local_id(general_id++) { cout<<"Base copy ctor id = "<<local_id<<endl; }
	virtual ~BaseExep() { cout<<"Base dtor id = "<<local_id<<endl; }
};
int BaseExep::general_id = 0;

struct DerivedExep : public BaseExep{
	const int der_id;
	DerivedExep() : BaseExep(), der_id(local_id) { cout<<"Derived ctor id = "<<der_id<<endl; }
	DerivedExep(DerivedExep&) : der_id(local_id) { cout<<"Derived copy ctor id = "<<der_id<<endl; }
	~DerivedExep() { cout<<"Derived dtor id = "<<der_id<<endl; }
};


struct my_exc1 : virtual std::exception{
	char const* what() const noexcept override { return "exc1"; }
};

struct my_exc2 : virtual std::exception{
	char const* what() const noexcept override { return "exc2"; }
};

struct your_exc3 : my_exc1, my_exc2 {
	char const* what() const noexcept override { return "exc3"; }
};


//whole function try-blocks
template<typename T>
class FuncLevelTryBlockClass {
	T x_;
public:
	FuncLevelTryBlockClass(T& x) try : x_(x) {
		//some actions
	}
	catch(std::exception& e){
		cout<<e.what()<<endl;
	}
};

int main(){
	
	try{
		foo1();
	}
	catch(int i){
		cout<<"catched "<<i<<endl;
	}
	try{
		try{
			throw MyClass{1};
		}
		catch (MyClass & m) {
			cout<<"first handler"<<endl;
			throw;	///rethrow
		}
	}
	catch (MyClass & m) {
		cout<<"catched!"<<endl;
	}
	
#ifdef CHECK
	{
	//check
		cout<<endl;
		BaseExep b1;
		BaseExep b2(b1);
		DerivedExep d1;
		DerivedExep d2(d1);
	}
	cout<<endl;
#endif //CHECK

	//classes of exeptions
	try{
		throw DerivedExep();
	}
#ifdef REF
	catch(BaseExep& b){
		//correct way, using reference
		//no copy-constructors, no base slice
	}
#else	
	catch(BaseExep b){					//base slice!
		//order of ctors and dtors:
		//Base (in try block)
		//Derived (in try block)
		//Base copy (catch block)
		//~Base (catch block)
		//~Derived (try block)
		//~Base (try block)
	}
#endif //REF

	try{
		throw your_exc3();
	}
	catch (std::exception const &e){		//if multiple inheriatance, will not work!
		cout<<e.what()<<endl;
	}
	catch(...){
		cout<<"nothing!"<<endl;	
	}
	
	return 0;
}