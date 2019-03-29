#include "../Linear/Matrix.h"
#include "../Linear/Stopwatch.h"
#include <iostream>
#include <string>

struct BaseBench
{
	static void start() { s.start(); }
	static void stop() { s.stop(); }
	static void printTime() { s.printCurrent(); }
	inline static Stopwatch<std::chrono::milliseconds> s;

	template<class Func, class... Args>
	static void runFunc(Func&& func, Args ...args)
	{
		s.start();
		func(std::forward<Args>(args)...);
		s.printCurrent();
	}
};

template<class... Args>
inline void print(Args&& ...args) 
{ (std::cout << ... << args);}


#define RUN_FUNC(func, subname, ...)  std::cout << subname << ' '; \
	BaseBench::start(); \
	func(##__VA_ARGS__); \
	BaseBench::printTime() \



template<class Type, bool MajorOrder>
struct basic : public BaseBench
{
	using Mat = Matrix<Type, MajorOrder>;

	static void run()
	{
		//RUN_FUNC(doOther, 1);
		print("mulSquareAlias \n");
		RUN_FUNC(mulSquareAlias, "100", 100);
		RUN_FUNC(mulSquareAlias, "250", 250);
		RUN_FUNC(mulSquareAlias, "500", 500);
	}

	static void mulSquareAlias(int size)
	{
		Mat m(size, size);
		m = m * m;
	}
};

int main()
{
	basic<int, false>::run();
	return 0;
}