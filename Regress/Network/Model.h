#pragma once
#include "InputLayer.h"
#include "OutputLayer.h"

template<class T>
class Model
{
	Input<T>&	input;
	Output<T>&	output;
public:
	Model(Input<T>& input, Output<T>& output);
};