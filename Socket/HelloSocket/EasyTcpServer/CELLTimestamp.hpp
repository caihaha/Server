#ifndef CELLTimestamp_hpp
#define CELLTimestamp_hpp

#include <chrono>
using namespace std::chrono;

class CELLTimestamp
{
public:
	CELLTimestamp()
	{
		Update();
	}

	~CELLTimestamp()
	{
	}

	void Update()
	{
		_begin = high_resolution_clock::now();
	}

	double GetElapsedSecond()
	{
		return GetElapsedMicroSec() * 0.000001;
	}

	double GetElapsedMilliSec()
	{
		return GetElapsedMicroSec() * 0.001;
	}

	long long GetElapsedMicroSec()
	{
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
	}

protected:
	time_point<high_resolution_clock> _begin;
};


#endif // CELLTimestamp_hpp
