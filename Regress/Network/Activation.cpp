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
void reluPrime(Matrix<T>& m)
{
	m.unaryExpr([](T& i)
	{
		i = i >= 0 ? 1 : 0;
	});
}

template<class T>
void sigmoidFunc(Matrix<T>& m)
{

}

template<class T>
void sigmoidPrime(Matrix<T>& m)
{

}

template<class T>
void activationFunction(Activation act, Matrix<T>& m)
{
	switch (act)
	{
	case None:
		break;
	case Relu:
		reluFunc(m);
		break;
	case SoftMax:
		break;
	case Sigmoid:
		sigmoidFunc(m);
		break;
	default:
		throw std::runtime_error("Invalid Activation function");
	}
}

template<class T>
void activationPrime(Activation act, Matrix<T>& m)
{
	switch (act)
	{
	case None:
		break;
	case Relu:
		reluPrime(m);
		break;
	case SoftMax:
		break;
	case Sigmoid:
		sigmoidPrime(m);
		break;
	default:
		throw std::runtime_error("Invalid Activation function");
	}
}

void activationFunction(Activation act, Matrix<float>& m)
{
	activationFunction<float>(act, m);
}

void activationPrime(Activation act, Matrix<float>& m)
{
	activationPrime<float>(act, m);
}
