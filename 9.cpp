//lecture 9 C++ 

/*
	Multiple inheritance
	We try to avoid if circumstances admit.
	But sometimes we use it.
	- Multiple inheritance of interfaces (relationship "is-a")
	
	Inheritance from realisation (not interfaces) are prohibited 
	in many languages because it makes certain problems
	
	Multiple inheritance: risk of rhumb-like scheme
					File
			/					\
	InputFile		OutputFile
			\					/
				IOFile (it is a File 2 times)
				
	How to access same fields in InputFile and OutputFile?
	
	IOFile f{11};
	int x = f.a;		//if a is a field of File, it's error
	int y = f.InputFile::a;		//ok, but it's pain
	
	We want to have only one copy of base class, 
	no matter how many ways it came to derived
	
	Such base classes called virtual
	
	struct File { ... };
	struct InputFile : virtual public File { ... };
	struct OutputFile: virtual public File { ... };
	struct IOFile: public InputFile, public OutputFile { ... };
	
	IOFile f{11};
	int x = f.a;		//OK
	int y = f.InputFile::a;		//also OK
	
	Order of construction:
		- First all virtual base classes
		- Secondary all normal base classes
		
	What if base class virtual not all ways?
		- One instance of base class from all virual ways and instances from 
		each normal way
		
	What if base class is virtual, but only one in derived? 
	(Virual, but not rhumb-like inheritance)
		- Like nirmal behaviour, but worse (one more layer of references in vtable)
		
	---
		
	Virtual base class must appear in initializer list of lowest derived class
	
	struct InputFile : virtual public File {
		InputFile() : File(smth1) {}	//this File() will NOT be called for IOFile
	};
	
	struct OutputFile : virual public File {
		OutputFile() : File(smth2) {} //also will NOT be called for IOFile
	};
	
	struct IOFile : public InputFile, public OutputFile {
		IOFile() : File(smth3), InputFile(), OutputFile() {}
	};
	
	IOFile f;		//will call File(smth3);
		
	---
	
	Need to keep in mind that virtual inheritance creates additional delay due to 
	multi-layered calls via pointers of vtable functions
	
	In some cases it's acceptable (file works, screen printing, etc)
	
	---
	
	Problem of conversions
	
	struct Base1;
	struct Base2;
	struct Derived: public Base1, public Base2;
	
	Derived* pd = new Derived{};
	Base1* pb1 = static_cast<Base1*>(pd);		//upcast. will it work?? Yes!
		//it's magic of static_cast
		//it will work other side too:
	pd = static_cast<Derived*>(pb);		//downcast. works
	
	when multiple inheritance, use static_cast
	
	! When multiple virtual inheritance, downcast
	(virtual Base->Derived) does not work

	
	----
	
	RTTI - RunTime Type Information
	
	- For resolving some issues (like "what dynamic type do I have?" and 
		for easy passing up-down through classes hierarchy, C++ program 
		keeps invisible to programmer data structures during runtime
	-	It's strange solution, since it contradicts to language ideology
	- There are two such doubtful solutions: RTTI and exeptions
	- Base of RTTI is typeinfo
	
	If a program have classes with vtables, objects also keep dynamic data
	structures (trees) with info about its dynamic type and parents / siblings
	
	So, every object knows it's dynamic type and knows, where you can travel across 
	siblings' types
	
TYPEID

	operator typeid returns object std::type_info, which we can compare and print to screen
	
	see examples in main() below
	
DYNAMIC CAST
	
	allows travelling across inheritance graph
	
	in case of trivial conversions, acts as simple static_cast (where possible)
	
	it works with pointers and with references
	
	in case it cannot perform cast for pointer, returns nullptr
	in case it fails for reference, it throws exeption
	
	
GRAMMAR

	-Alphabet is set of symbols: {a, b, c}
	-String is sequence of symbols: w = {a,a,b,c} or w = aabc
	-Empty string: _A_ (lambda)
	-Concatenation of strings: w = abc, z = ca, wz = abcca, zw = caabc
	-Exponent w^3 = www, w^0 = _A_
	
	Alphabet and simple operations above - free group
	
	Language above alphabet is set of its strings
	
	Language L_empty: empty set of strings
	Language L_free: all possible strings of alphabet 
	L1 = {a^m b^n} : a, ab, abb, aab, aabb...
	L2 = {a, cab, caaba}
	L3 = {a^n b^n} : ab, aabb, aaabbb...
	...
	
	Tasks for formal languages:
	- Belonging: having language L and string w, find out if w belongs L
	- Creation: having L, create all possible strings consequently
	- Equivalent: L1 and L2, check, if they are same (do not have strings belongs to one and not belongs to other)
	- Negative: have L1, define L2 so, it contains all string which not belongs to L1
	
REGULAR EXPRESSIONS
	
	- any alphabetical symbol is a language from this symbol: a is {a}
	- concatenation LxLy: {wz, where w belongs Lx and z belongs Ly}
	- disjunction Lx + Ly: {w, where w belongs Lx or w belongs Ly}
	- closure Lx* : {_A_, Lx, LxLx, LxLxLx, ...}
	additionally:
	- a? : a + _A_  (zero or one repetition)
	- (a+) : aa*  (one or more repetitions)
	- [[:digit:]] ( 0|1|2|3|4|5|6|7|8|9 )
	
	example:
	any whole number: 
		(-)?([[:digit:]])+
	floating point number:
		(0) + ( (-)?([[:digit:]]+)(.)[[:digit:]]+ )
	
REGULAR EXPRESSIONS IN C++

	#include<regex) - in C++
	See good examples below
	
	regex can be based on deterministic finite state machines
	DFSM - set of states and function of transition between states
	only one state is starting
	one or more states are finish (accepting)
	
	for some expressions it's not trivial to build DFSM 
	in this cases non-deterministic finite state machines are used (NFSM)
	
	it can be more than one way from state by single input
	(	i.e., from state X symbol 'b' may lead to state Y or state Z)
	
	so, it's a tree of possible changes os states
	if at least one way leads to accepting state, NFSM matches
	
	Easier to build from regex, but hard to simulate (memory usage)
	We want to convert NFSM into DFSM
	Alghorithm Rabin-Scott are used for this.
	
	std::regex is doing all this for us.
	
	One thing: it's pretty slow.
	We want to generate in Compile Time
	
*/


#include <iostream>
#include <string>
#include <regex>

using std::cout;
using std::endl;
using std::string;

//example for multiple inheritance and RTTI
struct File{
	int a;
	File(int x) : a(x) { cout<<"File ctor\n"; }
	~File() { cout<<"File dtor\n"; }
	
	virtual void who() const { cout<<"File. addr = "<<this<<endl; }
};

struct InputFile : virtual public File {
	int b;
	InputFile(int x) : File(x), b(x * 2) { cout<<"InputFile ctor\n"; }
	~InputFile() { cout<<"InputFile dtor\n"; }
	
	void who() const override { cout<<"InputFile. addr = "<<this<<endl; }
};

struct OutputFile : virtual public File {
	int c;
	OutputFile(int x) : File(x), c(x * 3) { cout<<"OutputFile ctor\n"; }
	~OutputFile() { cout<<"OutputFile dtor\n"; }
	
	void who() const override { cout<<"InputFile. addr = "<<this<<endl; }
};

struct IOFile : public InputFile, public OutputFile {
	int d;
	IOFile(int x) : File(x), InputFile(x), OutputFile(x), d(x * 4) { cout<<"IOFile ctor\n"; }
	~IOFile() { cout<<"IOFile dtor\n"; }
	
	void who() const override { cout<<"IOFile. addr = "<<this<<endl; }
};


void foo(File f) {	//for Base slice example
	cout<<"foo"<<endl;
	f.who();
	cout<<"end foo"<<endl;
};


int main() {
	
	IOFile* iofp = new IOFile{1};
	//upcasts - ok
	File* fp = static_cast<File*>(iofp);
	InputFile* ifp = static_cast<InputFile*>(iofp);
	OutputFile* ofp = static_cast<OutputFile*>(iofp);


	//below downcasts
	IOFile* iofp_down;
	
#if 0
	iofp_down = static_cast<IOFile*>(fp);	//compilation error: base class is virual parent
#endif

	iofp_down = dynamic_cast<IOFile*>(fp);		//works fine.
	iofp_down->who();
	
	iofp_down = static_cast<IOFile*>(ofp);		//also works OK because OutputFile is not virtual parent

	
	//siblings conversions
	InputFile* sibling_ifp;

#if 0
	sibling_ifp = static_cast<InputFile*>(ofp);		//compilation error: cannot convert 
																								//(neither parent, nor child)
#endif
	
	sibling_ifp = dynamic_cast<InputFile*>(ofp);	//works fine: dynamic_cast allow travel across all 
																								//inheritance tree


#ifdef BASE_SLICE
	foo(*iofp);	//Base slice example
#endif
	
	const int ci = 6;		
	int i = 6;
	
	cout<<"typeid of *iofp: "<<typeid(*iofp).name()<<endl;	//dynamic type
	cout<<"typeid of iofp: "<<typeid(iofp).name()<<endl;		//static type (compile-time)
	cout<<"typeid of it: "<<typeid(i).name()<<endl;				//static type (compile-time)
	cout<<"typeid of nullptr: "<<typeid(nullptr).name()<<endl;
	//typeid not affected by cv-qualifiers
	(typeid(ci) == typeid(i)) ? cout<<"ci == i"<<endl : cout<<"ci != i"<<endl;
	
	delete iofp;
	
	cout<<"\nRegular expressions\n"<<endl;
	
	// regex for float
	// 0|-?(([1-9][[:digit:]]*)|0)\.[[:digit:]]*[1-9]
	std::regex reg("");
	std::cmatch m;
	string s{""};
	cout<<std::boolalpha;
	cout<<"for enter regex type 'reg'"<<endl;
	cout<<"then type expressions to check match"<<endl;
	cout<<"for exit type 'exit'"<<endl;
	while(true){
		std::cin>>s;
		if(s == "exit")
			break;
		if(s == "reg"){
			cout<<"type regex"<<endl;
			std::cin>>s;
			reg = s;
			cout<<"regex created"<<endl;
			continue;
		}
		bool res = std::regex_match(s.c_str(), m, reg);
		cout<<"result: "<<res<<endl;
	}
	cout<<"Successfully exited"<<endl<<endl;
	return 0;
}