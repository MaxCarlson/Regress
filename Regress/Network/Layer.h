#pragma once
#include "Activation.h"
#include "Linear\Matrix.h"
#include <vector>

template<class> class Dense;
template<class> class Input;
template<class> class Output;


template<class T>
class Layer
{
protected:
	
	Activation			activation;
	std::vector<Layer*> inputs;
	std::vector<Layer*> outputs;

public:
	Layer();
	Layer(Activation activation);

	virtual void feedForward(Matrix<T>& input) {}
	virtual int numNodes() const { return 0; }

	friend Dense<T>;
	friend Input<T>;
	friend Output<T>;
};

template<class T>
inline Layer<T>::Layer() :
	activation{ Activation::None },
	inputs{},
	outputs{}
{
}

template<class T>
inline Layer<T>::Layer(Activation activation) : 
	activation{ activation },
	inputs{},
	outputs{}
{
}
