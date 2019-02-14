#pragma once
#include "Layer.h"

template<class T>
class Output : public Layer<T>
{
	Matrix<T> output;
public:

	// TODO: numOutputs needs to be a AxBx...N dimensions
	Output(int numOutputs, Activation activation);
	void feedForward(Matrix<T>& input);
	int numNodes() const { return output.columns(); }
};

template<class T>
inline Output<T>::Output(int numOutputs, Activation activation) : Layer<T>(activation),
	output{}
{
}

template<class T>
inline void Output<T>::feedForward(Matrix<T>& input) 
{
}
