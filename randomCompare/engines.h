#include <array>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <numeric>
#include <boost\random.hpp>
#include <boost\chrono\chrono.hpp>
#include <boost\timer\timer.hpp>


struct Results //for storing test times
{
	std::string distribution = "";
	long long int fastest = 0;
	long long int slowest = 0;
	long long int mean = 0;
	float total = 0;
	float cpuTotal = 0;

};


class BaseTest
{

public:
	virtual ~BaseTest() = default;
	virtual void operator()() = 0;
	virtual bool operator< (const BaseTest& b);
	virtual const std::string& getDesc() const = 0;
	virtual std::string getResults() const = 0;
	virtual const float& getSimpleTotals() const = 0;
	
protected:
	std::array<Results, 4> _results;
	static int& level; //reference to global arg for test level
	static int& iterations; //reference to global arg for iterations per test
	static int& clock; //reference to global arg for clock stats to display
};

bool BaseTest::operator< (const BaseTest& b)
{
	float resultA = 0;
	float resultB = 0;
	for (int i = 1;i < _results.size();i++)
	{
		resultA += _results[i].total;
		resultB += b._results[i].total;
	}
	if (resultA == resultB)
		return _results[0].total < b._results[0].total;
	else
		return resultA < resultB;

}


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
		_results[3].distribution = "Bernoulli";
		_timer.stop();
		_totalTimer.stop();
		}
	~EngineTest() {}
	
	virtual void operator()() override { this->runTest(); }

	virtual const std::string& getDesc() const override { return _desc; }

	//format _results and return
	virtual std::string getResults() const override;

	//return level 0 Totals from _results for faster comparison when sorting
	virtual const float& getSimpleTotals() const override;

	//format settings and return
	static std::string getSettings();

private:
	void runTest();

	//conver _totalTimer values and push to _results array struct
	void convertTotalTimer(int resultsIndex);
	//creates mean from total
	void convertResultsMean(int resultsIndex);


	boost::timer::cpu_timer _timer;	//single call timer
	boost::timer::cpu_timer _totalTimer; //loop timer


	T _eng;
	std::uniform_int_distribution<int> _dist1;
	std::normal_distribution<float> _dist2;
	std::bernoulli_distribution _dist3;

	std::string _desc;


};

template<typename T>
const float& EngineTest<T>::getSimpleTotals() const
{
	return _results[0].total;
}


template<typename T>
std::string EngineTest<T>::getResults() const
{

	std::stringstream stream;
	stream << std::setfill('-') << std::setw(52) << "" << '\n'
		<< std::setfill(' ') << std::setw((52 - (_desc.size() + 8)) / 2) << "" << "Engine: " << _desc << std::setw((52 - (_desc.size() + 8)) / 2) << "" << '\n'
		<< std::setfill('-') << std::setw(52) << "" << "\n\n";
	std::streamsize original = std::cout.precision();
	switch (level)
	{
	case 1:
		if (clock == 1)
		{
			stream << " Fastest:\t\t" << std::setfill(' ') << std::setw(std::numeric_limits<long long int>::digits10) << _results[0].fastest << " ns\n"
				<< " Slowest:\t\t" << std::setw(std::numeric_limits<long long int>::digits10) << _results[0].slowest << " ns\n"
				<< " Mean:\t\t\t" << std::setw(std::numeric_limits<long long int>::digits10) << _results[0].mean << " ns\n\n"
				<< " Total:\t\t\t\t\t" << std::left << std::setfill('0') << std::fixed << std::setprecision(7) << std::setw(9) << _results[0].total << std::setprecision(original) <<  std::right << " s\n";
			break;
		}
		else if (clock == 2)
		{
			
			
			float percentage = (_results[0].cpuTotal / _results[0].total) * 100.0f;

			stream << " Real Time:\t\t\t\t" << std::left << std::setfill('0') << std::fixed << std::setprecision(7) << std::setw(9) << _results[0].total << " s\n"
				<< " CPU " << std::right << std::setfill(' ') << std::setw(7) << std::setprecision(2) << percentage << std::setprecision(original) << "%:\t\t\t\t" << std::left << std::setfill('0') << std::setprecision(7) << std::setw(9) << _results[0].cpuTotal << std::setprecision(original) << " s\n";
			break;
		}

	case 2:
		//fall through to case 3

	case 3:
		if (clock == 1)
		{
			for (int x = 1; x < 4; x++)
			{
				stream << std::setfill('-') << _results[x].distribution << std::setw(52 - _results[x].distribution.size()) << "" << "\n\n"
					<< " Fastest:\t\t" << std::setfill(' ') << std::setw(std::numeric_limits<long long int>::digits10) << _results[x].fastest << " ns\n"
					<< " Slowest:\t\t" << std::setw(std::numeric_limits<long long int>::digits10) << _results[x].slowest << " ns\n"
					<< " Mean:\t\t\t" << std::setw(std::numeric_limits<long long int>::digits10) << _results[x].mean << " ns\n\n"
					<< " Total:\t\t\t\t\t" << std::left << std::setfill('0') << std::fixed << std::setprecision(7) << std::setw(9) << _results[x].total << std::setprecision(original) << std::right << " s\n\n";
			}
			break;
		}
		else if (clock == 2)
		{
			
			for (int x = 1; x < 4; x++)
			{
				float percentage = (_results[x].cpuTotal / _results[x].total) * 100.0f;

				stream << std::setfill('-') << _results[x].distribution << std::setw(52 - _results[x].distribution.size()) << "" << "\n\n"
					<< " Real Time:\t\t\t\t" << std::left << std::setfill('0') << std::fixed << std::setprecision(7) << std::setw(9) << _results[x].total << " s\n"
					<< " CPU " << std::right << std::setfill(' ') << std::setw(7) << std::setprecision(2) << percentage << std::setprecision(original) << "%:\t\t\t\t" << std::left << std::setfill('0') << std::setprecision(7) << std::setw(9) << _results[x].cpuTotal << std::setprecision(original) << " s\n\n";
			}
			break;
		}

	}
	return stream.str();

}

template<typename T>
std::string EngineTest<T>::getSettings()
{
	std::stringstream stream;
	stream << std::setfill('-') << std::setw(52) << "" << '\n'
		<< "\nIterations:  " << iterations << '\n'
		<< "Assigned to Vector:  ";
	if (level == 3)
		stream << "yes";
	else
		stream << "no";
	stream << '\n';
	return stream.str();

}


template<typename T>
void EngineTest<T>::runTest() 
{
	
	bool initialized = false;
	switch (level)
	{
	case 1:
		std::cout << ">Starting test for: " << _desc << "...";
		
		if(clock == 1)
		{ //clock variant 1
			for (int i = 0; i < iterations; i++)
			{ //run test
				_timer.elapsed().clear();
				_timer.start();
				_eng();
				_timer.stop();
				long long int thisIt = _timer.elapsed().wall;
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
				_results[0].total += thisIt / 1000000000.0; //keep accumulating total time
			}
			convertResultsMean(0); //convert total to mean
		}
		else if (clock == 2)
		{ //clock variant 2
			_totalTimer.elapsed().clear();
			_totalTimer.start();
			for (int i = 0; i < iterations; i++)
				_eng();
			_totalTimer.stop();
			convertTotalTimer(0); //convert timer and push to _results array struct
		}	
		
		std::cout << "done!\n";

		break;
	case 2:
		std::cout << ">Starting test for: " << _desc << '\n';
		for (int x = 1; x < 4; x++)
		{ //for each distribution starting at _results[1] to _results[4] as x
			std::cout << ">" << x << "/3...";
			if (clock == 1)
			{ //clock variant 1
				for (int i = 0; i < iterations; i++)
				{ //run test for x

					switch (x) {
					case 1:
						_timer.elapsed().clear();
						_timer.start();
						_dist1(_eng);
						_timer.stop();
						break;
					case 2:
						_timer.elapsed().clear();
						_timer.start();
						_dist2(_eng);
						_timer.stop();
						break;
					case 3:
						_timer.elapsed().clear();
						_timer.start();
						_dist3(_eng);
						_timer.stop();
						break;
					}
					long long int thisIt = _timer.elapsed().wall;
					if (!initialized)
					{
						_results[x].fastest = thisIt;
						initialized = true;
					}
					if (_results[x].fastest > thisIt)
						_results[x].fastest = thisIt;
					if (_results[x].slowest < thisIt)
						_results[x].slowest = thisIt;
					_results[x].total += thisIt / 1000000000.0;

				}
				convertResultsMean(x);
			}
			else if (clock == 2)
			{ // clock variant 2
				switch (x) {  //run test for x
				case 1:
					_totalTimer.elapsed().clear();
					_totalTimer.start();
					for (int i = 0; i < iterations; i++)
						_dist1(_eng);
					_totalTimer.stop();
					convertTotalTimer(x);
					break;
				case 2:
					_totalTimer.elapsed().clear();
					_totalTimer.start();
					for (int i = 0; i < iterations; i++)
						_dist2(_eng);
					_totalTimer.stop();
					convertTotalTimer(x);
					break;
				case 3:
					_totalTimer.elapsed().clear();
					_totalTimer.start();
					for (int i = 0; i < iterations; i++)
						_dist3(_eng);
					_totalTimer.stop();
					convertTotalTimer(x);
					break;
				}

			}
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
			if (clock == 1)
			{ //clock variant 1
				for (int i = 0; i < iterations; i++)
				{ //run test for x

					switch (x) {
					case 1:
						_timer.elapsed().clear();
						_timer.start();
						intVec[i] = _dist1(_eng);
						_timer.stop();
						break;
					case 2:
						_timer.elapsed().clear();
						_timer.start();
						floatVec[i] = _dist2(_eng);
						_timer.stop();
						break;
					case 3:
						_timer.elapsed().clear();
						_timer.start();
						boolVec[i] = _dist3(_eng);
						_timer.stop();
						break;
					}
					long long int thisIt = _timer.elapsed().wall;
					if (!initialized)
					{
						_results[x].fastest = thisIt;
						initialized = true;
					}
					if (_results[x].fastest > thisIt)
						_results[x].fastest = thisIt;
					if (_results[x].slowest < thisIt)
						_results[x].slowest = thisIt;
					_results[x].total += thisIt / 1000000000.0;

				}
				convertResultsMean(x);
			}

			else if (clock == 2)
			{ //clock variant 2
				switch (x) {  //run test for x
				case 1:
					_totalTimer.elapsed().clear();
					_totalTimer.start();
					for (int i = 0; i < iterations; i++)
						intVec[i] = _dist1(_eng);
					_totalTimer.stop();
					convertTotalTimer(x);
					break;
				case 2:
					_totalTimer.elapsed().clear();
					_totalTimer.start();
					for (int i = 0; i < iterations; i++)
						floatVec[i] = _dist2(_eng);
					_totalTimer.stop();
					convertTotalTimer(x);
					break;
				case 3:
					_totalTimer.elapsed().clear();
					_totalTimer.start();
					for (int i = 0; i < iterations; i++)
						boolVec[i] = _dist3(_eng);
					_totalTimer.stop();
					convertTotalTimer(x);
					break;
				}

			}

			std::cout << "done!\n";
		}		
		break;
	}
}

template<typename T>
void EngineTest<T>::convertTotalTimer(int resultsIndex)
{
	_results[resultsIndex].total = _totalTimer.elapsed().wall / 1000000000.0;
	float tempUser = _totalTimer.elapsed().user / 1000000000.0;
	float tempSystem = _totalTimer.elapsed().system / 1000000000.0;
	_results[resultsIndex].cpuTotal = tempUser + tempSystem;

}

template<typename T>
void EngineTest<T>::convertResultsMean(int resultsIndex)
{
	this->_results[resultsIndex].mean = _results[resultsIndex].total * 1000000000.0 / iterations;

}
