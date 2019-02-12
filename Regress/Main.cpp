#include "Linear\Matrix.h"
#include "Network\Dense.h"

int main()
{
	Matrix<int> m1{ 2, 2 };
	Matrix<int> m2{ 2, 2 };

	m1(0, 0) = 1;
	m1(0, 1) = 2;
	m1(1, 0) = 3;
	m1(1, 1) = 4;

	m2(0, 0) = 4;
	m2(0, 1) = 3;
	m2(1, 0) = 2;
	m2(1, 1) = 1;

	auto m3 = m1 * m2;

	Dense<float> d1(3, 3, 3, false, nullptr, Relu);

	return 0;
}