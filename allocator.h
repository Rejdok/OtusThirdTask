#include <map>
#include <array>
#include <list>
#include <memory>
#include <functional>
#include <iostream>
#include <utility>
//simple pool allocator can't use whith array,vector, e.t.c.

template<class T, size_t Size>
class RingBuffer {
public:
	RingBuffer() = default;
	~RingBuffer() {};
	void pushBack(T val) {
		if (haveFreeSpase()) {
			buf[head] = val;
			head = (head + 1) % (Size);
		}
	}
	void pop() {
		if (!isEmpty()) {
			tail = (tail + 1) % (Size);
		}
	}
	T& getTail() {
		return buf[tail];
	}
	T& getHead() {
		return buf[head];
	}
	bool isFull() {
		return ((head + 1) % (Size)) == tail;
	}
	bool isEmpty() {
		return head == tail;
	}
	size_t usedSpace() {
		int64_t _head = head, _tail = tail;
		return (_tail>_head ? static_cast<int64_t>(Size) : 0) + (_head - _tail);
	}
	size_t avaiableSpace() {
		return Size - usedSpace();
	}
	uint32_t haveFreeSpase() {
		return !isFull();
	}
protected:
	std::array<T, Size> buf{};
	size_t head = 0;
	size_t tail = 0;

};

//simple pool allocator can't use whith array,vector, e.t.c.
template <typename T, size_t PoolSize>
struct MyAllocator
{
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef std::array<value_type, PoolSize> PoolType;
	typedef std::true_type propagate_on_container_copy_assignment;
	typedef std::true_type propagate_on_container_move_assignment;
	typedef std::true_type propagate_on_container_swap;
	
	MyAllocator() = default;
	class Pool {
	public:
		PoolType memoryPool = {};
		RingBuffer<pointer, PoolSize> avaiableBlocks = {};
		Pool() {
			initAvaiableBlocks();
		}
		Pool(Pool&& other) {
			initAvaiableBlocks();
		}
		pointer allocate(std::size_t size) {
			std::cout << " In pool" << std::endl <<this<<" "<< &memoryPool<<std::endl;
			if (size>1 || avaiableBlocks.isEmpty()) {
				std::bad_alloc exception;
				throw exception;
			}
			auto tmp = avaiableBlocks.getTail();
			avaiableBlocks.pop();
			return tmp;
		}
		size_type max_size() const {
			return PoolSize;
		}
		size_t isFull() {
			return avaiableBlocks.isEmpty();
		}
	private:
		void initAvaiableBlocks() {
			std::cout << "Init new pool" << std::endl << "addres = " << &memoryPool<<"this "<<this<<std::endl;
			for (auto i = 0; i < PoolSize; i++) {
				std::cout << "Add to pool addres" << &(memoryPool[i])<<std::endl;
				avaiableBlocks.pushBack(&(memoryPool[i]));
			}
		}
	};
	std::shared_ptr<std::list<Pool>> memoryPools = std::make_shared<std::list<Pool>>(std::list<Pool>{});
	MyAllocator(const MyAllocator& other) {
		memoryPools = other.memoryPools;
	}
	template< class U >
	MyAllocator(const MyAllocator<U, PoolSize>& other) {
		std::shared_ptr<std::list<Pool>> memoryPools = std::make_shared<std::list<Pool>>(std::list<Pool>{});
	}
	template <typename U, typename ...Args >
	void construct(U* p, Args&& ...args) {
		new(p) U(std::forward<Args>(args)...);
	}
	template<typename U>
	struct rebind
	{
		typedef MyAllocator<U, PoolSize> other;
	};
	void reserve() {
		memoryPools->emplace_back(Pool{});
	}
	pointer allocate(std::size_t size) {
		Pool* avaiablePool = findAvaiablePool();
		if (avaiablePool == nullptr) {
			reserve();
			avaiablePool = &(memoryPools->back());
		}
		std::cout << avaiablePool << " " << &(memoryPools->back())<<std::endl;
		auto returnedPtr = avaiablePool->allocate(size);
		std::cout << "Return form pool ptr \n"<<&avaiablePool->memoryPool <<" "<< returnedPtr<<std::endl;
		return returnedPtr;
	}
	void deallocate(pointer p, std::size_t size) {
		//current implementation doesn't dealocate resourses  
	}
	template< class U >
	void destroy(U* p) {
		p->~U();
	}
private:
	Pool* findAvaiablePool() {
		for (auto&i : *memoryPools) {
			if (i.isFull()==false) {
				return &i;
			}
		}
		return nullptr;
	}
};