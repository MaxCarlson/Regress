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
	Matrix<T> errors;
	Matrix<T> deltas;
	Matrix<T> actPrime;

public:

	// TODO: If bias is true, we need to add a dimension of 1's to the input layer

	Dense(int neurons, bool bias, Layer<T>* input, Activation activation);

	void		feedForward(Matrix<T>& input);

	int			numNodes()		const	{ return neurons; }
	Matrix<T>*	getOutput()				{ return &output; }
	Matrix<T>*	getWeights()			{ return &weights; }
	Activation	getActivation() const	{ return this->activation; }

	void		calcDeltasOutput(Matrix<T>& target, ErrorFunction errorFunc);
	void		calcDeltas(Matrix<T>& outWeights, Matrix<T>& outErrors, Matrix<T>& outActPrime, Matrix<T>& target);
};

#include <random>

template<class T>
inline Dense<T>::Dense(int neurons, bool bias, Layer<T>* input, Activation activation) : Layer<T>(activation),
	neurons{		neurons						},
	bias{			bias						},
	weights{},
	net{},
	output{},
	errors{},
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
	actPrime = output;
	activationPrime(this->activation, actPrime);

	
	for (Layer<T>*& outs : this->outputs)
		outs->feedForward(output);
}

template<class T>
inline void Dense<T>::calcDeltasOutput(Matrix<T>& target, ErrorFunction errorFunc)
{
	// Working for stoiciastic gradient descent, but not batch!!
	// TODO: Make errors only availible in output layers!

	errorPrime(errors, target, output, errorFunc); 

	deltas = errors;
	deltas.cwiseProduct(actPrime);

	// Deltas calculated
	// changing weights here for test
	Matrix<T>* prevOutput = this->inputs[0]->getOutput();

	deltas = deltas.transpose();
	deltas *= *prevOutput;
	deltas = deltas.transpose();


	for (auto& in : this->inputs)
		in->calcDeltas(weights, errors, actPrime, target);

	// Update weights. Should probably be done somewhere else of function renamed
	//weights = weights - deltas;
}


// Note outErrors are actually outErrorPrime's
template<class T>
inline void Dense<T>::calcDeltas(Matrix<T>& outWeights, Matrix<T>& outErrors, Matrix<T>& outActPrime, Matrix<T>& target)
{
	// Instead of error (like it is in output node) 
	// this is actually the partial derivative of Etotal with respect to (output)s
	errors = outWeights * outErrors.transpose();

	errors = errors.cwiseProduct(actPrime.transpose());

	//errors = errors.transpose();
	errors *= *this->inputs[0]->getOutput();

	weights = weights - errors;
}



