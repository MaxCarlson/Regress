#include "Linear\Matrix.h"
#include "Network\Model.h"
#include "Network\Dense.h"
#include "Network\InputLayer.h"
#include "Linear\MatrixOp.h"

int main()
{

	/* // Transpose testing
	Matrix<float> test(3, 2);
	test(0, 0) = 1;
	test(0, 1) = 2;
	test(1, 0) = 3;
	test(1, 1) = 4;
	test(2, 0) = 5;
	test(2, 1) = 6;
	auto t = test.transpose();
	*/

	///* // Batch gradient descent testing
	// Training bitwise XOR ( ^ ) operation

	Matrix<float> input(3, 2);
	input(0, 0) = 0;
	input(0, 1) = 1;
	input(1, 0) = 1;
	input(1, 1) = 0;
	input(2, 0) = 1;
	input(2, 1) = 1;

	Matrix<float> label(3, 2);
	label(0, 0) = 0;
	label(0, 1) = 1;
	label(1, 0) = 0;
	label(1, 1) = 1;
	label(2, 0) = 1;
	label(2, 1) = 0;
	//*/

	/*
	Matrix<float> input(1, 2);
	input(0, 0) = 0;
	input(0, 1) = 1;

	Matrix<float> label(1, 2);
	label(0, 0) = 0;
	label(0, 1) = 1;
	*/

	// TODO: Shit, looks like the backprop algo I wrote only works if all neurons are the same size!

	Input<float>  in(&input);
	Dense<float>  d1(2, false, &in, Activation::Relu);
	Dense<float>  d2(2, false, &d1, Activation::Relu);
	Dense<float>  d3(2, false, &d2, Activation::SoftMax);


	Model<float> mod(in, &d3, 0.1, ErrorFunction::Squared);
	
	//Matrix<float> m1(label.rows(), label.columns());

	while(true)
		mod.train(input, label);



	return 0;
}