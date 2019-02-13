#pragma once
#include "Linear\Matrix.h"
#include <vector>

template<class> class Dense;
template<class> class Input;


template<class T>
class Layer
{
protected:
	std::vector<Layer*> inputs;
	std::vector<Layer*> outputs;

public:
	Layer() = default;

	virtual void feedForward(Matrix<T>& input) {};

	friend Dense<T>;
	friend Input<T>;
};


