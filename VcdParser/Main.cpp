#include <iostream>
#include <string>
#include "Parser.h"

int main(int argc, char **argv)
{
	std::string vcdFilePath;
	Parser parser;

	std::cout << "Provide the .vcd file path: ";
	std::cin >> vcdFilePath;
	auto returnValue = EXIT_SUCCESS;
	
	// vcdFilePath = "C:\\Users\\danie\\Dropbox\\ConteudoPucrs\\VLSI2\\T1\\sap.vcd";
	
	if (parser.Parse(vcdFilePath))
	{
		parser.ShowReport();
	}
	else
	{
		std::cout << std::endl << "It was unable to parse the provided file!";
		returnValue = EXIT_FAILURE;
	}

	return returnValue;
}
