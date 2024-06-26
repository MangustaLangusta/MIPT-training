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



*/



#include <iostream>

using std::cout;
using std::endl;

int main() {
	cout<<"CHAR_BIT = " << CHAR_BIT <<endl;
	cout<<"size of pointer = " << sizeof(void*) << endl;
	return 0;
}