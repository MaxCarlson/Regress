#include "Activation.h"
#include "Linear\Matrix.h"
#include <algorithm>

template<class T>
void reluFunc(Matrix<T>& m)
{
	m.unaryExpr([](T& i)
	{
		i = std::max<T>(0, i);
	});

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
