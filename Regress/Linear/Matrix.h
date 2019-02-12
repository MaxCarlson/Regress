
template<class T>
class Matrix
{
	int rows;
	int columns;
public:
	Matrix(int rows, int columns) :
		rows{rows},
		columns{columns}
	{}

	friend Matrix operator+(const Matrix& m1, const Matrix& m2);
	friend Matrix operator*(const Matrix& m1, const Matrix& m2);
};