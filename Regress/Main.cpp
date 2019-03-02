#include "Linear\Matrix.h"
#include "Network\Model.h"
#include "Network\Dense.h"
#include "Network\InputLayer.h"
#include "Linear\ExprOperators.h" // TODO: replace this with Matrix header once implemented
#include "Stopwatch.h"

int main()
{
	Matrix<int> lhs = { 
		{1, 2},
		{3, 4},
		{5, 6},
	};

	Matrix<int> rhs = {
		{1, 2, 3},
		{4, 5, 6}
	};

	//				  lhs*rhs
	//				{9,  12, 15}	{134, 184, 234}
	// {1, 2, 3} *	{19, 26, 33} =	{305, 418, 531}
	// {4, 5, 6}	{29, 40, 51}


	//   lhs
	// {1, 2}		  rhs*2			{18, 24,  30}
	// {3, 4}   { {1, 2, 3} * 2 } = {38, 52,  66}
	// {5, 6} * { {4, 5, 6}     }   {58, 80, 102}

	//Stopwatch w;

	Matrix<int> exVal;
	auto exp = (lhs * ((rhs - rhs) + rhs) * (lhs * (rhs + rhs))) - lhs * rhs;
	exVal = exp;

	auto vv = exVal.transpose();
	Matrix<int> vvv = lhs.cwiseProduct(~rhs);

	//auto ex = (lhs * rhs) * (ihs * rhs); 
	//exVal = ex;

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