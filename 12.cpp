//lecture 12 c++

/*
It's important to have noexcept move assignment and move ctor
Because in many operations we want to use basic funcs like swap and so on to use them below "Calb's line"
(where we don't want to throw exceptions)

CONDITIONAL NOEXCEPT
	... noexcept( -boolean_expression- );
	
	noexcept(true) == noexcept
	noexcept(false) means there's no noexcept annotation
	
	Hint: if we want make noexcept destructor, we can put:
	
	~MyClass() noexcept(false) { ... }
	
OPERATOR NOEXCEPT
	
	bool b = noexcept(MyClass(my_class));			- checks if copy-constructor is nothrow
	
	operator noexcept returns true/false depending on compile-time calculations
	
	alterantive:
	
	std::is_nothrow_copy_constructible<T>::value    - same as above, but simplier to read
	
	
NOEXCEPT(FALSE) IN DESTRUCTOR

	by default, throwing exception from dtor calls abort
	but if we annotate dtor noexcept(false), we can throw exceptions
	
	and we can check if stack unwinding in progress by calling
		std::uncaught_exceptions()
		
	Do not throw exceptions from destructor!
	
EXCEPTION SAFETY 
	
	We have class MyVector with method pop() which returns last element and deletes it
	(see below)
	What if we do like this:
	MyVectorElemType a = my_vector.pop();		//what if exception will be thrown in operator=?
	
	value of last element of vector is already removed in method pop(),
	but it was not yet assigned to a, so, it has been lost.
	
	That's why in std::vector's method pop() returns void, and for getting last element
	there is a method back()
	
GLOBAL OPERATORS

	in C language, functions malloc and free serve for memory allocation / deallocation
	
	void *p = malloc(10);
	free(p);
	
	in C++, operators new and delete do this
	
	they both have global forms
	AND they both have local form for exact type
	global form will be used for default type, when called:
	
	int *n = new int(5);		//allocation AND construction
	n = (int*) ::operator new(sizeof(int));	//ONLY allocation
	
	second form almost like malloc, but it also may thow exceptions
	
	we can overload global new/delete and change bahaviour of ALL classes which use these ops
	(see below example of overload)
	
	Note new[] should give strong exception guarantee
	
NOTHROW NEW
	
	We have a special form of operator new which does not throws exceptions:
	
	p = new (nothrow) int{42};
	p = (int *) ::operator new(sizeof(int), nothrow);
	see example below
	
	we can add more than one arguments in operator new
	
PLACEMENT NEW

	If memory for some object is already allocated before,
	we can construct object exactly in this pre-allocated memory.
	
	void* operator new(std::size_t size, void* ptr) noexcept;
	
	no operator delete matches to this new because it does not allocate any memory, 
	instead, it only places new object in pre-allocated raw memory
	
	It helps to separate memory allocation from work with object in this memory
	
	For example, we can allocate mempry first, 
	then, if succeeded, continue with constructing object on this memory
	see example below
	
	Important!
	if constructor was called "by hands" (using placement new),
	destructor should be called also "by hands":
	
	Widget* w = new (raw) Widget;
	...
	w->~Widget();
	::operator delete(raw);
	
LOCAL NEW AND DELETE

	We can define operator new and delete in class, in this case these operators 
	be called instead of global new/delete
	
	we even can add unlimited amount of arguments to local  new/delete
	Note: they are static member functions
	
	see example below
	
USE INFORMATION ABOUT EXCEPTIONS FOR METHODS CREATION

	void MyVector::push(const T& t) {
		if(std::is_nothrow_move_assignable<T>::value)
			push_move(t);
		else 
			push_copy(t);
	}
	
	Here we have two variants of push methods:
	-one with move semantics (optimised, but not exception-safe)
	-second with copy semantics (not optimised, but exception-safe)
	
	We can choose between them depending on undellying type is noexcept, or not
	It works, also, when reallocation of memory in vector takes place
	
	std::vector is arranged like this
	
	
STATIC ASSERT

	Static assert works in compile time, which is useful in many cases
	
	For example, we can state pre-conditions for our methods:
	
	static_assert(std::is_nothrow_move_constructible<T>::value);
	static_assert(std::is_nothrow_move_assignable<T>::value);
	...
	
	We use asserts to document invariants of classes
	
	
	
*/

#include <iostream>
#include <stdexcept>
#include <type_traits>			//for is_fundamental
#include <span>
#include <list>


using std::cout;
using std::endl;


//conditional noexcept

template<class T>
T copy(T const& orig) noexcept(std::is_fundamental<T>::value) {
	return orig;
}

template <class T>
T copy2(T const& orig) noexcept(noexcept(T(orig))) {
	return orig;
}

template <class T>
T copy3(T const& orig) noexcept(std::is_nothrow_copy_constructible<T>::value) {
	return orig;
}

class ExceptionAssignmentClass{
	int val_;
public:
	ExceptionAssignmentClass(const int val) : val_(val) {}
	
	ExceptionAssignmentClass& operator=(const ExceptionAssignmentClass& rhs) {
		throw std::exception();
		if(&rhs == this)
			return *this;
		val_ = rhs.val_;
		return *this;
	}
	
};

template <typename T>
class MyVector{
	T *arr_ = nullptr;
	size_t size_, used_ = 0;
public:
	MyVector(const std::span<T> arr) {
		size_t tmp_size = arr.size();
		T* tmp_arr = new T[tmp_size];
		for(int i = 0; i < tmp_size; ++i){
			tmp_arr[i] = arr[i];
		}
		// ----------- Calb's Line -----------------
		std::swap(arr_, tmp_arr);
		used_ = size_ = tmp_size;
		cout<<"MyVector ctor"<<endl;
	}
	~MyVector() { delete[] arr_; cout<<"MyVector dtor"<<endl; }
	
	//this is not safe method
	T pop_not_safe() {
		if(used_ <= 0) throw std::underflow_error("Underflow error");
		T result = arr_[used_ - 1];
		used_ -= 1;
		return result;
	}
	
	//safe pop. it's only removes back element and not returns it, so exception safe
	void pop_safe() {
		if(used_ <= 0) throw std::underflow_error("Underflow error");
		used_ -= 1;
	}
	
};

//class with overloaded operators new and delete
class Widget{
public:
	static void* operator new(size_t size, /*here user args*/int n){
		void *p = malloc(size);
		if(!p) throw std::bad_alloc{};
		printf("Custom Widget new: %p, size is %zu, arg value is %d\n", p, size, n);
		return p;
	}
	
	static void operator delete(void* mem) noexcept{
		free(mem);
		printf("Custom Widget Dealloc: %p\n", mem);
	}
};

//overload of global new and delete
void* operator new(std::size_t n) {
	void* pt = malloc(n);
	
	if(pt == nullptr) throw std::bad_alloc{};
	printf("Alloc: %p, size is %zu\n", pt, n);
	return pt;
}

void* operator new(std::size_t n, const std::nothrow_t& tag) noexcept {
	//here I rearranged new with nothrow in terms of notrmal new
	try{
		return ::operator new(n);
	}
	catch(std::exception& e){
		cout<<"Exception caught! "<<e.what()<<endl;
		return nullptr;
	}
}


void operator delete(void* ptr) noexcept{
	free(ptr);
	printf("Dealloc: %p\n", ptr);
}

void operator delete(void* ptr, size_t s) noexcept{
	free(ptr);
	printf("Dealloc: %zu bytes by address of %p\n", s, ptr);
}


int main(){
	
	{
	int i_arr[5] = {1,2,3,4,5};
	MyVector vec{std::span<int>(i_arr)};
	
	try{
		for(int i = 0; i < 10; ++i)
			cout<<vec.pop_not_safe()<<endl;
	}
	catch(std::underflow_error& e){
		cout<<"Caught: "<<e.what()<<endl;
	}
	}
	cout<<"New and delete overloads"<<endl;
	
	
	int* pi = new int(5);
	delete pi;
	
	cout<<endl;
	
	{
		std::list<int> l;
		l.push_back(42);
	}
	
	
	cout<<endl;
	//nothrow new
	cout<<"Nothrow new"<<endl;
	long long* pl = new(std::nothrow) long long{42};
	if(pl == nullptr) cout<<"nullptr!"<<endl;
	delete pl;
	
	cout<<endl;
	//placement new
	//allows separate works with memory and works with object in memory
	{
	cout<<"Placement new"<<endl;
	void* raw = ::operator new(sizeof(MyVector<int>), std::nothrow);
	if(!raw) cout<<"raw is nullptr, not allocated!"<<endl;
	
	int i_arr[5] = {1,2,3,4,5};
	
	MyVector<int>* vec_p = new(raw) MyVector{std::span<int>(i_arr)};
	
	cout<<"vec_p addr: "<<vec_p<<endl;
	vec_p->~MyVector();
	
	::operator delete(raw);
	
	}
	
	
	cout<<endl;
	
	//custom new
	{
	cout<<"Custom new"<<endl;
	Widget* widget_p = new (42) Widget;
	delete widget_p;
	}
	return 0;
}