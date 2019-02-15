#include "Linear\Matrix.h"
#include "Network\Model.h"
#include "Network\Dense.h"
#include "Network\InputLayer.h"
#include "Linear\MatrixOp.h"

struct BaseB
{
	
};

template<class T>
struct FF : public BaseB
{
	FF(T t) : t{t}
	{}
	T t;
};

int main()
{
	/*
	Matrix<float> m1( 2, 2 );
	Matrix<float> m2( 2, 2 );

	m1(0, 0) = 1;
	m1(0, 1) = 2;
	m1(1, 0) = 3;
	m1(1, 1) = 4;

	m2(0, 0) = 4;
	m2(0, 1) = 3;
	m2(1, 0) = 2;
	m2(1, 1) = 1;

	auto m3 = m1 * m2;
	*/

	// Training bitwise OR ( | ) operation

	std::vector<BaseB*> bb;
	bb.emplace_back(new FF<float>(.1f));
	bb.emplace_back(new FF<double>(0.0));


	Matrix<float> input(1, 2);
	input(0, 0) = 0;
	input(0, 1) = 1;

	Matrix<float> label(1, 2);
	label(0, 0) = 0;
	label(0, 1) = 1;


	Input<float>  in(&input);
	Dense<float>  d1(3, false, &in, Activation::Relu);
	Dense<float>  d2(2, false, &d1, Activation::SoftMax);


	Model<float> mod(in, &d2, 0.1, ErrorFunction::Squared);
	
	//Matrix<float> m1(label.rows(), label.columns());

	mod.train(input, label);


	return 0;
}