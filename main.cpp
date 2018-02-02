#include "allocator.h"
#include <functional>
#include <iostream>
#include <utility>
template<typename T, typename Alloc>
struct MyCustomContainer{
	
	struct Node{
		Node(T value, Node* ptr):value(value),next(ptr){};
		Node() = default;
		T value;
		Node* next=nullptr;
	}*node_=nullptr;
	using alloc_type = typename Alloc::template rebind<Node>::other;
	alloc_type allocator;
	void insert(T val){
		auto tmp = allocator.allocate(1);
		allocator.construct(tmp,val,node_);
		
		std::swap(tmp,node_);
	}
	template <typename R>
	void forEach(R&& function){
		auto tmpNode = node_;
		while(tmpNode!=nullptr){
			function(tmpNode->value);
			tmpNode = tmpNode->next;
		}
	}
};
long factorial(long n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

int main()
{
	std::map<long, long> stdAllocMap;
	std::map<long, long, std::less<const long>, MyAllocator<std::pair<const long, long>, 11>> customAllocMap;
	MyCustomContainer<int,MyAllocator<int,11>> myContainer;
	for (auto i = 0; i < 10; i++) {
		long fact = factorial(i);
		stdAllocMap.insert({ i, fact });
		customAllocMap.insert({i,fact});
		myContainer.insert(i);
	}
	for(auto &i:customAllocMap){
		std::cout<<i.first<<" "<<i.second<<std::endl;
	}
	for (auto i = 5; i < 10; i++) {
			customAllocMap.erase(i);
	}
	for(auto &i:customAllocMap){
		std::cout<<i.first<<" "<<i.second<<std::endl;
	}
	
	myContainer.forEach([](int a){
		std::cout<<a<<std::endl;
	});
	
	return 0;
}

