#pragma once
#include "Activation.h"
#include "ErrorFuncs.h"
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
	std::vector<Layer*> inputs;		// TODO: Probably dont need multiple inputs? That would be handled by a layer by itself?
	std::vector<Layer*> outputs;

public:
	Layer();
	Layer(Activation activation);
	virtual ~Layer() {}

	virtual void		feedForward(Matrix<T>& input) {}
	virtual int			numNodes() const { return 0; }
	virtual Matrix<T>*	getNet()		{ return nullptr; }
	virtual Matrix<T>*	getOutput()		{ return nullptr; }
	virtual Matrix<T>*	getWeights()	{ return nullptr; }
	virtual Activation	getActivation() { return Activation::None; }
	virtual void		calcDeltasOutput(Matrix<T>& target, ErrorFunction errorFunc) {}
	virtual void		calcDeltas(Matrix<T>& outWeights, Matrix<T>& outErrors, Matrix<T>& outActPrime, Matrix<T>& target) {}

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
