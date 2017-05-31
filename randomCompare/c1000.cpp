#include <algorithm>
#include <cctype>
#include <fstream>
#include <memory>
#include "engines.h"

//settings struct, default initialized to...defaults
struct Settings {
	std::string progN = "";
	std::string fileN = "";
	int level = 3; //level 1 = run engine(), level 2 = run dist1-3(eng), level 3 = run dist1-3(eng) assigned to vector
	int iterations = 1000000; // 1 - 5,000,000
	int clock = 2;
	bool help = false;
	bool pagination = true;
	bool sorting = true;

};

static Settings set;
// set BaseTest ref to reference settings struct values
int& BaseTest::level(set.level);
int& BaseTest::iterations(set.iterations);
int& BaseTest::clock(set.clock);


enum errors {
	ILLEGAL_FLAG,
	IMPROPER_USAGE,
	ILLEGAL_VALUE,
	OUT_OF_BOUNDS_VALUE,
	ILLEGAL_FILENAME

};

void printHelp()
{
	std::cerr << "\n-- STD/BOOST RANDOM COMPARANATOR 1000 --\n         _Benchmark Tool Help_\n\n"
		<< "Usage:\n\n " << set.progN << " -FLAG VALUE\n\n"
		<< "Options:\n\n"
		<< " FLAG\t| VALUE/@Description\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -h\t|\n"
		<< "\t| @Display help message\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -c\t|   1\tFastest, Slowest, Mean, Total\n"
		<< "\t|   2\tTotal, CPU\n"
		<< "\t| @(1)Time each call or (2)time overall\n"
		<< "\t| @default: Real Clock + CPU time\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -f\t|   FILENAME\t\n"
		<< "\t| @Print results to FILENAME.txt\n"
		<< "\t| @default: print to console\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -i\t|   1-10000000\t\n"
		<< "\t| @Set number of iterations per test\n"
		<< "\t| @default: 1,000,000\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -l\t|   1\tPRNG wo distribution\n"
		<< "\t|   2\tPRNG w 3 distributions\n"
		<< "\t|   3\tPRNG w 3 distributions, assign to vec\n"
		<< "\t| @Set benchmark level\n"
		<< "\t| @default: 3\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -p\t|   0\tDisable\n"
		<< "\t|   1\tEnable\n"
		<< "\t| @Enable/Disable pagination when printing to console\n"
		<< "\t| @default: enabled\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -s\t|   0\tDisable\n"
		<< "\t|   1\tEnable\n"
		<< "\t| @Enable/Disable sorting of results\n"
		<< "\t| @default: enabled\n";
	

}

void printError(errors err)
{
	switch (err)
	{
	case ILLEGAL_FLAG:
		std::cerr << ">Error: Unknown flag";
		break;
	case IMPROPER_USAGE:
		std::cerr << ">Error: Improper usage";
		break;
	case ILLEGAL_VALUE:
		std::cerr << ">Error: Missing or illegal value";
		break;
	case OUT_OF_BOUNDS_VALUE:
		std::cerr << ">Error: Value out of bounds";
		break;
	case ILLEGAL_FILENAME:
		std::cerr << ">Error: Missing or illegal filename";
	default:
		break;
	}
	std::cerr << "\n>To display help: " << set.progN << " -h\n";

}


bool isNumber(const std::string& s) 
{
	return !s.empty() && std::all_of(s.begin(),
		s.end(), ::isdigit); // check if each character of the string isdigit()
}

bool isValidFN(const std::string& s)
{
	std::size_t pos = s.find_first_of("\"\'\\/:.|?*<>{}[]()\' '"); //if found illegal or 'troublesome' character, bad file name string
	if (!s.empty() && pos == std::string::npos)
		return true;
	else
		return false;
}

bool parseOptions(const std::vector<std::string>& args)
{
	for (int i = 1;i < args.size(); i++)
	{
		if (args[i][0] == '-' && args[i].size() == 2) //if switch is properly formed eg. first character is - and is 2 length
		{
			if (args[i][1] == 'h' || args[i][1] == '?') //help switch, if found stop parsing and print help
			{
				set.help = true;
				return true;
			}
			else if (args[i][1] == 'i') //iterations switch, argument must be number, must be between 1 and 5,000,000 inclusive
			{
				if(i+1 < args.size() && isNumber(args[i+1])){
					int temp = std::stoi(args[i + 1]);
					if(temp < 10000001 && temp >0)
					{
						set.iterations = temp;
						i++;
					}
					else // value too large or too small(negative)
					{
						printError(errors::OUT_OF_BOUNDS_VALUE);
						return false;		
					}
				}
				else // not a sane number, or no argument
				{
					printError(errors::ILLEGAL_VALUE);
					return false;
				}
			}
			else if (args[i][1] == 'l') //test level switch, argument must be number, must be 1 digit
			{
				if (i + 1 < args.size() && isNumber(args[i + 1]) && args[i+1].size() < 2) {
					int temp = std::stoi(args[i + 1]);
					if (temp == 1 || temp == 2 || temp == 3)
					{
						set.level = temp;
						i++;
					}
					else //value not 1,2,3
					{
						printError(errors::OUT_OF_BOUNDS_VALUE);
						return false;
					}
				}
				else  // not a sane number, or no argument
				{
					printError(errors::ILLEGAL_VALUE);
					return false;
				}
			}
			else if (args[i][1] == 'f') // file name switch, argument must be valid filename
			{
				if (i + 1 < args.size() && isValidFN(args[i + 1]))
				{			
					set.fileN = args[i + 1];
					i++;		
				}
				else // bad filename
				{
					printError(errors::ILLEGAL_FILENAME);
					return false;
				}
			}
			else if (args[i][1] == 'c') // clock switch, argument must be number, must be 1 digit
			{
				if (i + 1 < args.size() && isNumber(args[i + 1]) && args[i + 1].size() < 2) {
					int temp = std::stoi(args[i + 1]);
					if (temp == 1 || temp == 2)
					{
						set.clock = temp;
						i++;
					}
					else //value not 1,2
					{
						printError(errors::OUT_OF_BOUNDS_VALUE);
						return false;
					}
				}
				else  // not a sane number, or no argument
				{
					printError(errors::ILLEGAL_VALUE);
					return false;
				}
			}
			else if (args[i][1] == 'p') // pagination switch, argument must be number, must be 1 digit
			{
				if (i + 1 < args.size() && isNumber(args[i + 1]) && args[i + 1].size() < 2) {
					int temp = std::stoi(args[i + 1]);
					if (temp == 0 || temp == 1)
					{
						set.pagination = temp;
						i++;
					}
					else //value not 0,1
					{
						printError(errors::OUT_OF_BOUNDS_VALUE);
						return false;
					}
				}
				else  // not a sane number, or no argument
				{
					printError(errors::ILLEGAL_VALUE);
					return false;
				}
			}
			else if (args[i][1] == 's') // sorting switch, argument must be number, must be 1 digit
			{
				if (i + 1 < args.size() && isNumber(args[i + 1]) && args[i + 1].size() < 2) {
					int temp = std::stoi(args[i + 1]);
					if (temp == 0 || temp == 1)
					{
						set.sorting = temp;
						i++;
					}
					else //value not 0,1
					{
						printError(errors::OUT_OF_BOUNDS_VALUE);
						return false;
					}
				}
				else  // not a sane number, or no argument
				{
					printError(errors::ILLEGAL_VALUE);
					return false;
				}
			}
			else // bad or unrecognized flag
			{
				printError(errors::ILLEGAL_FLAG);
				return false;
			}
		}
		else // no switch formed at all
		{
			printError(errors::IMPROPER_USAGE);
			return false;
		}
	}
	return true;

}

struct simpleTotalSort
{
	bool operator()(const std::unique_ptr<BaseTest>& x, const std::unique_ptr<BaseTest>& y) const
	{
		return x->getSimpleTotals() < y->getSimpleTotals();
	}
};

struct multiTotalSort
{
	bool operator()(const std::unique_ptr<BaseTest>& x, const std::unique_ptr<BaseTest>&y) const
	{
		return *x < *y;
	}
};


int main(int argc, char* argv[])
{
	
	//copy args to string vector so we can use string algorithms
	std::vector<std::string> args;
	for(int i =0; i<argc; i++)
		args.push_back(argv[i]);
	//get the program name without the path
	std::size_t pos = args[0].find_last_of("/\\");
	set.progN = args[0].substr(pos + 1);
	//if there's more than one command line option, parse it and update settings struct
	if (argc != 1)
		if (!parseOptions(args))
			return EXIT_FAILURE; // if parseOptions fails, end the program
		

	if(set.help==true){
		printHelp();
		return EXIT_FAILURE; //after displaying help, end
	}


	//Create std random EngineTest pointers
	std::unique_ptr<BaseTest> t0(new EngineTest<std::minstd_rand>("std - Minimum Standard"));
	std::unique_ptr<BaseTest> t1(new EngineTest<std::minstd_rand0>("std - Minimum Standard 0"));

	std::unique_ptr<BaseTest> t2(new EngineTest<std::knuth_b>("std - Knuth B"));

	std::unique_ptr<BaseTest> t3(new EngineTest<std::mt19937>("std - Mersenne Twister"));
	std::unique_ptr<BaseTest> t4(new EngineTest<std::mt19937_64>("std - Mersenne Twister 64"));

	std::unique_ptr<BaseTest> t5(new EngineTest<std::ranlux24_base>("std - Ranlux24 Base"));
	std::unique_ptr<BaseTest> t6(new EngineTest<std::ranlux24>("std - Ranlux24"));

	std::unique_ptr<BaseTest> t7(new EngineTest<std::ranlux48_base>("std - Ranlux48 Base"));
	std::unique_ptr<BaseTest> t8(new EngineTest<std::ranlux48>("std - Ranlux48"));

	//boost engines --- commented engines will not compile using std random distributions
	std::unique_ptr<BaseTest> t9(new EngineTest<boost::random::ecuyer1988>("boost - Ecuyer 1988"));

	std::unique_ptr<BaseTest> t10(new EngineTest<boost::random::hellekalek1995>("boost - HelleKalek 1995"));

	std::unique_ptr<BaseTest> t11(new EngineTest<boost::random::knuth_b>("boost - Knuth B"));

	std::unique_ptr<BaseTest> t12(new EngineTest<boost::random::kreutzer1986>("boost - Kreutzer 1986"));

//	std::unique_ptr<BaseTest> t13(new EngineTest<boost::random::lagged_fibonacci1279>("boost - L Fibonacci 1279"));
//	std::unique_ptr<BaseTest> t14(new EngineTest<boost::random::lagged_fibonacci19937>("boost - L Fibonacci 19937"));
//	std::unique_ptr<BaseTest> t15(new EngineTest<boost::random::lagged_fibonacci2281>("boost - L Fibonacci 2281"));
//	std::unique_ptr<BaseTest> t16(new EngineTest<boost::random::lagged_fibonacci23209>("boost - L Fibonacci 23209"));
//	std::unique_ptr<BaseTest> t17(new EngineTest<boost::random::lagged_fibonacci3217>("boost - L Fibonacci 3217"));
//	std::unique_ptr<BaseTest> t18(new EngineTest<boost::random::lagged_fibonacci4423>("boost - L Fibonacci 4423"));
//	std::unique_ptr<BaseTest> t19(new EngineTest<boost::random::lagged_fibonacci44497>("boost - L Fibonacci 44497"));
//	std::unique_ptr<BaseTest> t20(new EngineTest<boost::random::lagged_fibonacci607>("boost - L Fibonacci 607"));
//	std::unique_ptr<BaseTest> t21(new EngineTest<boost::random::lagged_fibonacci9689>("boost - L Fibonacci 9689"));

	std::unique_ptr<BaseTest> t22(new EngineTest<boost::random::minstd_rand>("boost - Minimum Standard"));
	std::unique_ptr<BaseTest> t23(new EngineTest<boost::random::minstd_rand0>("boost - Minimum Standard 0"));

	std::unique_ptr<BaseTest> t24(new EngineTest<boost::random::mt11213b>("boost - Mersenne Twister 11213b"));
	std::unique_ptr<BaseTest> t25(new EngineTest<boost::random::mt19937>("boost - Mersenne Twister"));
	std::unique_ptr<BaseTest> t26(new EngineTest<boost::random::mt19937_64>("boost - Mersenne Twister 64"));

//	std::unique_ptr<BaseTest> t27(new EngineTest<boost::random::ranlux_base_01>("boost - Ranlux Base_01"));
	std::unique_ptr<BaseTest> t28(new EngineTest<boost::random::ranlux24_base>("boost - Ranlux24 Base"));
	std::unique_ptr<BaseTest> t29(new EngineTest<boost::random::ranlux24>("boost - Ranlux24"));
	
	std::unique_ptr<BaseTest> t30(new EngineTest<boost::random::ranlux3>("boost - Ranlux3"));
//	std::unique_ptr<BaseTest> t31(new EngineTest<boost::random::ranlux3_01>("boost - Ranlux3_01"));

	std::unique_ptr<BaseTest> t32(new EngineTest<boost::random::ranlux4>("boost - Ranlux4"));
//	std::unique_ptr<BaseTest> t33(new EngineTest<boost::random::ranlux4_01>("boost - Ranlux4_01"));
	std::unique_ptr<BaseTest> t34(new EngineTest<boost::random::ranlux48_base>("boost - Ranlux48 Base"));
	std::unique_ptr<BaseTest> t35(new EngineTest<boost::random::ranlux48>("boost - Ranlux48"));
	
	std::unique_ptr<BaseTest> t36(new EngineTest<boost::random::ranlux64_base>("boost - Ranlux64 Base"));
	std::unique_ptr<BaseTest> t37(new EngineTest<boost::random::ranlux64_3>("boost - Ranlux64_3"));
//	std::unique_ptr<BaseTest> t38(new EngineTest<boost::random::ranlux64_3_01>("boost - Ranlux64_3_01"));
	std::unique_ptr<BaseTest> t39(new EngineTest<boost::random::ranlux64_4>("boost - Ranlux64_4"));
//	std::unique_ptr<BaseTest> t40(new EngineTest<boost::random::ranlux64_4_01>("boost - Ranlux64_4_01"));

	std::unique_ptr<BaseTest> t41(new EngineTest<boost::random::taus88>("boost - Taus 88"));
	
	//create vector of tests
	std::vector<std::unique_ptr<BaseTest>> tests;

	//add tests to vector
	tests.push_back(std::move(t0));
	tests.push_back(std::move(t1));
	tests.push_back(std::move(t2));
	tests.push_back(std::move(t3));
	tests.push_back(std::move(t4));
	tests.push_back(std::move(t5));
	tests.push_back(std::move(t6));
	tests.push_back(std::move(t7));
	tests.push_back(std::move(t8));

	tests.push_back(std::move(t9));
	tests.push_back(std::move(t10));
	tests.push_back(std::move(t11));
	tests.push_back(std::move(t12));
//	tests.push_back(std::move(t13));
//	tests.push_back(std::move(t14));
//	tests.push_back(std::move(t15));
//	tests.push_back(std::move(t16));
//	tests.push_back(std::move(t17));
//	tests.push_back(std::move(t18));
//	tests.push_back(std::move(t19));
//	tests.push_back(std::move(t20));
//	tests.push_back(std::move(t21));
	tests.push_back(std::move(t22));
	tests.push_back(std::move(t23));
	tests.push_back(std::move(t24));
	tests.push_back(std::move(t25));
	tests.push_back(std::move(t26));
//	tests.push_back(std::move(t27));
	tests.push_back(std::move(t28));
	tests.push_back(std::move(t29));
	tests.push_back(std::move(t30));
//	tests.push_back(std::move(t31));
	tests.push_back(std::move(t32));
//	tests.push_back(std::move(t33));
	tests.push_back(std::move(t34));
	tests.push_back(std::move(t35));
	tests.push_back(std::move(t36));
	tests.push_back(std::move(t37));
//	tests.push_back(std::move(t38));
	tests.push_back(std::move(t39));
//	tests.push_back(std::move(t40));
	tests.push_back(std::move(t41));

	//run each instances test
	for (int i = 0; i < tests.size(); i++)
		tests[i]->operator()();

	//If sorting enabled, sort vector
	if (set.sorting)
	{
		std::cout << ">Sorting results...";
		if(set.level==1)
			std::sort(tests.begin(), tests.end(), simpleTotalSort());
		else
			std::sort(tests.begin(), tests.end(), multiTotalSort());
		std::cout << "done!\n";
	}


	//If file name is not set, print to console
	if(set.fileN.empty())  
	{
		for (int i = 0; i < tests.size(); i++){
			std::cout << tests[i]->getResults();
			if ((i+1) % 10 == 0 && set.pagination){
				std::cout << "\n__________________\n"
						  << "[Enter] to Advance\n";
				std::cin.ignore();
			}
		}
		std::cout << EngineTest<>::getSettings();
	}
	//else print to file
	else
	{
		set.fileN.append(".txt");
		std::ofstream results;
		results.open(set.fileN);
		for (int i = 0; i < tests.size(); i++)
			results << tests[i]->getResults();
		results << EngineTest<>::getSettings();
		results.close();
	}

	return EXIT_SUCCESS;

}