#pragma once
#include "Layer.h"
#include "Activation.h"

template<class T>
class Dense : public Layer<T>
{
	int neurons;
	int inputNodes; // Number of features
	bool bias;

	// Input to a layer looks like so:
	// Where sn is the sample and in the row are it's features
	//
	// s1 { f1, f2, ..., fn }
	// s2 { f1, f2, ..., fn }
	// ...
	// sn { f1, f2, ..., fn }
	//

	// Weights are stored like so:
	// Where Nn is the neuron and it's weights
	// for inputs 1-n are all in a column
	//    N1,  N2,  N3,  N4, ...,  Nn 
	// { wi1, wi1, wi1, wi1, ..., wi1 }
	// { wi2, wi2, wi2, wi2, ..., wi2 }
	// { ..., ..., ..., ..., ..., ... }
	// { win, win, win, win, ..., win }

	Matrix<T> weights;
	Matrix<T> output;
	//Matrix<T> deltas;

	Activation activation;


public:

	Dense(int neurons, int inputNodes, bool bias, Layer<T>* input, Activation activation);

	void feedForward(Matrix<T>& input);
};

#include <random>

template<class T>
inline Dense<T>::Dense(int neurons, int inputNodes, bool bias, Layer<T>* input, Activation activation) : Layer<T>(),
	neurons{		neurons						},
	inputNodes{		inputNodes					},
	bias{			bias						},
	weights(		inputNodes + bias, neurons	),
	output{										},
	activation{		activation					}
{
	// TODO: Randomize weight init values
	if (input)
	{
		input->outputs.emplace_back(static_cast<Layer<T>*>(this));
		this->inputs.emplace_back(input);
	}

	weights.unaryExpr([](T& v)
	{
		static auto dis = std::uniform_real_distribution<T>();
		static auto re = std::default_random_engine{};
		v = dis(re);
	});
}

template<class T>
inline void Dense<T>::feedForward(Matrix<T>& input)
{
	// TODO: Make sure we're not reallocating space for output each time
	output = input * weights;

	activationFunction(activation, output);
	
	for (Layer<T>*& outs : this->outputs)
		outs->feedForward(output);
}

