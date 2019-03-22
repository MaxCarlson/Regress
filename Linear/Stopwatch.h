#pragma once
#include <iostream>
#include <chrono>

template<class Measure = std::chrono::microseconds>
class Stopwatch
{
	std::chrono::time_point<std::chrono::high_resolution_clock> now()
	{
		return std::chrono::high_resolution_clock::now();
	}
public:
	Stopwatch() :
		begin{ now() },
		end{ begin }
	{}

	void start()
	{
		begin = end = now();
	}

	void stop()
	{
		end = now();
	}

	void printCurrent()
	{
		auto n = now();
		auto count = std::chrono::duration_cast<Measure>(n - begin).count();
		std::cout << count << '\n';
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> begin, end;
};