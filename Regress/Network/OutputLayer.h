#pragma once
#include "Layer.h"
/*
template<class T>
class Output : public Layer<T>
{
	Matrix<T> output;
public:

	// TODO: numOutputs needs to be a AxBx...N dimensions
	Output(int numOutputs, Layer* input, Activation activation);
	void feedForward(Matrix<T>& input);
	int numNodes() const { return output.columns(); }
};

template<class T>
inline Output<T>::Output(int numOutputs, Layer<T>* input, Activation activation) : Layer<T>(activation),
	output{}
{
	
	if (input)
	{
		input->outputs.emplace_back(static_cast<Layer<T>*>(this));
		this->inputs.emplace_back(input);
		output.resize(input->numNodes(), neurons);
	}

}

template<class T>
inline void Output<T>::feedForward(Matrix<T>& input) 
{
}
*/
