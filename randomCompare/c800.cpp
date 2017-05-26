#include <algorithm>
#include <cctype>
#include <fstream>
#include <memory>
#include "engines.h"

//settings struct, default initialized to...defaults
struct Settings {
	std::string progN = "";
	int level = 3; //level 1 = run engine(), level 2 = run dist1-3(eng), level 3 = run dist1-3(eng) assigned to vector
	int iterations = 500000; // 1 - 5,000,000
	std::string fileN = "";
	bool help = false;

};

static Settings set;
// set BaseTest ref to reference settings struct values
int& BaseTest::level(set.level);
int& BaseTest::iterations(set.iterations);

enum errors {
	ILLEGAL_FLAG,
	IMPROPER_USAGE,
	ILLEGAL_VALUE,
	OUT_OF_BOUNDS_VALUE,
	ILLEGAL_FILENAME

};

void printHelp()
{
	std::cerr << "\n-- STD RANDOM COMPARANATOR 800 --\n  -    Benchmark Tool Help    -\n\n"
		<< "Usage:\n\n " << set.progN << " -FLAG VALUE\n\n"
		<< "Options:\n\n"
		<< " FLAG\t| VALUE/@Description\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -h\t|\n"
		<< "\t| @Display help message\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -l\t|   1\tPRNG wo distribution\n"
		<< "\t|   2\tPRNG w 3 distributions\n"
		<< "\t|   3\tPRNG w 3 distributions, assign to vec\n"
		<< "\t| @Set benchmark level\n"
		<< "\t| @default: 3\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -i\t|   1-5000000\t\n"
		<< "\t| @Set number of iterations per test\n"
		<< "\t| @default: 500,000\n"
		<< std::setfill('-') << std::setw(55) << "" << '\n'
		<< " -f\t|   FILENAME\t\n"
		<< "\t| @Print results to FILENAME.txt\n"
		<< "\t| @default: print to console\n";
	

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
					if(temp < 5000001 && temp >0)
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

	//Create std random EngineTest pointers, should use unique_ptr here
	std::unique_ptr<BaseTest> t0(new EngineTest<std::minstd_rand>("Minimum Standard"));
	std::unique_ptr<BaseTest> t1(new EngineTest<std::minstd_rand0>("Minimum Standard 0"));
	std::unique_ptr<BaseTest> t2(new EngineTest<std::knuth_b>("Knuth B"));
	std::unique_ptr<BaseTest> t3(new EngineTest<std::mt19937>("Mersenne Twister"));
	std::unique_ptr<BaseTest> t4(new EngineTest<std::mt19937_64>("Mersenne Twister 64"));
	std::unique_ptr<BaseTest> t5(new EngineTest<std::ranlux24_base>("Ranlux24 Base"));
	std::unique_ptr<BaseTest> t6(new EngineTest<std::ranlux24>("Ranlux24"));
	std::unique_ptr<BaseTest> t7(new EngineTest<std::ranlux48_base>("Ranlux48 Base"));
	std::unique_ptr<BaseTest> t8(new EngineTest<std::ranlux48_base>("Ranlux48"));
	
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

	//run each instances test
	for (int i = 0; i < tests.size(); i++)
		tests[i]->operator()();

	//If file name is not set, print to console
	if(set.fileN.empty())  
	{
		for (int i = 0; i < tests.size(); i++)
			std::cout << tests[i]->getResults();
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