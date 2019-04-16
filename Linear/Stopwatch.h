#pragma once
#include <iostream>
#include <chrono>

template<class Measure = std::chrono::microseconds>
class Stopwatch
{
	std::chrono::time_point<std::chrono::high_resolution_clock> now() const
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

	decltype(auto) getTime() const
	{
		auto n = now();
		auto count = std::chrono::duration_cast<Measure>(n - begin).count();
		return count;
	}

	void printCurrent(int factor = 1)
	{
		auto n = now();
		auto count = std::chrono::duration_cast<Measure>(n - begin).count() / factor;
		std::cout << count << '\n';
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> begin, end;
};