#include <map>
#include <array>
#include <list>
#include <memory>
template<class T, size_t Size>
class RingBuffer {
public:
	RingBuffer() = default;
	~RingBuffer() {};
	void pushBack(T val) {
		if (haveFreeSpase()) {
			buf[head] = val;
			head = (head + 1)%(Size);
		}
	}
	void pop() {
		if (!isEmpty()) {
			tail = (tail + 1)%(Size);
		}
	}
	T& getTail() {
		return buf[tail];
	}
	T& getHead() {
		return buf[head];
	}
	bool isFull() {
		return ((head + 1)%(Size)) == tail;
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
template <typename T,size_t PoolSize>
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
	PoolType memoryPool;
	RingBuffer<pointer,PoolSize> avaiableBlocks;

	MyAllocator() {
		initAvaiableBlocks();
	}
	pointer allocate(std::size_t size) {
		if (size>1||avaiableBlocks.isEmpty()) {
			std::bad_alloc exception;
			throw exception;
		}
		auto tmp = avaiableBlocks.getTail();
		avaiableBlocks.pop();
		return tmp;
	}
	void deallocate(pointer p, std::size_t size) {
		if (size > 1) {
			throw std::abort;
		}
		avaiableBlocks.pushBack(p);
	}
	void destroty(pointer p) {
		p->~T();
	}
	template <typename U,typename ...Args >
	void construct(U* p, Args&& ...args) {
		new(p) U(std::forward<Args>(args)...);
	}
	template<typename U>
	struct rebind
	{
		typedef MyAllocator<U,PoolSize> other;
	};
	size_type max_size() const {
		return PoolSize;
	}
	MyAllocator(const MyAllocator& other) {
		initAvaiableBlocks();
	}
	template< class U >
	MyAllocator(const MyAllocator<U,PoolSize>& other) {
		initAvaiableBlocks();
	}
private:
	void initAvaiableBlocks() {
		for (auto i = 0; i < PoolSize; i++) {
			avaiableBlocks.pushBack(&(memoryPool)[i]);
		}
	}
};

int main()
{
	std::map<long, long, std::less<const long>, MyAllocator<std::pair<const long, long>, 1024>> a;
	for (auto i = 0; i < 1023; i++) {
		a.insert({ i, i*i });
	}
	for (auto i = 512; i < 1023; i++) {
			a.erase(i);
	}
	return 0;
}

