#pragma once
#include "InputLayer.h"
#include "OutputLayer.h"
#include "ErrorFuncs.h"
#include "InfoPrinter.h"

template<class T>
class Model
{
	Input<T>&		input;
	Layer<T>*		output;
	double			learningRate;
	double			error;
	ErrorFunction	errorFunc;
	Matrix<T>		merror;			// The error for each output neuron
	InfoPrinter		printer;

	void calcError(Matrix<T>& labels);
	void backprop(Matrix<T>& labels);

public:
	Model(Input<T>& input, Layer<T>* output, double lr, ErrorFunction error, InfoPrinter printer);

	void run(Matrix<T>& features);
	void train(Matrix<T>& features, Matrix<T>& labels);

	void feedForward();
};

template<class T>
inline void Model<T>::calcError(Matrix<T>& labels)
{
	error = calculateError(merror, labels, *output->getOutput(), errorFunc);
}

template<class T>
inline void Model<T>::backprop(Matrix<T>& labels)
{
	output->calcDeltasOutput(labels, errorFunc, learningRate);
}

template<class T>
inline Model<T>::Model(Input<T>& input, Layer<T>* output, double lr, ErrorFunction errorFunc, InfoPrinter printer) :
	input{ input },
	output{ output },
	learningRate{ lr },
	error{ 0.0 },
	errorFunc{ errorFunc },
	merror{},
	printer{ printer }
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
	merror.resize(labels.rows(), labels.cols());

	input.feedForward();
	calcError(labels);
	backprop(labels);

	printer.printInfo(error);
}

template<class T>
inline void Model<T>::feedForward()
{
	input.feedForward();
}
