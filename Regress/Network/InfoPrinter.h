#pragma once
#include <sstream>
#include <iostream>

class InfoPrinter
{
	int counter;
	int frequency;
public:

	InfoPrinter(int frequency) :
		counter{ 0 }, 
		frequency{ frequency }
	{
	}

	void printInfo(double error)
	{
		if (++counter % frequency != 0)
			return;

		std::stringstream ss;
		ss << "Mini-batch " << counter << '\n';
		ss << "Current Error is: " << error << '\n';

		std::cout << ss.str();
	}
};