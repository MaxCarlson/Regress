#pragma once

enum Activation
{
	Relu,
	SoftMax,
	Sigmoid
};

template<class> class Matrix;

void activationFunction(Activation act, Matrix<float>& m);
