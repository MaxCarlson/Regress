#include "../Linear/Matrix.h"
#include "../Linear/Stopwatch.h"
#include "GemmDimensionSearch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>


struct TestResults
{
	struct TypeResult
	{
		std::vector<int> times;
	};

	std::string testName;
	std::set<int> sizes;
	std::map<std::string, TypeResult> results;

	TestResults(std::string testName) :
		testName{testName}
	{}

	void addSubtestSize(int size)
	{
		sizes.emplace(size);
	}

	void addResult(const std::string& type, int size, int time) 
	{ 
		addSubtestSize(size);
		results[type].times.emplace_back(time);
	}

	void print() const
	{
		std::stringstream ss;
		ss << testName << '\n';
		static constexpr int spacing = 10;

		ss << std::left << std::setw(15) << "Test Count: ";
		for (const auto& sz : sizes)
			ss << std::left << std::setw(spacing) << sz;

		for (auto[k, v] : results)
		{
			ss << '\n' << std::setw(15) << k;
			for (int i = 0; i < sizes.size(); ++i)
				ss << std::left << std::setw(spacing) << v.times[i];
		}


		std::cout << ss.str() << "\n\n";
	}
};

struct BaseBench
{
	static void start() { s.start(); }
	static void stop() { s.stop(); }
	static void printTime(int factor = 1) { s.printCurrent(factor); }
	inline static Stopwatch<std::chrono::microseconds> s;

	template<class Func, class Type, class Size, class... Args>
	static void runFunc(Func&& func, Type t, TestResults& results, Size size, int count, Args ...args)
	{
		s.start();
		for (int i = 0; i < count; ++i)
			func(size, std::forward<Args>(args)...);

		results.addResult(typeid(Type).name(), size, s.getTime() / count);
	}
};

template<class Type, bool MajorOrder>
struct MulSquareAlias : public BaseBench
{
	using Mat = Matrix<Type, MajorOrder>;
	inline static constexpr int relDur = 3; // Relative duration

	static std::string name()
	{
		return "Mul Square Alias ";
	}

	static void run(TestResults& res)
	{
		runFunc(mulSquareAlias, Type{}, res, 50,	50 * relDur);
		runFunc(mulSquareAlias, Type{}, res, 100,	20 * relDur);
		runFunc(mulSquareAlias, Type{}, res, 250,	20 * relDur);
		runFunc(mulSquareAlias, Type{}, res, 500,	15 * relDur);
		runFunc(mulSquareAlias, Type{}, res, 1000,	10 * relDur);
		runFunc(mulSquareAlias, Type{}, res, 1500,	8  * relDur);
	}

	static void mulSquareAlias(int size)
	{
		Mat m(size, size);
		m = m * m;
	}
};

std::string majorOrderToString(bool MajorOrder)
{
	return MajorOrder ? "ColMajor Order" : "RowMajor Order";
}

template<template<class, bool> class Bench, bool MajorOrder>
void runTypeTests()
{
	TestResults res{ Bench<float, MajorOrder>::name() +  majorOrderToString(MajorOrder)};

	Bench<float,	MajorOrder>::run(res);
	Bench<double,	MajorOrder>::run(res);
	Bench<int,		MajorOrder>::run(res);
	res.print();
}

int main()
{
	//FindBestDims<int, RowMajor>::run();
	runTypeTests<MulSquareAlias, RowMajor>();
	//runTypeTests<MulSquareAlias, ColMajor>();

	return 0;
}