#pragma once
#include "Linear\Matrix.h"

enum Error
{
	Squared,
	BinaryCrossEntropy,
	CrossEntropyWithSoftmax
};

template<class T>
double squaredError(Matrix<T>& merror, const Matrix<T>& target, const Matrix<T>& output)
{
	merror = (target - output);
	return merror.sum();
}

template<class T>
double calculateError(Matrix<T>& merror, const Matrix<T>& target, const Matrix<T>& output, Error error)
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