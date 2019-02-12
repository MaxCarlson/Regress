#pragma once
#include <vector>

template<class> class Dense;

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
};


