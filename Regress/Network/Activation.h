#pragma once

enum Activation
{
	None,
	Relu,
	SoftMax,
	Sigmoid
};

template<class, bool> 
class Matrix;

namespace ActivationImpl
{
template<class T, bool MajorOrder>
inline void reluFunc(Matrix<T, MajorOrder>& m)
{
	m.unaryExpr([](T& i)
	{
		i = std::max<T>(0, i);
	});

}

template<class T, bool MajorOrder>
inline void reluPrime(Matrix<T, MajorOrder>& m)
{
	m.unaryExpr([](T& i)
	{
		i = i >= 0 ? 1 : 0;
	});
}

template<class T, bool MajorOrder>
inline void sigmoidFunc(Matrix<T, MajorOrder>& m)
{

}

template<class T, bool MajorOrder>
inline void sigmoidPrime(Matrix<T, MajorOrder>& m)
{

}
}


template<class T, bool MajorOrder>
inline void activationFunction(Activation act, Matrix<T, MajorOrder>& m)
{
	switch (act)
	{
	case None:
		break;
	case Relu:
		ActivationImpl::reluFunc(m);
		break;
	case SoftMax:
		break;
	case Sigmoid:
		ActivationImpl::sigmoidFunc(m);
		break;
	default:
		throw std::runtime_error("Invalid Activation function");
	}
}

template<class T, bool MajorOrder>
inline void activationPrime(Activation act, Matrix<T, MajorOrder>& m)
{
	switch (act)
	{
	case None:
		break;
	case Relu:
		ActivationImpl::reluPrime(m);
		break;
	case SoftMax:
		break;
	case Sigmoid:
		ActivationImpl::sigmoidPrime(m);
		break;
	default:
		throw std::runtime_error("Invalid Activation function");
	}
}
