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
		
	
	
	
	
	
	
	
*/

#include <iostream>
#include <memory>

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

int main(){
	
	try{
		foo1();
	}
	catch(int i){
		cout<<"catched "<<i<<endl;
	}
	
	return 0;
}