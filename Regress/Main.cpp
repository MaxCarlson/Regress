#include "Linear\Matrix.h"
#include "Network\Model.h"
#include "Network\Dense.h"
#include "Network\InputLayer.h"
#include "Linear\Stopwatch.h"


int main()
{
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
	Dense<float>  d1(2,  false, &in, Activation::Relu);
	Dense<float>  d2(10, false, &d1, Activation::Relu);
	Dense<float>  d3(16, false, &d1, Activation::Relu);
	Dense<float>  d4(2,  false, &d2, Activation::Relu);


	Model<float> mod(in, &d4, 0.005, ErrorFunction::Squared, InfoPrinter{ 5 });
	
	for(int i = 0; i < 1500; ++i)
		mod.train(input, label);



	return 0;
}