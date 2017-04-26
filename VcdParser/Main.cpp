#include <iostream>
#include <string>
#include "Parser.h"
#include <algorithm>

int main(int argc, char **argv)
{
	std::string vcdFilePath;
	Parser parser;
	auto returnValue = EXIT_SUCCESS;

	if (argc > 1)
	{
		vcdFilePath = argv[1];
	}
	else
	{
		std::cout << "Provide the .vcd file path: ";
		std::cin >> vcdFilePath;
	}
	
	std::transform(vcdFilePath.begin(), vcdFilePath.end(), vcdFilePath.begin(), ::tolower);

	if (vcdFilePath.find(".vcd") != std::string::npos)
	{
		if (parser.Parse(vcdFilePath))
		{
			parser.ShowReport();
		}
		else
		{
			std::cout << std::endl << "It was unable to parse the provided file!";
			returnValue = EXIT_FAILURE;
		}
	}
	else
	{
		std::cout << std::endl << "The provided file is not a .vcd file!";
		returnValue = EXIT_FAILURE;
	}

	return returnValue;
}
