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
	// TODO: This vec of errors might not be needed, we'll see
	Matrix<T> m(labels.rows(), labels.columns());
	error = calculateError(m, labels, *output->getOutput(), errorFunc);
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
