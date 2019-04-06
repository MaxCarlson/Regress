#include "../Linear/Matrix.h"
#include "../Linear/Stopwatch.h"
#include <iostream>
#include <string>

struct BaseBench
{
	static void start() { s.start(); }
	static void stop() { s.stop(); }
	static void printTime(int factor = 1) { s.printCurrent(factor); }
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


#define RUN_FUNC(func, subname, tests, ...)  std::cout << subname << ' '; \
	BaseBench::start(); \
	for(int i = 0; i < tests; ++i) \
		func(##__VA_ARGS__); \
	BaseBench::printTime(tests) \



template<class Type, bool MajorOrder>
struct basic : public BaseBench
{
	using Mat = Matrix<Type, MajorOrder>;
	inline static const std::string order = MajorOrder ? "Column" : "Row";

	static void run()
	{
		print("mulSquareAlias ", typeid(Type).name(), '\n');
		print("MajorOrder: ", order, '\n');
		//RUN_FUNC(mulSquareAlias, "10", 10,  10);
		//RUN_FUNC(mulSquareAlias, "50", 10, 50);
		RUN_FUNC(mulSquareAlias, "100", 10, 100);
		RUN_FUNC(mulSquareAlias, "250", 10, 250);
		RUN_FUNC(mulSquareAlias, "500", 10, 500);
		RUN_FUNC(mulSquareAlias, "1000", 10, 1000);
		RUN_FUNC(mulSquareAlias, "1500", 6, 1500);
	}

	static void mulSquareAlias(int size)
	{
		Mat m(size, size);
		m = m * m;
	}
};

template<template<class, bool> class Bench, bool MajorOrder>
void runTypeTests()
{
	Bench<float,	MajorOrder>::run();
	Bench<double,	MajorOrder>::run();
	Bench<int,		MajorOrder>::run();
	//Bench<int64_t,	MajorOrder>::run();
}

int main()
{
	runTypeTests<basic, RowMajor>();
	//runTypeTests<basic, true>();

	return 0;
}