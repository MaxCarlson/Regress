#include "Linear\Matrix.h"
#include "Network\Model.h"
#include "Network\Dense.h"
#include "Network\InputLayer.h"
#include "Linear\MatrixOp.h"

int main()
{
	Matrix<float> input = {
		{0, 1},
		{1, 0}
	};

	Matrix<float> label = {
		{0, 1},
		{0, 1},
	};

	// TODO: Shit, looks like the backprop algo I wrote only works if all neurons are the same size!

	Input<float>  in(&input);
	Dense<float>  d1(2, false, &in, Activation::Relu);
	Dense<float>  d2(3, false, &d1, Activation::Relu);
	Dense<float>  d3(2, false, &d2, Activation::SoftMax);


	Model<float> mod(in, &d3, 0.1, ErrorFunction::Squared);
	
	//Matrix<float> m1(label.rows(), label.columns());

	while(true)
		mod.train(input, label);



	return 0;
}