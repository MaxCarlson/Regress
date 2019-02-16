#pragma once
#include "Layer.h"

template<class T>
class Dense : public Layer<T>
{
	int neurons;
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
	Matrix<T> net;
	Matrix<T> output;
	Matrix<T> deltas;
	Matrix<T> actPrime;

public:

	// TODO: If bias is true, we need to add a dimension of 1's to the input layer

	Dense(int neurons, bool bias, Layer<T>* input, Activation activation);

	void		feedForward(Matrix<T>& input);
	int			numNodes() const { return neurons; }
	Matrix<T>*	getOutput() { return &output; }
	Activation	getActivation() const { return this->activation; }
	void		calcDeltas(Matrix<T>& deltaIn, Matrix<T>& target, bool outputLayer);
};

#include <random>

template<class T>
inline Dense<T>::Dense(int neurons, bool bias, Layer<T>* input, Activation activation) : Layer<T>(activation),
	neurons{		neurons						},
	bias{			bias						},
	weights{},
	net{},
	output{},
	deltas{},
	actPrime{}
{
	// TODO: Randomize weight init values
	if (input)
	{
		input->outputs.emplace_back(static_cast<Layer<T>*>(this));
		this->inputs.emplace_back(input);
		weights.resize(input->numNodes() + bias, neurons);
	}
	else
		throw std::runtime_error("Dense layer needs an input!");

	weights.unaryExpr([](T& v)
	{
		// TODO: Optimize
		static auto dis = std::uniform_real_distribution<T>();
		static auto re = std::default_random_engine{};
		v = dis(re);
	});
}

template<class T>
inline void Dense<T>::feedForward(Matrix<T>& input)
{
	net		= input * weights;
	output	= net;

	activationFunction(this->activation, output);

	
	// Store the derivative of the activation function
	// applied to layers output (used in backprop to calculate deltas)
	//actPrime = output;
	//activationPrime(this->activation, actPrime);

	
	for (Layer<T>*& outs : this->outputs)
		outs->feedForward(output);
}

template<class T>
inline void Dense<T>::calcDeltas(Matrix<T>& deltaIn, Matrix<T>& target, bool outputLayer)
{
	if (outputLayer)
	{
//		deltas = -(target - output);
		deltas.cwiseProduct(output.cwiseProduct(1 - output));
	}
}



