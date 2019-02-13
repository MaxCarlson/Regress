#pragma once

enum Activation
{
	None,
	Relu,
	SoftMax,
	Sigmoid
};

template<class> class Matrix;

void activationFunction(Activation act, Matrix<float>& m);
