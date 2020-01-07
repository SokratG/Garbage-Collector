#include <iostream>
#define LOG_GC
#include "GarbageCollector.h"

using std::cout;
using std::endl;


void test1(void);
const int N = 10000;
class LoadTest {

private:
	int a, b;
public:
	double n[N]{ 0 };
	double val;
	LoadTest() { a = b = 0; }
	LoadTest(int x, int y) {
		a = x;
		b = y;
		val = 0.0;
	}
	friend std::ostream& operator<<(std::ostream& stream, LoadTest& obj);
};


std::ostream& operator<<(std::ostream& stream, LoadTest& obj)
{
	stream << "(" << obj.a << " " << obj.b << ")";
	return stream;
}



int main(int agrc, char* argv[])
{
	try
	{
		test1();
	}
	catch (TimeOutException & e)
	{
		cout << "Fail GC: " << e.getMessage() << endl;
	}
	catch (const std::exception& e)
	{
		cout << "Fail GC: " << e.what() << endl;
		return 1;
	}
	
	catch (...)
	{
		cout << "Unknow fail GC!"  << endl;
	}

	return 0;
}



void test1(void)
{
	GCPtr<LoadTest> mp;
	int i;
	for (i = 1; i < 2000; i++) {
		try
		{
			mp = new LoadTest(i, i);
			if (!(i % 100))
				cout << "gclist contains " << mp.GClistSize() << " entries.\n";
		}
		catch (const std::bad_alloc& ba)
		{
			cout << "Last object: " << *mp << endl;
			cout << "Length of gclist: " << mp.GClistSize() << endl;
		}
	}
	cout << "After work contains: " << mp.GClistSize() << " entries.\n";
#undef DEBUG
#ifdef DEBUG
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	cout << "After cleaning contains: " << mp.GClistSize() << " entries.\n";
#endif
	return;
}