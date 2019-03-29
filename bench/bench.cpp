#include "../Linear/Matrix.h"
#include "../Linear/Stopwatch.h"

struct BaseBench
{
	static void start() { s.start(); }
	static void stop() { s.stop(); }
	static void print() { s.printCurrent(); }
	inline static Stopwatch<std::chrono::milliseconds> s;

	template<class Func, class... Args>
	static void runFunc(Func&& func, Args ...args)
	{
		s.start();
		func(std::forward<Args>(args)...);
		s.printCurrent();
	}
};

#define RUN_FUNC(func, subname, ...) std::cout << func << ' ' << subname << ' '; \
	BaseBench::start(); \
	func(##__VA_ARGS__); \
	BaseBench::print() \

template<class Type, bool MajorOrder>
struct basic : public BaseBench
{
	using Mat = Matrix<Type, MajorOrder>;

	static void run()
	{
		//RUN_FUNC(doOther, 1);
		RUN_FUNC(mulSquareAlias, "100", 100);
	}

	static void mulSquareAlias(int size)
	{
		Mat m(size, size);
		m = m * m;
	}
};

int main()
{
	basic<int, false>::run();
	return 0;
}