#pragma once
#include "Linear\Matrix.h"
#include "Activation.h"

template<class T>
class Dense
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
	//Matrix<T> deltas;

	Dense* input;
	Dense* output; 

	Activation activation;


public:
	Dense(int neurons, int inputNodes, int outputNodes, bool bias, Dense* input, Activation activation);
};

template<class T>
inline Dense<T>::Dense(int neurons, int inputNodes, int outputNodes, bool bias, Dense* input, Activation activation) :
	neurons{		neurons						},
	inputNodes{		inputNodes					},
	outputNodes{	outputNodes					},
	bias{			bias						},
	weights(		neurons, inputNodes + bias	),
	input{			input						},
	output{			nullptr						},
	activation{		activation					}
{
	// TODO: Randomize weight init values
	if (input)
	{
		input->output = this;
	}
}

