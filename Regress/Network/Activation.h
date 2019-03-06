#pragma once

enum Activation
{
	None,
	Relu,
	SoftMax,
	Sigmoid
};

template<class, bool = false> 
class Matrix;

namespace ActivationImpl
{
template<class T>
inline void reluFunc(Matrix<T>& m)
{
	m.unaryExpr([](T& i)
	{
		i = std::max<T>(0, i);
	});

}

template<class T>
inline void reluPrime(Matrix<T>& m)
{
	m.unaryExpr([](T& i)
	{
		i = i >= 0 ? 1 : 0;
	});
}

template<class T>
inline void sigmoidFunc(Matrix<T>& m)
{

}

template<class T>
inline void sigmoidPrime(Matrix<T>& m)
{

}
}


template<class T>
inline void activationFunction(Activation act, Matrix<T>& m)
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

template<class T>
inline void activationPrime(Activation act, Matrix<T>& m)
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
