#pragma once
#include "Layer.h"

template<class T>
class Input : public Layer<T>
{
	// Inputs must be ordered like so:
	// sample1 { f1, f2, ..., fn }
	// sample2 { f1, f2, ..., fn }
	// samplen { f1, f2, ..., fn }
	// Where f is a feature

	Matrix<T>* input;

public:
	Input(Matrix<T>* input);
	void feedForward();
	int numNodes() const { return input->columns(); }
	void setInput(Matrix<T>* newInput) { input = newInput; }
	Matrix<T>* getNet() { return input; }

};

template<class T>
inline Input<T>::Input(Matrix<T>* input) :
	input{ input }
{}

template<class T>
inline void Input<T>::feedForward()
{
	if (!input)
		throw std::runtime_error("No input set during feedForward");

	for (Layer<T>*& outs : this->outputs)
		outs->feedForward(*input);
}
