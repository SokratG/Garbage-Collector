// GarbageCollector.h :

#pragma once
#include <iostream>
#include <typeinfo>
#include <cstdlib>
#include <list>
#include <memory>
#include <iterator>
#include <thread>
#include <mutex>
#include <chrono>
#include "Exception.h"



//#define LOG_GC
#define _ref(type) &(*type)
 

using std::list;
using std::size;
using ul = unsigned long;
using std::cout;
using std::endl;
using std::thread;
using std::mutex;
using std::timed_mutex;
using std::unique_lock;

class SimpleTimer  
{
public:
	SimpleTimer()
	{
		start = std::chrono::high_resolution_clock::now();
	}
	~SimpleTimer()
	{
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - start;
		cout << "Duration = " << duration.count() << " time." << endl;
	}
private:
	std::chrono::time_point<std::chrono::steady_clock> start, end;
};

template<typename IteratorType>
class GarbageCollectorIterator
{
	using GCIter = GarbageCollectorIterator;
	using T = IteratorType;
private:
	T* currentPtr; // current pointer to memory
	T* headBlock;  // pointer to a head of memory block
	T* endBlock;   // pointer to element after end block
	ul length; 
public:
	
	GarbageCollectorIterator() {
		this->headBlock = this->endBlock = this->currentPtr = nullptr;
		this->length = 0;
	}
	GarbageCollectorIterator(std::initializer_list<T> argv);
	GarbageCollectorIterator(T* ptr, T* head, T* end);

	ul getSize() { return length; }
	T& operator*();
	T* operator->();
	GCIter operator++(); //prefix
	GCIter operator--(); //prefix
	GCIter operator++(int rhs); //postfix
	GCIter operator--(int rhs); //postfix
	IteratorType& operator[](int index);
	bool operator==(GCIter _arg) const noexcept;
	bool operator!=(GCIter _arg) const noexcept;
	bool operator<(GCIter _arg) const noexcept;
	bool operator<=(GCIter _arg) const noexcept;
	bool operator>(GCIter _arg) const noexcept;
	bool operator>=(GCIter _arg) const noexcept;
	GCIter operator-(int offset);
	GCIter operator+(int offset);
	int operator-(GCIter& _arg) const {
		return std::distance(currentPtr, _arg.currentPtr);	
	}
};

template<typename T>
GarbageCollectorIterator<T>::GarbageCollectorIterator(std::initializer_list<T> argv) {
	if (argv.size() != 3)
		throw FailExpectedSize();

	currentPtr = argv[0];
	headBlock = argv[1];
	endBlock = argv[2];
	length = std::distance(argv[1], argv[2]);
}


template<typename T>
GarbageCollectorIterator<T>::GarbageCollectorIterator(T* ptr, T* head, T* end) {
	if (head == end)
		throw FailExpectedSize();
	currentPtr = ptr;
	headBlock = head;
	endBlock = end;
	length = std::distance(head, end);
}

template<typename IteratorType>
inline IteratorType& GarbageCollectorIterator<IteratorType>::operator*()
{
	if (currentPtr >= endBlock || currentPtr < headBlock)
		throw OutOfRangeException();
	return *currentPtr;
}

template<typename IteratorType>
inline IteratorType* GarbageCollectorIterator<IteratorType>::operator->()
{
	if (currentPtr >= endBlock || currentPtr < headBlock)
		throw OutOfRangeException();
	return currentPtr;
}

template<typename IteratorType>
inline GarbageCollectorIterator<IteratorType> GarbageCollectorIterator<IteratorType>::operator++()
{
	currentPtr++;
	if (currentPtr > endBlock)
		throw OutOfRangeException();
	return *this;
}

template<typename IteratorType>
inline GarbageCollectorIterator<IteratorType> GarbageCollectorIterator<IteratorType>::operator--()
{
	currentPtr--;
	if (currentPtr < headBlock)
		throw OutOfRangeException();
	return *this;
}

template<typename IteratorType>
inline GarbageCollectorIterator<IteratorType> GarbageCollectorIterator<IteratorType>::operator++(int rhs)
{
	IteratorType* tempPtr = currentPtr;
	currentPtr++;
	if (currentPtr > endBlock)
		throw OutOfRangeException();
	return GarbageCollectorIterator<IteratorType>(tempPtr, headBlock, endBlock);
}

template<typename IteratorType>
inline GarbageCollectorIterator<IteratorType> GarbageCollectorIterator<IteratorType>::operator--(int rhs)
{
	IteratorType* tempPtr = currentPtr;
	currentPtr--;
	if (currentPtr < headBlock)
		throw OutOfRangeException();
	return GarbageCollectorIterator<IteratorType>(tempPtr, headblock, endBlock);
}

template<typename IteratorType>
inline IteratorType& GarbageCollectorIterator<IteratorType>::operator[](int index)
{
	if (index < 0 || (index >= length))
		throw OutOfRangeException();
	return currentPtr[index];
}


template<typename IteratorType>
inline bool GarbageCollectorIterator<IteratorType>::operator==(GCIter _arg) const noexcept
{
	return (currentPtr == _arg.currentPtr);
}

template<typename IteratorType>
inline bool GarbageCollectorIterator<IteratorType>::operator!=(GCIter _arg) const noexcept
{
	return (currentPtr != _arg.currentPtr);
}

template<typename IteratorType>
inline bool GarbageCollectorIterator<IteratorType>::operator<(GCIter _arg) const noexcept
{
	return (currentPtr < _arg.currentPtr);
}

template<typename IteratorType>
inline bool GarbageCollectorIterator<IteratorType>::operator<=(GCIter _arg) const noexcept
{
	return (currentPtr <= _arg.currentPtr);
}

template<typename IteratorType>
inline bool GarbageCollectorIterator<IteratorType>::operator>(GCIter _arg) const noexcept
{
	return (currentPtr > _arg.currentPtr);
}

template<typename IteratorType>
inline bool GarbageCollectorIterator<IteratorType>::operator>=(GCIter _arg) const noexcept
{
	return (currentPtr >= _arg.currentPtr);
}

template<typename IteratorType>
inline GarbageCollectorIterator<IteratorType> GarbageCollectorIterator<IteratorType>::operator-(int offset)
{
	currentPtr -= offset;
	if (currentPtr < headBlock)
		throw OutOfRangeException();
	return *this;
}

template<typename IteratorType>
inline GarbageCollectorIterator<IteratorType> GarbageCollectorIterator<IteratorType>::operator+(int offset)
{
	currentPtr += offset;
	if (currentPtr > endBlock)
		throw OutOfRangeException();
	return *this;
}




/*
	class define element, which remembered in 
	information list
*/

template<typename Type>
class GarbageCollectorInformation {

	using Iter = GarbageCollectorIterator;
	using GCInfo = GarbageCollectorInformation;

private:

public:
	ul refcount; //current number of reference
	Type* memPtr;  // pointer to allocate memory
	bool isArray;
	ul ArraySize;
	GCInfo(Type* memPtr, ul size = 0) {
		refcount = 1;
		this->memPtr = memPtr;
		isArray = (size != 0) ? true : false;
		ArraySize = size;
	}
};

// compare operator require for std::list from STL
template<class T>
bool operator==(const GarbageCollectorInformation<T>& lhs, const GarbageCollectorInformation<T>& rhs) {
	return (lhs.memPtr == rhs.memPtr);
}






// GCptr implement pointer for garbage collection(memory control)
template<typename Type, int Size = 0>
class GCPtr 
{
public:
	typedef GarbageCollectorIterator<Type> GCiterator;
	typedef GarbageCollectorInformation<Type>  GCInfo; 

private:
	static list<GCInfo> GClist; // list of garbage collection
	Type* addr; // pointer to allocate memory space
	bool isArray; //check if pointer to array
	ul ArraySize;
	static bool firstControl; // check when create first GCPtr
	typename list<GCInfo>::iterator FindPtrInfo(Type* ptr); // return iterator for info in GClist
private:
	static std::thread::id threadId;	// id thread
	static thread GCThread; // thread
	static mutex GCMutex; // mutex
	static timed_mutex GCTimed_Mutex; // mutex
	static int instCount; //counter object GCPtr
	static bool isRunnung() { return instCount > 0; }
protected:
	
public:
	GCPtr(Type* p = nullptr);
	~GCPtr();
	GCPtr(const GCPtr& other);
	static bool collect(); // collect garbage. return true if at least 1 object has been deleted
	static size_t GClistSize();
	static void showlist();
	static void shutdown();
	Type* operator=(Type* ptr);
	GCPtr& operator=(GCPtr& other);
	Type& operator*() { return *addr; }
	Type* operator->() { return addr; }
	Type& operator[](int i) { return addr[i]; }
	operator Type* () { return addr; } //function convertion	
	//void* operator new(size_t sz);
	GCiterator begin();
	GCiterator end();
	friend void __stdcall wrapgc(void);
	static size_t __stdcall gc(void* param = nullptr);
};

template<typename Type, int Size>
void __stdcall wrapgc(void) {
	GCPtr<Type, Size>::gc();
}

// allocate memory for static variable
//------------------------
template<typename Type, int Size>
list<GarbageCollectorInformation<Type>> GCPtr<Type, Size>::GClist;

template<typename Type, int Size>
bool GCPtr<Type, Size>::firstControl = true;

template<typename Type, int Size>
int GCPtr<Type, Size>::instCount = 0;

template<typename Type, int Size>
thread GCPtr<Type, Size>::GCThread(&wrapgc<Type, Size>);

template<typename Type, int Size>
timed_mutex GCPtr<Type, Size>::GCTimed_Mutex;


template<typename Type, int Size>
std::thread::id GCPtr<Type, Size>::threadId;
//------------------------

template<typename Type, int Size>
inline typename list<GarbageCollectorInformation<Type>>::iterator GCPtr<Type, Size>::FindPtrInfo(Type* ptr)
{
	list<GarbageCollectorInformation<Type>>::iterator It;
	for (It = GClist.begin(); It != GClist.end(); ++It) {
		if (It->memPtr == ptr)
			return It;
	}

	return It;
}

template<typename Type, int Size>
size_t __stdcall GCPtr<Type, Size>::gc(void* param)
{
#ifdef LOG_GC
	cout << "Garbage colelction started.\n";
#endif
	
	while (isRunnung())
	{

#ifdef DEBUG
		std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // load simulation
#endif // DEBUG
		collect();
	}


	collect(); // collect garbage before exit function

#define TEST
#ifdef TEST
	GCThread.detach(); // free thread
#endif // DEBUG

#ifdef LOG_GC
	cout << "Garbage collection terminated for "
			<< typeid(Type).name() << "with thread id: " << threadId << endl;
#endif

	return 0;
}


template<typename Type, int Size>
inline GCPtr<Type, Size>::GCPtr(Type* ptr)
{
	unique_lock<timed_mutex> lck(GCTimed_Mutex, std::defer_lock);
	if (!lck.try_lock_for(std::chrono::milliseconds(10000)))
		throw TimeOutException();

	if (firstControl) {
		atexit(shutdown); // when programm is over call shutdown
		threadId = GCThread.get_id();
	}

	firstControl = false;

	auto It = FindPtrInfo(ptr);

	// if p in gclist -> increase reference count, otherwise add them in gclist
	if (It != GClist.end())
		It->refcount++;
	else {
		GarbageCollectorInformation<Type> gcObject(ptr, Size);
		GClist.push_front(gcObject);
	}

	this->addr = ptr;
	ArraySize = Size;
	isArray = (Size > 0) ? true : false;


	this->instCount++;  //increase counter objects GC
}

template<typename Type, int Size>
inline GCPtr<Type, Size>::~GCPtr()
{
	unique_lock<timed_mutex> lck(GCTimed_Mutex, std::defer_lock);
	if (!lck.try_lock_for(std::chrono::milliseconds(10000)))
		throw TimeOutException();
	

	auto It = FindPtrInfo(addr);
	if (It->refcount)
		It->refcount--; // decrease counter 

	this->instCount--; // decrease counter instances of GC objects

#ifdef NO
	collect();
#endif
}


template<typename T, int Size>
inline GCPtr<T, Size>::GCPtr(const GCPtr& other)
{
	unique_lock<timed_mutex> lck(GCTimed_Mutex, std::defer_lock);
	if (!lck.try_lock_for(std::chrono::milliseconds(10000)))
		throw TimeOutException();

	auto It = FindPtrInfo(other.addr);
	It->refcount++;

	addr = other.addr;
	ArraySize = other.ArraySize;
	isArray = (ArraySize > 0) ? true : false;
}



template<typename Type, int Size>
inline bool GCPtr<Type, Size>::collect()
{
	unique_lock<timed_mutex> lck(GCTimed_Mutex, std::defer_lock);
	if (!lck.try_lock_for(std::chrono::milliseconds(10000)))
		throw TimeOutException();

	bool memfree = false;

	list<GarbageCollectorInformation<Type>>::iterator It;
	do {

		for (It = GClist.begin(); It != GClist.end(); ++It) {
			if (It->refcount > 0)
				continue;
			memfree = true;
			

			if (It->memPtr) {
				if (It->isArray) {
					delete[] It->memPtr; // delete array
				}
				else {
					delete It->memPtr;
				}
			}
			break; // continue search;
		}

		if (It == GClist.end())
			break;

		auto tempIt = It;  // exclude pointer to garbage
		std::advance(It, 1);
		GClist.erase(tempIt);

	} while (It != GClist.end());

	lck.unlock();

	return memfree;
}

template<typename Type, int Size>
inline size_t GCPtr<Type, Size>::GClistSize()
{
	unique_lock<timed_mutex> lck(GCTimed_Mutex, std::defer_lock);
	if (!lck.try_lock_for(std::chrono::milliseconds(10000)))
		throw TimeOutException();

	size_t sz = GClist.size();

	lck.unlock();
	return sz;
}

template<typename Type, int Size>
inline void GCPtr<Type, Size>::showlist()
{

	unique_lock<timed_mutex> lck(GCTimed_Mutex, std::defer_lock);
	if (!lck.try_lock_for(std::chrono::milliseconds(10000)))
		throw TimeOutException();

	cout << "GClist<" << typeid(Type).name() << ", " << Size << ">:\n";

	cout << "     memPtr\t      refcount\t\t    value\n";
	if (GClist.begin() == GClist.end()){
		cout << "\t\t     -- Empty --\n\n";
		return;
	}

	for (auto It = GClist.begin(); It != GClist.end(); ++It) {
		cout << "[" << static_cast<void*>(It->memPtr) << "]"
			<< " \t " << It->refcount << " ";
		if (It->memPtr)
			cout << "\t\t\t" << *It->memPtr;
		else
			cout << "\t\t\t---";
		cout << endl;
	}
	cout << endl;

}

template<typename Type, int Size>
inline void GCPtr<Type, Size>::shutdown()
{
#ifdef DEBUG
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	SimpleTimer st;
#endif // DEBUG

	if (GCThread.joinable())
		GCThread.join(); // free thread

	if (GClistSize() == 0)
		return; //list is empty

	for (auto It = GClist.begin(); It != GClist.end(); ++It) {
		It->refcount = 0; // setup all counter to zero
	}
#ifdef LOG_GC
	cout << "Before collection shutdown() for "
			<< typeid(Type).name() << endl;
#endif // LOG_GC

	collect();

#ifdef LOG_GC
	cout << "After collection shutdown() for "
			<< typeid(Type).name() << endl;
#endif // LOG_GC
}

template<typename Type, int Size>
inline Type* GCPtr<Type, Size>::operator=(Type* ptr)
{

	unique_lock<timed_mutex> lck(GCTimed_Mutex, std::defer_lock);
	if (!lck.try_lock_for(std::chrono::milliseconds(10000)))
		throw TimeOutException();

	// decrease counter for current pointer to mem space
	auto It = FindPtrInfo(addr);
	It->refcount--;

	// if new address exists in list just increase count, else create new element
	auto pIt = FindPtrInfo(ptr);

	if (pIt != GClist.end())
		pIt->refcount++;
	else {
		GarbageCollectorInformation<Type> gcObject(ptr, Size);
		GClist.push_front(gcObject);
	}

	this->addr = ptr; //store address

	return ptr;
}

template<typename Type, int Size>
inline GCPtr<Type, Size>& GCPtr<Type, Size>::operator=(GCPtr& other)
{

	unique_lock<timed_mutex> lck(GCTimed_Mutex, std::defer_lock);
	if (!lck.try_lock_for(std::chrono::milliseconds(10000)))
		throw TimeOutException();

	// decrease count for current pointer to mem space
	auto It = FindPtrInfo(addr);
	It->refcount--;

	// increase count for new address
	auto pIt = FindPtrInfo(other.addr);
	pIt->refcount++;

	return other;
}


template<typename Type, int Size>
inline GarbageCollectorIterator<Type> GCPtr<Type, Size>::begin()
{
	int size;

	if (isArray)
		size = ArraySize;
	else
		size = 1;
	return GarbageCollectorIterator<Type>(addr, addr, addr + size);
}

template<typename Type, int Size>
inline GarbageCollectorIterator<Type> GCPtr<Type, Size>::end()
{
	int size;

	if (isArray)
		size = ArraySize;
	else
		size = 1;
	return GarbageCollectorIterator<Type>(addr+size, addr, addr + size);
}
