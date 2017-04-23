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

Parser::Parser()
	: clock(0), clockCounter(0), isClockCalculated(false)
{
}

void Parser::AddModule(Module module)
{
	modules.push_back(module);
}

void Parser::ShowReport()
{
	std::cout << "################" << std::endl;
	std::cout << "#  VCD Parser  #" << std::endl;
	std::cout << "################" << std::endl;
	std::cout << std::endl;

	for (auto &module : modules)
	{
		std::cout << "Module: " << module.Name << std::endl;
		std::cout << "Signals: " << module.Signals.size() << std::endl;

		for (auto &signal : module.Signals)
		{
			if (signal.SwitchingActivityCounter > 0)
			{
				std::cout << "Signal: " << signal.Name << std::endl;
				std::cout << "Switching activity: " << signal.SwitchingActivityCounter << std::endl;
				std::cout << std::endl;
			}
		}

		std::cout << "==================================" << std::endl;
		std::cout << std::endl;
	}

	std::cout << "Clock frequency: " << clock << " Hz" << std::endl;
	std::cout << std::endl;
}

void Parser::CalculeClockFrequency()
{
	if ((line.size() > 0) && (line.at(0) == '#') && (clockCounter < 2))
	{
		unsigned middleValue = 0;
		unsigned right = 0;
		std::string::size_type sz;
		std::string numberStr = line.substr(1, std::string::npos);
		int i_dec = std::stoi(numberStr, &sz);
	
		//TODO: Maybe the middle is not necessary
		if (clockCounter == 0)
		{
			middleValue = std::stoi(numberStr, &sz);
		}
		else if (clockCounter == 1)
		{
			right = std::stoi(numberStr, &sz);
		}

		if (clockCounter == 1)
		{
			clock = 1.0 / (right*10e-9);
			isClockCalculated = true;
		}

		clockCounter++;
	}
}

bool Parser::Parse(std::string vcdFilePath)
{
	bool result = false;
	std::string line;
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
					if (line.find("$var") != std::string::npos)
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

				AddModule(module);
			}
			else if (line.find("$dumpvars") != std::string::npos)
			{
				char switchingActivitySignals[SwitchingActivitySignalsSize] = { '0', '1', 'z' };

				auto foundSwitchingActivity = false;

				while (getline(vcdFile, line))
				{
					if (!IsClockCalculated())
						CalculeClockFrequency();

					if ((line[0] == '0') || (line[0] == '1') || (line[0] == 'z'))
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
								for (auto i = 0; i < SwitchingActivitySignalsSize && !foundSwitchingActivity; i++)
								{
									std::string lineSubStr = line.substr(1, line.size());

									if (signalsIterator->Symbol == "!")
									{
										std::cout << "1";
									}

									if (lineSubStr == signalsIterator->Symbol)
									{
										char currentSignalActivity = ' ';
										if (line[0] == '0')
										{
											currentSignalActivity = '0';
										}
										else if (line[0] == '1')
										{
											currentSignalActivity = '1';
										}
										else if (line[0] == 'z')
										{
											currentSignalActivity = 'z';
										}

										if (signalsIterator->SwitchingActivityCounter == -1)
										{
											signalsIterator->SwitchingActivityCounter++;
											signalsIterator->CurrentActivity = currentSignalActivity;
											foundSwitchingActivity = true;
										}
										else
										{
											if (signalsIterator->CurrentActivity != switchingActivitySignals[i])
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
		result = true;
	}

	return result;
}

