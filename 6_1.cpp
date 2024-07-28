//lecture 6  C++  (check static_cast for explicit ctor

#include <iostream>

struct T {};

struct S {
	explicit S(T) { std::cout<<"Explicit ctor S from T\n"; }
};

int foo(S s) {
	std::cout<<"inside foo\n";
	return 0;
}

int main() {
	T x;
	//  foo(x);		//compilation error!
	foo(static_cast<S>(x));		//note that static_cast uses explicit ctor S(T)
	return 0;
}