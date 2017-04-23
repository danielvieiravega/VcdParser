#include <iostream>
#include <string>
#include "Parser.h"

int main()
{
	std::string vcdFilePath = "C:\\Users\\danie\\Dropbox\\ConteudoPucrs\\VLSI2\\T1\\sap.vcd";
	Parser parser;

	if (parser.Parse(vcdFilePath))
		parser.ShowReport();
	else
		std::cout << "It was unable to parse the provided file!";
}