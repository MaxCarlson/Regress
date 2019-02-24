#include "Linear\Matrix.h"
#include "Network\Model.h"
#include "Network\Dense.h"
#include "Network\InputLayer.h"
#include "Linear\MatrixTest.h"

int main()
{

	MatrixT<float> lhs = { 
		{1, 2},
		{3, 4},
		{5, 6}
	};

	MatrixT<float> rhs = {
		{5, 6, 7},
		{8, 9, 10}
	};

	auto ep = lhs * rhs * lhs;
	rhs = ep;

	//MatrixT<float> t3 = (t1 * t2);// t1 * (t1 + t1) * ;

	Matrix<float> input = {
		{0, 1},
		{1, 0},
		{0, 1},
		{1, 1},
	};

	Matrix<float> label = {
		{0, 1},
		{0, 1},
		{0, 1},
		{1, 0},
	};

	Input<float>  in(&input);
	Dense<float>  d1(2, false, &in, Activation::Relu);
	Dense<float>  d2(10, false, &d1, Activation::Relu);
	Dense<float>  d3(16, false, &d1, Activation::Relu);
	Dense<float>  d4(2, false, &d2, Activation::Relu);


	Model<float> mod(in, &d4, 0.009, ErrorFunction::Squared, InfoPrinter{ 5 });
	
	for(int i = 0; i < 100; ++i)
		mod.train(input, label);



	return 0;
}