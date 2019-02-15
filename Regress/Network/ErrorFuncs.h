#pragma once
#include "Linear\Matrix.h"

enum ErrorFunction
{
	Squared,
	BinaryCrossEntropy,
	CrossEntropyWithSoftmax
};

template<class T>
double squaredError(Matrix<T>& merror, const Matrix<T>& target, const Matrix<T>& output)
{
	merror = (target - output);
	merror.unaryExpr([](T& i)
	{
		i = std::pow(i, 2);
	});
	merror *= 0.5;

	return merror.sum();
}

template<class T>
double squaredPrime(Matrix<T>& pd, const Matrix<T>& target, const Matrix<T>& output)
{
	pd = output - target;
}

template<class T>
double calculateError(Matrix<T>& merror, const Matrix<T>& target, const Matrix<T>& output, ErrorFunction error)
{
	switch (error)
	{
	case Squared:
		return squaredError(merror, target, output);
	case BinaryCrossEntropy:
		break;
	case CrossEntropyWithSoftmax:
		break;
	default:
		break;
	}
}

template<class T>
void partialErrorDerivative(Matrix<T>& pd, const Matrix<T>& target, const Matrix<T>& output, ErrorFunction error)
{
	switch (error)
	{
	case Squared:
		squaredPrime(pd, target, output);
		break;
	case BinaryCrossEntropy:
		break;
	case CrossEntropyWithSoftmax:
		break;
	default:
		break;
	}
}