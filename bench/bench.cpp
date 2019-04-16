#include "../Linear/Matrix.h"
#include "../Linear/Stopwatch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>


struct TestResults
{
	struct TypeResult
	{
		std::vector<int> times;
	};

	std::string testName;
	std::vector<std::string> names;
	std::map<std::string, TypeResult> results;

	TestResults(std::string testName) :
		testName{testName}
	{}

	template<class... Args>
	void addSubtestNames(Args&& ...args)
	{
		std::vector<std::string> tmp{std::forward<Args>(args)...};
		names = std::move(tmp);
	}

	void addResult(const std::string& type, int time) 
	{ 
		results[type].times.emplace_back(time);
	}

	void print() const
	{
		std::stringstream ss;
		ss << testName << '\n';
		static constexpr int spacing = 10;

		ss << std::left << std::setw(15) << "Test Count: ";
		for (int i = 0; i < names.size(); ++i)
			ss << std::left << std::setw(spacing) << names[i];

		for (auto[k, v] : results)
		{
			ss << '\n' << std::setw(15) << k;
			for (int i = 0; i < names.size(); ++i)
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

	template<class Func, class Type, class... Args>
	static void runFunc(Func&& func, Type t, TestResults& results, int count, Args ...args)
	{
		s.start();
		for(int i = 0; i < count; ++i)
			func(std::forward<Args>(args)...);

		results.addResult(typeid(Type).name(), s.getTime() / count);
	}
};

template<class Type, bool MajorOrder>
struct basic : public BaseBench
{
	using Mat = Matrix<Type, MajorOrder>;
	inline static const std::string order = MajorOrder ? "Column" : "Row";
	inline static constexpr int relDur = 4; // Relative duration

	static std::string name()
	{
		return "mulSquareAlias ";
	}

	static void run(TestResults& res)
	{
		res.addSubtestNames("50", "100", "250", "500", "1000", "1500");

		runFunc(mulSquareAlias, Type{}, res, 50 * relDur, 50);
		runFunc(mulSquareAlias, Type{}, res, 20 * relDur, 100);
		runFunc(mulSquareAlias, Type{}, res, 20 * relDur, 250);
		runFunc(mulSquareAlias, Type{}, res, 15 * relDur, 500);
		runFunc(mulSquareAlias, Type{}, res, 10 * relDur, 1000);
		runFunc(mulSquareAlias, Type{}, res, 7  * relDur, 1500);
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
	runTypeTests<basic, RowMajor>();
	//runTypeTests<basic, ColMajor>();

	return 0;
}