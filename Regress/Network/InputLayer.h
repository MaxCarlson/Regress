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

	Matrix<T>& input;

public:
	Input(Matrix<T>& input);
	void feedForward();
	int numNodes() const { return input.columns(); }
};

template<class T>
inline Input<T>::Input(Matrix<T>& input) :
	input{ input }
{}

template<class T>
inline void Input<T>::feedForward()
{

}
