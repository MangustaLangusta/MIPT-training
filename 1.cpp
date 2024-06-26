//cache

#include <unordered_map>
#include <iostream>
#include <optional>
#include <string>


struct page {
	int index;	//page index 1, 2, 3...
	int sz;			//page size
	char *data;	//page data
	page(int new_index) : index(new_index) {}
};

/*
strategy LRU (least recently used)
	- if element found, it moves front
	- if not found, it goes front, last elemnt removed
	
*/


/*
how to read c-declarations (cdecl):

	-> <- ^   (go right, go left, go up)
	example:
	typedef struct page_t * (*slow_get_page_t) ();
	
	what is slow_get_page_t?
	go rigth:   -none
	go left:    pointer to...
	go up:
	go right:   function, which returns...
	go left:    pointer to page_t
	
	so:
		-pointer to function which returns pointer to page_t

*/


/* Two abstraction mechanisms of C++:
	- ability to unite structures and methods
	- generalisation of data and methods (via templates)
*/


/* C-style macros may have side effects

  for example, max of two numbers:
	#define MAX(x,y) (((X) > (y)) ? (x) : (y))
	
	what if it will be i++ instead of x? 
	this will be done several times
	
	and performance issues 
	(what if function calls instead of x and y?)
	same functions will be called several times
*/

/* better not to write this->... in methods if it possible */

/* put const for methods which are not change fields */

/* Do not start names with underline (_value, __smthg, etc) 
		They are reserved for std library
*/


/*
	- train in git (version control)
	- cmake   (Bill Hofman   CMake: one tool to build them all)
	- tests (learn how to use scripts for tests)
	- code quality: short, logical, ...

/* if any constructor other than default defined, default constructor invalidates */
//my version
//its could be improved by using only standart types: list and unordered_map.
//I did not used list because was not aware of list::splice() method - it moves elements betweet lists without invalidating iterators
//Iterators were my concern, but splice solves this.



using std::unordered_map;
using std::cout;
using std::cin;
using std::optional;
using std::string;

namespace caches{			//do not spoil global namespace



using index_t = int;

template<typename T>
T slow_get_value(index_t index){
	cout<<"Slow Get Value of index " <<index<<"\n";
	return T(static_cast<int>(index));
}



template <typename V>
class Cache{
private:

	template <typename L>
	struct MyLRUList{

		template <typename E>
		struct MyListElem{
			index_t index_;
			E value_;
			MyListElem<E>* prev_;
			MyListElem<E>* next_;
			MyListElem(E val, MyListElem<E>* prev = nullptr, MyListElem<E>* next = nullptr) 
				: value_(val), prev_(prev), next_(next), index_(static_cast<index_t>(val.index) ) {
				cout<<"Constructor MyListElem index = " << index_ << "\n";
			}
			
		};	//struct MyListElem

		int max_size_;
		int current_size_ = 0;
		
		MyListElem<L>* first_node_ = nullptr;
		MyListElem<L>* last_node_ = nullptr;
		
		unordered_map<index_t, MyListElem<L>* > associations_;
		
		MyLRUList(int max_size = 5) : max_size_(max_size) {
			cout<<"Constructor MyLRUList with size = " << max_size_ << "\n";
		}
		~MyLRUList() { 
			cout<<"Destructor MyLRUList...\n";
			int count = 0;
			for(auto it : associations_){
				delete (it.second);
				++count;
			}
			cout<<count<<" items deleted\n";
		}
		
		void Dump() const{
			cout<<"dumping cache...\n";
			for(auto it : associations_){
				cout<<" - - - - \n";
				cout<<"	index: " << it.first << "\n";
				cout<<"	addr:  " << it.second << "\n";
			}
			cout<<" - - - - - \n";
		}
		
		void MoveFront(MyListElem<L>* node_to_front) {
			cout<<"MoveFront node with index " << node_to_front->index_ << "\n";
			if(first_node_ == node_to_front)
				return;
			
			auto prev_elem = node_to_front->prev_;
			auto next_elem = node_to_front->next_;
			
			if(node_to_front == last_node_){
				last_node_ = prev_elem;
			}
			
			if(prev_elem != nullptr){
				prev_elem->next_ = next_elem;
			}
			if(next_elem != nullptr){
				next_elem->prev_ = prev_elem;
			}
			prev_elem = nullptr;
			next_elem = first_node_;
			
			first_node_->prev_ = node_to_front;
			first_node_ = node_to_front;
		}	//MoveFront
		
		optional<L> GetValue(index_t index) {
			cout<<"GetValue of index "<<index<< "\n";
			auto assoc_it = associations_.find(index);
			if(assoc_it != associations_.end()){
				cout<<"Cache hit\n";
				MyListElem<L>* elem_ptr = assoc_it->second;
				
				MoveFront(elem_ptr);
				return std::make_optional<L> (elem_ptr->value_);
			}
			cout<<"Cache miss\n";
			return std::nullopt;
		}	//GetValue
		
		void AddValue(L new_value) {
			index_t new_index = static_cast<index_t>(new_value.index);
			
			cout<<"Add to cache page with index = " << new_index << "\n";
			
			if(current_size_ < max_size_){
				MyListElem<L>* new_elem_ptr = new MyListElem<L>(new_value, last_node_, nullptr);
				associations_.insert( {new_index, new_elem_ptr} );
				last_node_ = new_elem_ptr;
				if(current_size_ == 0){
					first_node_ = last_node_;
				}
				current_size_ = associations_.size();
			}
			else{
				cout<<"Page with index "<<last_node_->index_<< "will be removed\n";
				associations_.erase(last_node_->index_);
				last_node_->value_ = new_value;
				last_node_->index_ = new_index;
				associations_.insert( {new_index, last_node_} );
				MoveFront(last_node_);
			}
		}	//AddFront
		
	};	//struct MyLRUList
	
	MyLRUList<V> cache_;
	
public:
	Cache(int max_size = 5) {}
	
public:
	V GetValue(index_t index) {	
		
		optional<V> search_result = cache_.GetValue(index);
		
			
		if(search_result != std::nullopt){  //if have such element in cache
			return search_result.value();
		}
			
		else{  //cache miss
			const V new_elem = slow_get_value<V>(index);
			cache_.AddValue(new_elem);
			return new_elem;
		}
	}
	
	void Dump() const{
		cache_.Dump();
	}
};

}	//namespace caches


int main() {
	
	caches::Cache<page> my_cache{5};			//better to use {} in c++
	
	string input = "";
	while(1){
		cout<<"Type requested page index\n\n";
		cin >> input;
		cout<<"\n";
		if(input == "exit")
			break;
		if(input == "dump"){
			cout<<"\n";
			my_cache.Dump();
			continue;
		}
		if(input == "cls"){
			system("cls");
			continue;
		}
		int request = std::stoi(input);
		my_cache.GetValue(request);
	}
	
	cout<<"program ended\n";
	return 0;
}