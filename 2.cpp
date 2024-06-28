//2 references and pointers

/*
value type: range of values of object (i.e. -7..8, 0..255 etc)
object tyoe: summary of operations on objects (+, -, ...)

I can make my own operations on my own types in c++

Static typisation: type is connected to name forever
If name can have different types in different places in program, it's dynamically typisation language

RAM model of memory:

 11001000001110011001111
    a  |      b  |
		
Objects are not separated from other objects
Address - distance from beginning of memory to object
&a = 3
&b = 13

type of address is a pointer

size of a pointer - at least such that it can contain all possible addresses of architecture
(example above: sizeof(void*) = 5 (because 22 bits is a full address space)

limits.h  ->  constant CHAR_BIT is a size of char in bits
char is a minimum addresable cell
cannot address in middle of char

sizeof(int) == 4 means that it have size of 4 chars.
Also it means that we can put pointer inside of it in four different ways

null pointers:
0, NULL, nullptr   - all are different things
0 (int) , NULL (void*) - for C
nullptr (nullptr_t) - for c++

we use nullptr in c++ bacause 0 wins overloads

in C++ there are lvalue references and rvalue references


	const char *s1;
	char const *s2;
	char * const s3;
	char const * const s4;


int foo(Heavy fst, Heavy snd);		//bad - Heavy objects passing by value
int foo(const Heavy *fst, const Heavy *snd);		//better - pointers, but will have to unreference pointers fst->xalloc
int foo(const Heavy &fst, const Heavy &snd); 		//by reference. pointers are used implicitly. fst.x


also, good idea to use references as aliases in big objects:
int &internal = obj.faraway[3].guts.internal;

advantage here: ref do not consume memory.
also, reference cannot be re-assigned (pointers can be):
	int *internal = &obj.faraway[3].guts.internal;
	internal += 5;   //not makes sense, but possible
	
	
out-argument reference vs out-argument pointer

There is an opinion that better not to use out-args at all

Incapsulation - hiding of object's internal state

No need to put everything always in private section. Only things that really need to be hidden.
For example, why to put in private fields of proxy-class (which is used only by other classes)?

Invariants are conditions which need to be met within object's lifetime, otherwise it becomes inconsistent.

Fields responsible for invariants, for example, better to put in private part

Structs vs classes: two differences:
	- struct is public by default, class is private by default
	- derivatives from structs are public, derivatives from classes are private
	
We still can cast object to char* and break all invariants... but this is very bad practice

Incapsulation is a property of class, not only object of class
(private fields of any object of my class can be accessed by any other object of same class):

template<typename T>
class list_t{
...
	void concat_wuth( list_t<T> other) {
		for(auto cur = other.top_;
				cur != other.back_;
				cur = cur->next_)
			push(cur->data_);
	}

...But different template args creates different types:

template <typename T>
class list_t{
...
	void concat_with( list_t<U> other) {
		...	//will not work
	}
*/



#include <iostream>
#include <cassert>

using std::cout;
using std::endl;


int foo() { return 42; }

int main() {
	cout<<"CHAR_BIT = " << CHAR_BIT <<endl;
	cout<<"size of pointer = " << sizeof(void*) << endl;
	
	
	int x = 4;
	int &y = x; 		//now y is lvalue reference of x. Both x and y refer to same object
	/* note that y now cannot change object of its reference - it's always refers to x;	*/
	
	assert(&y == &x);		//address of a reference is same as address of referenced object
	
	int *xptr = &y;			//ok, pointer to object x (&y is same as &x)
	
	//int &*xrefptr = &y;		//fail! can not be because references do not have address by themselves so, pointer to reference is impossible
	int *&xptrref = xptr;		//ok, reference to pointer
	
	const char *s1;
	char const *s2;			//s1 and s2 are same- pointer to const char
	//char * const s3;					//fail! const object should be initialized (const pointer)
	//char const * const s4;		//same as previous
	
	/*
	char &r1 = r;							//reference to non-const object
	const char &r2 = r1;			//reference to const object
	*/
	const int &cref = foo();		//when const ref binds to rvalue, it makes an object on stack (prolongs its lifetime)
	
	cout<<&cref<<endl;			//display address of this newly created object (created from rvalue)
	
	
	
	
	return 0;
}