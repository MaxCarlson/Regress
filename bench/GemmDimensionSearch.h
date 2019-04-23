#pragma once
#include <string>
#include "../Linear/ForwardDeclarations.h"

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
		static constexpr int runs = 2;

		auto printBest = [&]()
		{
			std::cout << "Best m, k, n: " << bestM << ", " << bestK << ", " << bestN << '\n';
		};

		for (int m = 8; m < 1000; m += 64)
			for (int k = 8; k < 1000; k += 64)
				for (int n = 8; n < 1000; n += 64)
				{
					testBs.mc = m; testBs.kc = k; testBs.nc = n;

					auto runTest = [&]()
					{
						sw.start();
						mulSquareAlias(800);
						return sw.getTime();
					};

					int time = 0;
					for (int i = 0; i < runs; ++i)
						time += runTest();
					time /= runs;

					if (time < bestT)
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
