#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

struct Module
{
	std::string Name;
	unsigned Signals;
};

struct Signal
{
	std::string Name;
	std::string Symbol;
};

std::vector<std::string> SplitBySpace(std::string str)
{
	std::string buf;
	std::stringstream ss(str);

	std::vector<std::string> tokens;

	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}

int main()
{
	std::string vcdFilePath = "C:\\Users\\daniel.vega\\Dropbox\\ConteudoPucrs\\VLSI2\\T1\\sap.vcd";
	std::ifstream vcdFile;
	std::string line;
	std::vector<Module> modules;

	vcdFile.open(vcdFilePath);

	if (vcdFile.is_open())
	{
		while (getline(vcdFile, line))
		{
			if (line.find("$scope") != std::string::npos)
			{
				std::vector<std::string> splittedLine = SplitBySpace(line);
				Module module;
				module.Name = splittedLine[2];

				unsigned signalCounter = 0;
				while (getline(vcdFile, line))
				{
					if(line.find("$var") != std::string::npos)
					{
						signalCounter++;
					}
					else {
						break;
					}
				}
				module.Signals = signalCounter;
				modules.push_back(module);
			}
			
		}
		vcdFile.close();
	}
	else {
		std::cout << "Unable to open the provided vcd file";
	}

	std::cout << "Number of Modules: " << modules.size() << std::endl;
	
	for (auto it = modules.begin(); it != modules.end(); ++it)
	{
		std::cout << "Module: " << (*it).Name << std::endl;
		std::cout << "Signals count: " << (*it).Signals << std::endl;
		std::cout << std::endl;
	}

	std::cout << "Clock frequency: " << "TBD" << std::endl;
	std::cout << "Switching activiy: " << "TBD" << std::endl;
	std::cout << std::endl;

}