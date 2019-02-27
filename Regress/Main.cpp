#include "Linear\Matrix.h"
#include "Network\Model.h"
#include "Network\Dense.h"
#include "Network\InputLayer.h"
#include "Linear\ExprOperators.h" // TODO: replace this with Matrix header once implemented
#include "Stopwatch.h"

int main()
{


	MatrixT<int> lhs = { 
		{1, 2},
		{3, 4},
		{5, 6},
	};

	MatrixT<int> rhs = {
		{1, 2, 3},
		{4, 5, 6}
	};

	//				  lhs*rhs
	//				{9,  12, 15}	{134, 184, 234}
	// {1, 2, 3} *	{19, 26, 33} =	{305, 418, 531}
	// {4, 5, 6}	{29, 40, 51}


	//Stopwatch w;

	MatrixT<int> exVal;
	auto exp = rhs + (rhs + rhs) + (rhs + rhs);
	exVal = exp;

	auto ex = rhs * (lhs * rhs) * (lhs * rhs); 
	exVal = ex;

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