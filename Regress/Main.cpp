#include "Linear\Matrix.h"
#include "Network\Dense.h"
#include "Network\InputLayer.h"

int main()
{
	/*
	Matrix<float> m1( 2, 2 );
	Matrix<float> m2( 2, 2 );

	m1(0, 0) = 1;
	m1(0, 1) = 2;
	m1(1, 0) = 3;
	m1(1, 1) = 4;

	m2(0, 0) = 4;
	m2(0, 1) = 3;
	m2(1, 0) = 2;
	m2(1, 1) = 1;

	auto m3 = m1 * m2;
	*/

	Matrix<float> input(1, 3);
	input(0, 0) = 2;
	input(0, 1) = 3;

	Input<float> in(input);
	Dense<float> d1(3, 3, false, &in, Relu);
	Dense<float> d2(3, 3, false, &d1, Relu);

	d1.feedForward(input);

	return 0;
}