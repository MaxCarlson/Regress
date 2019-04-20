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
struct MulSquareAlias : public BaseBench
{
	using Mat = Matrix<Type, MajorOrder>;
	inline static constexpr int relDur = 1; // Relative duration

	static std::string name()
	{
		return "Mul Square Alias ";
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

template<class Type, bool MajorOrder>
struct FindBestDims
{
	using Mat = Matrix<Type, MajorOrder>;
	static std::string name()
	{
		return "Find Best Dimensions ";
	}

	static void run()
	{
		Stopwatch sw;
		int count = 0;
		int bestT = std::numeric_limits<int>::max();
		int bestM = 0, bestK = 0, bestN = 0;

		auto printBest = [&]()
		{
			std::cout << "Best m, k, n: " << bestM << ", " << bestK << ", " << bestN << '\n';
		};

		for(int m = 8; m < 1000; m += 64)
			for(int k = 8; k < 1000; k += 64)
				for (int n = 8; n < 1000; n += 64)
				{
					testBs.mc = m; testBs.kc = k; testBs.nc = n;

					auto runTest = [&]()
					{
						sw.start();
						mulSquareAlias(1200);
						return sw.getTime();
					};

					int time = runTest();
					
					if (time < bestT && runTest() < bestT)
					{
						bestT = time;
						bestM = m;
						bestK = k;
						bestN = n;
					}

					if (++count % 15 == 0)
						printBest();
				}
		std::cout << "Best Overall for " << typeid(Type).name() << '\n';
		printBest();
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