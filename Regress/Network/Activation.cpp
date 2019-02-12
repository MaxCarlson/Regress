#include "Activation.h"
#include "Linear\Matrix.h"

template<class T>
void reluFunc(Matrix<T>& m)
{

}

template<class T>
void activationFunction(Activation act, Matrix<T>& m)
{
	switch (act)
	{
	case Relu:
		reluFunc(m);
		break;
	case SoftMax:
		break;
	case Sigmoid:
		break;
	default:
		throw std::runtime_error("Invalid Activation function");
	}
}

void activationFunction(Activation act, Matrix<float>& m)
{
	activationFunction<float>(act, m);
}
