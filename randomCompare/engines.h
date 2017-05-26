#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <vector>


//for storing test times
struct Results
{
	std::string distribution = "";
	long long int fastest = 0;
	long long int slowest = 0;
	long long int mean = 0;
	std::string total = "";

};


class BaseTest
{

public:
	virtual ~BaseTest() = default;
	virtual void operator()() = 0;
	virtual const std::string& getDesc() const = 0;
	virtual std::string getResults() const = 0;
	
protected:
	static int& level; //reference to global command arg settable value
	static int& iterations; //reference to global command arg settable value
};



template<typename T = std::minstd_rand>
class EngineTest final : public BaseTest
{
public:
	EngineTest(std::string d) { _desc = d;
		_dist1.param(std::uniform_int<int>::param_type(0, 9));
		_dist2.param(std::normal_distribution<float>::param_type(0, 1));
		_dist3.param(std::bernoulli_distribution::param_type(.5));
		_results[0].distribution = "none";
		_results[1].distribution = "Uniform Integer";
		_results[2].distribution = "Normal";
		_results[3].distribution = "Bernoulli";}
	~EngineTest() {}
	
	virtual void operator()() override { this->runTest(); }

	virtual const std::string& getDesc() const override { return _desc; }

	//format _results and return
	virtual std::string getResults() const override;

	//format settings and return
	static std::string getSettings();

private:
	void runTest();

	//formats accumulated time into _results[].total as mm:ss.msec
	void convertResultsTotal(int resultsIndex);
	//creates proper mean
	void convertResultsMean(int resultsIndex);

	static std::chrono::steady_clock _steadyClock;
	std::chrono::steady_clock::time_point _start;
	std::chrono::steady_clock::time_point _stop;

	T _eng;
	std::uniform_int_distribution<int> _dist1;
	std::normal_distribution<float> _dist2;
	std::bernoulli_distribution _dist3;

	std::string _desc;
	std::array<Results, 4> _results;

};

template<typename T>
void EngineTest<T>::runTest() 
{
	bool initialized = false;
	switch (level)
	{
	case 1:
		std::cout << ">Starting test for: " << _desc << "...";
		for (int i = 0; i < iterations; i++) {
			
			_start = _steadyClock.now();
			_eng();
			_stop = _steadyClock.now();
			long long int thisIt = std::chrono::duration_cast<std::chrono::nanoseconds>(_stop - _start).count();
			if (!initialized)
			{
				_results[0].fastest = thisIt;
				initialized = true;
			}
			//keep track of fastest and slowest
			if (_results[0].fastest > thisIt)
				_results[0].fastest = thisIt;
			if (_results[0].slowest < thisIt)
				_results[0].slowest = thisIt;
			_results[0].mean += thisIt; //keep accumulating total time inside mean
			
		}

		convertResultsTotal(0); //pass control to convert time accumulated in mean
		convertResultsMean(0); //convert mean to proper mean
		std::cout << "done!\n";

		break;
	case 2:
		std::cout << ">Starting test for: " << _desc << '\n';
		for (int x = 1; x < 4; x++) { //for each distribution starting at _results[1] to _results[4] as x
			std::cout << ">" << x << "/3...";
			for (int i = 0; i < iterations; i++) { //run test for x
				
				switch (x) {
				case 1:
					_start = _steadyClock.now();
					_dist1(_eng);
					_stop = _steadyClock.now();
					break;
				case 2:
					_start = _steadyClock.now();
					_dist2(_eng);
					_stop = _steadyClock.now();
					break;
				case 3:
					_start = _steadyClock.now();
					_dist3(_eng);
					_stop = _steadyClock.now();
					break;
				}
				long long int thisIt = std::chrono::duration_cast<std::chrono::nanoseconds>(_stop - _start).count();
				if (!initialized)
				{
					_results[x].fastest = thisIt;
					initialized = true;
				}
				if (_results[x].fastest > thisIt)
					_results[x].fastest = thisIt;
				if (_results[x].slowest < thisIt)
					_results[x].slowest = thisIt;
				_results[x].mean += thisIt;

			}
			convertResultsTotal(x);
			convertResultsMean(x);
			std::cout << "done!\n";
		}

		break;
	case 3:
		std::cout << ">Starting test for: " << _desc << '\n';
		for (int x = 1; x < 4; x++) { //for each distribution starting at _results[1] to _results[4] as x
			std::cout << ">" << x << "/3...";		
			//if test is x, resize apropriate array to right size
			std::vector<int> intVec;
			if(x==1)
				intVec.resize(iterations);
			std::vector<float> floatVec;
			if(x==2)
				floatVec.resize(iterations);
			std::vector<bool> boolVec;
			if(x==3)
				boolVec.resize(iterations);
			for (int i = 0; i < iterations; i++) { //run test for x

				switch (x) {
				case 1:
					_start = _steadyClock.now();
					intVec[i] = _dist1(_eng);
					_stop = _steadyClock.now();
					break;
				case 2:
					_start = _steadyClock.now();
					floatVec[i] = _dist2(_eng);
					_stop = _steadyClock.now();
					break;
				case 3:
					_start = _steadyClock.now();
					boolVec[i] = _dist3(_eng);
					_stop = _steadyClock.now();
					break;
				}
				long long int thisIt = std::chrono::duration_cast<std::chrono::nanoseconds>(_stop - _start).count();
				if (!initialized)
				{
					_results[x].fastest = thisIt;
					initialized = true;
				}
				if (_results[x].fastest > thisIt)
					_results[x].fastest = thisIt;
				if (_results[x].slowest < thisIt)
					_results[x].slowest = thisIt;
				_results[x].mean += thisIt;

			}
			convertResultsTotal(x);
			convertResultsMean(x);
			std::cout << "done!\n";
		}		
		break;
	}
}


template<typename T>
void EngineTest<T>::convertResultsTotal(int resultsIndex)
{
	std::chrono::milliseconds steady_result(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds (_results[resultsIndex].mean)));

	std::chrono::minutes mm = std::chrono::duration_cast<std::chrono::minutes>(steady_result % std::chrono::hours(1));
	std::chrono::seconds ss = std::chrono::duration_cast<std::chrono::seconds>(steady_result % std::chrono::minutes(1));
	std::chrono::milliseconds msec = std::chrono::duration_cast<std::chrono::milliseconds>(steady_result % std::chrono::seconds(1));

	std::stringstream stream;
	stream << std::setfill('0')
		<< std::setw(2) << mm.count() << ":"
		<< std::setw(2) << ss.count() << "."
		<< std::setw(3) << msec.count();

	this->_results[resultsIndex].total = stream.str();
	
}

template<typename T>
void EngineTest<T>::convertResultsMean(int resultsIndex)
{

	this->_results[resultsIndex].mean /= iterations;

}

template<typename T>
std::string EngineTest<T>::getResults() const
{

	std::stringstream stream;
	stream << std::setfill('-') << std::setw(50) << "" << '\n'
		<< std::setfill(' ') << std::setw((50-(_desc.size()+8))/2) << "" << "Engine: " << _desc << std::setw((50-(_desc.size()+8))/2) << "" << '\n'
		<< std::setfill('-') << std::setw(50) << "" << "\n\n";
	
	switch (level)
	{
	case 1:
		stream << " Fastest:\t\t" << std::setfill(' ') << std::setw(std::numeric_limits<long long int>::digits10) << _results[0].fastest << " ns\n"
			<< " Slowest:\t\t" << std::setw(std::numeric_limits<long long int>::digits10) << _results[0].slowest << " ns\n"
			<< " Mean:\t\t\t" << std::setw(std::numeric_limits<long long int>::digits10) << _results[0].mean << " ns\n\n"
			<< " Total (min:sec:msec):\t\t\t" << _results[0].total << '\n';

		break;
	case 2:
		//fall through to case 3

	case 3:
		for (int x = 1; x < 4; x++) {
			stream << std::setfill('-') << _results[x].distribution << std::setw(50 - _results[x].distribution.size()) << "" << "\n\n"
				<< " Fastest:\t\t" << std::setfill(' ') << std::setw(std::numeric_limits<long long int>::digits10) << _results[x].fastest << " ns\n"
				<< " Slowest:\t\t" << std::setw(std::numeric_limits<long long int>::digits10) << _results[x].slowest << " ns\n"
				<< " Mean:\t\t\t" << std::setw(std::numeric_limits<long long int>::digits10) << _results[x].mean << " ns\n\n"
				<< " Total (min:sec:msec):\t\t\t" << _results[x].total << "\n\n";
		}

		break;
	}	
	return stream.str();

}

template<typename T>
std::string EngineTest<T>::getSettings()
{
	std::stringstream stream;
	stream << std::setfill('-') << std::setw(50) << "" << '\n'
		<< "\nIterations:  " << iterations << '\n'
		<< "Assigned to Vector:  ";
	if (level == 3)
		stream << "yes";
	else
		stream << "no";
	stream << '\n';
	return stream.str();

}