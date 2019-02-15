#pragma once
#include "InputLayer.h"
#include "OutputLayer.h"
#include "ErrorFuncs.h"

template<class T>
class Model
{
	Input<T>&		input;
	Layer<T>*		output;
	double			learningRate;
	double			error;
	ErrorFunction	errorFunc;

	void calcError(Matrix<T>& labels);

public:
	Model(Input<T>& input, Layer<T>* output, double lr, ErrorFunction error);

	void run(Matrix<T>& features);
	void train(Matrix<T>& features, Matrix<T>& labels);

	void feedForward();
};

template<class T>
inline void Model<T>::calcError(Matrix<T>& labels)
{
	Matrix<T> merror(labels.rows(), labels.columns());
	error = calculateError(merror, labels, *output->getOutput(), errorFunc);
	output->calcDeltas(merror, true);
	
	activationPrime(output->getActivation(), *output->getOutput());
}

template<class T>
inline Model<T>::Model(Input<T>& input, Layer<T>* output, double lr, ErrorFunction errorFunc) :
	input{ input },
	output{ output },
	learningRate{ lr },
	error{ 0.0 },
	errorFunc{ errorFunc }
{

}

template<class T>
inline void Model<T>::run(Matrix<T>& features)
{
	input.setInput(&features);
}

template<class T>
inline void Model<T>::train(Matrix<T>& features, Matrix<T>& labels)
{
	input.feedForward();
	calcError(labels);
}

template<class T>
inline void Model<T>::feedForward()
{
	input.feedForward();
}
