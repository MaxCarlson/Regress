#pragma once
#include "InputLayer.h"
#include "OutputLayer.h"
#include "ErrorFuncs.h"

template<class T>
class Model
{
	Input<T>&	input;
	Layer<T>*	output;
	double		learningRate;
	Error		errorFunction;
public:
	Model(Input<T>& input, Layer<T>* output, double lr, Error error);

	void run(Matrix<T>& features);
	void train(Matrix<T>& features, Matrix<T>& labels);

	void feedForward();
};

template<class T>
inline Model<T>::Model(Input<T>& input, Layer<T>* output, double lr, Error error) :
	input{ input },
	output{ output },
	learningRate{ lr },
	errorFunction{ error }
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

}

template<class T>
inline void Model<T>::feedForward()
{
	input.feedForward();
}
