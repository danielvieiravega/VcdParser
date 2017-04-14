#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "Parser.h"

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
	std::string vcdFilePath = "C:\\Users\\danie\\Dropbox\\ConteudoPucrs\\VLSI2\\T1\\sap.vcd";
	std::ifstream vcdFile;
	std::string line;
	std::vector<Module> modules;
	double clock = 0;

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

				while (getline(vcdFile, line))
				{
					if(line.find("$var") != std::string::npos)
					{
						Signal signal;
						signal.SwitchingActivityCounter = -1;
						std::vector<std::string> splittedLineSignal = SplitBySpace(line);
						signal.Symbol = splittedLineSignal[3];
						if (splittedLineSignal.size() == 6)
						{
							signal.Name = splittedLineSignal[4];
						}
						else
						{
							signal.Name = splittedLineSignal[4] + ' ' + splittedLineSignal[5];
						}

						module.Signals.push_back(signal);
					}
					else {
						break;
					}
				}

				modules.push_back(module);
			}
			else if (line.find("$dumpvars") != std::string::npos)
			{
				std::vector<std::string> switchingActivitySignals;
				switchingActivitySignals.push_back("0");
				switchingActivitySignals.push_back("1");
				switchingActivitySignals.push_back("z");

				bool foundSwitchingActivity = false;

				
				unsigned clockCounter = 0;
				unsigned middle = 0;
				unsigned right = 0;
				std::string::size_type sz;
				while (getline(vcdFile, line))
				{
					if((line.size() > 0) && (line.at(0) == '#') && (clockCounter < 2))
					{
						std::string numberStr = line.substr(1, std::string::npos);
						int i_dec = std::stoi(numberStr, &sz);

						//TODO: Maybe the middle is not necessary
						if (clockCounter == 0)
						{
							middle = std::stoi(numberStr, &sz);
						}
						else if (clockCounter == 1)
						{
							right = std::stoi(numberStr, &sz);
						}

						if (clockCounter == 1)
						{
							clock = 1.0 / (right*10e-9);
						}

						clockCounter++;
					}

					if ((line.find('0') != std::string::npos)
						|| (line.find('1') != std::string::npos)
						|| (line.find('z') != std::string::npos))
					{
						foundSwitchingActivity = false;

						for (std::vector<Module>::iterator modulesIterator = modules.begin(); 
							modulesIterator != modules.end() && !foundSwitchingActivity; 
							++modulesIterator)
						{
							for (std::vector<Signal>::iterator signalsIterator = modulesIterator->Signals.begin();
								signalsIterator != modulesIterator->Signals.end() && !foundSwitchingActivity;
								++signalsIterator)
							{
								for (std::vector<std::string>::iterator switchingActivityIterator = switchingActivitySignals.begin();
									(switchingActivityIterator != switchingActivitySignals.end()) && !foundSwitchingActivity;
									++switchingActivityIterator)
								{
									std::string currentSignalActivity = "";
									if (line.find('0') != std::string::npos)
									{
										currentSignalActivity = '0';
									}
									else if (line.find('1') != std::string::npos)
									{
										currentSignalActivity = '1';
									}
									else if (line.find('z') != std::string::npos)
									{
										currentSignalActivity = 'z';
									}

									if (line.find(signalsIterator->Symbol) != std::string::npos)
									{
										if (signalsIterator->SwitchingActivityCounter == -1)
										{
											signalsIterator->SwitchingActivityCounter++;
											signalsIterator->CurrentActivity = currentSignalActivity;
											foundSwitchingActivity = true;
										}
										else
										{
											if (signalsIterator->CurrentActivity != *switchingActivityIterator)
											{
												signalsIterator->SwitchingActivityCounter++;
												signalsIterator->CurrentActivity = currentSignalActivity;
												foundSwitchingActivity = true;
											}
											else
											{
												foundSwitchingActivity = false;
											}
										}
									}
									else
									{
										foundSwitchingActivity = false;
									}
								}
							}
						}
					}
				}
			}
			
			
		}

		

		vcdFile.close();
	}
	else {
		std::cout << "Unable to open the provided vcd file";
	}

	std::cout << "Number of Modules: " << modules.size() << std::endl;
	
	for (auto &it : modules)
	{
		std::cout << "Module: " << it.Name << std::endl;
		std::cout << "Signals count: " << it.Signals.size() << std::endl;
		std::cout << std::endl;
	}

	std::cout << "Clock frequency: " << clock << " Hz"<< std::endl;
	std::cout << std::endl;
}
