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
	inline static const std::string order = MajorOrder ? "Column" : "Row";

	static void run()
	{
		print("mulSquareAlias ", typeid(Type).name(), '\n');
		print("MajorOrder: ", order, '\n');
		RUN_FUNC(mulSquareAlias, "10", 10);
		RUN_FUNC(mulSquareAlias, "50", 50);
		RUN_FUNC(mulSquareAlias, "100", 100);
		RUN_FUNC(mulSquareAlias, "250", 250);
		//RUN_FUNC(mulSquareAlias, "500", 500);
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
	runTypeTests<basic, false>();
	runTypeTests<basic, true>();

	return 0;
}