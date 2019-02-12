#pragma once
#include "Layer.h"
#include "Activation.h"
#include "Linear\Matrix.h"

template<class T>
class Dense : public Layer<T>
{
	int neurons;
	int inputNodes;
	int outputNodes;
	bool bias;

	// Weights are stored like so
	// where iN is the weight for input node N
	// neuron1 { i1, i2, i3, ..., iN }
	// neuron2 { i1, i2, i3, ..., iN }
	// neuron..{ i1, i2, i3, ..., iN }
	// neuronN { i1, i2, i3, ..., iN }

	Matrix<T> weights;
	Matrix<T> output;
	//Matrix<T> deltas;

	Activation activation;


public:

	Dense(int neurons, int inputNodes, int outputNodes, bool bias, Layer<T>* input, Activation activation);

	void feedForward(Matrix<T>& input);
};

template<class T>
inline Dense<T>::Dense(int neurons, int inputNodes, int outputNodes, bool bias, Layer<T>* input, Activation activation) : Layer<T>(),
	neurons{		neurons						},
	inputNodes{		inputNodes					},
	outputNodes{	outputNodes					},
	bias{			bias						},
	weights(		neurons, inputNodes + bias	),
	output{										},
	activation{		activation					}
{
	// TODO: Randomize weight init values
	if (input)
	{
		input->outputs.emplace_back(static_cast<Layer<T>*>(this));
		this->inputs.emplace_back(input);
	}
}

template<class T>
inline void Dense<T>::feedForward(Matrix<T>& input)
{
	// TODO: Make sure we're not reallocating space for outut each time
	output = input * weights;
	
	for (Layer<T>*& outs : this->outputs)
		outs->feedForward(output);
}

