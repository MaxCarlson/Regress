#pragma once
#include <vector>

template<class T>
class Layer
{
protected:
	std::vector<Layer*> inputs;
	std::vector<Layer*> outputs;
public:
	Layer();
	Layer(Layer* input);
};

template<class T>
inline Layer<T>::Layer()
{
}

template<class T>
inline Layer<T>::Layer(Layer * input) :
	inputs{ input }
{
}
