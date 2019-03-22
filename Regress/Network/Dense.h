#pragma once
#include "Layer.h"

template<class T>
class Dense : public Layer<T>
{
	int neurons;
	bool bias;

	// Input to a layer looks like so:
	// Where sn is the sample# and in the row are it's features
	//
	// s1 { f1, f2, ..., fn }
	// s2 { f1, f2, ..., fn }
	// ...
	// sn { f1, f2, ..., fn }
	//

	// Weights are stored like so:
	// Where Nn is the neuron and its weights
	// for inputs 1-n are all in a column
	//    N1,  N2,  N3,  N4, ...,  Nn, B(if bias)
	// { wi1, wi1, wi1, wi1, ..., wi1, b1 }
	// { wi2, wi2, wi2, wi2, ..., wi2, b2 }
	// { ..., ..., ..., ..., ..., ...  ...}
	// { win, win, win, win, ..., win, bn }

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

	int			numNodes()		const	{ return neurons;	}
	Matrix<T>*	getNet()				{ return &net;		}
	Matrix<T>*	getOutput()				{ return &output;	}
	Matrix<T>*	getWeights()			{ return &weights;	}
	Activation	getActivation() const	{ return this->activation; }

	void		calcDeltasOutput(Matrix<T>& target, ErrorFunction errorFunc, double lr);
	void		calcDeltas(Matrix<T>& outWeights, Matrix<T>& outErrors, Matrix<T>& outActPrime, double lr);
	void updateWeights(double lr);
};

#include <random>

template<class T>
inline Dense<T>::Dense(int neurons, bool bias, Layer<T>* input, Activation activation) : Layer<T>(activation),
	neurons{ neurons },
	bias{ bias },
	weights{},
	net{},
	output{},
	errors{},
	deltas{},
	actPrime{}
{
	if (input)
	{
		input->outputs.emplace_back(static_cast<Layer<T>*>(this));
		this->inputs.emplace_back(input);
		weights.resize(input->numNodes(), neurons + bias);
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
inline void Dense<T>::calcDeltasOutput(Matrix<T>& target, ErrorFunction errorFunc, double lr)
{
	errorPrime(errors, target, output, errorFunc); 
	
	const auto& prevOutput = *this->inputs[0]->getOutput();
	deltas = prevOutput.transpose() * errors.cwiseProduct(actPrime);

	for (auto& in : this->inputs)
		in->calcDeltas(weights, errors, actPrime, lr);

	updateWeights(lr);
}


// Note: outErrors are actually outErrorPrime's
template<class T>
inline void Dense<T>::calcDeltas(Matrix<T>& outWeights, Matrix<T>& outErrors, Matrix<T>& outActPrime, double lr)
{
	// Instead of error (like it is in output node) 
	// this is actually the partial derivative of Etotal with respect to (output)s
	
	// In order to get the deltas here we do
	// dEtotal/dout * dout/dnet * dnet/dout
	// where
	// dEtotal/dout = dE_o1/out_h1 + dE_o2/out_h1 + ... dE_on/out_h1
	// TODO: Not done commenting
	
	const auto& inNet = *this->inputs[0]->getNet();

	errors = outErrors * outWeights.transpose();
	deltas = errors.cwiseProduct(actPrime);

	// TODO: Get transpose operator working with expressions
	deltas = deltas.transpose() * inNet;	
	deltas = deltas.transpose();
	

	for (auto& in : this->inputs)
		in->calcDeltas(weights, errors, actPrime, lr);

	updateWeights(lr);
}

template<class T>
inline void Dense<T>::updateWeights(double lr)
{
	//
	// TODO: Implement -=
	//
	//weights -= deltas * lr;
}



