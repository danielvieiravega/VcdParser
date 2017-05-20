#include "Parser.h"
#include <algorithm>
#include <iomanip>

/**
 * \brief Splits the given string by space in a vector
 * \param str string to be splitted
 * \return A vector containing the the splitted line_
 */
std::vector<std::string> SplitBySpace(std::string str)
{
	std::string buf;
	std::stringstream ss(str);

	std::vector<std::string> tokens;

	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}

/**
 * \brief Constructor
 */
Parser::Parser()
	: clock_(0), clock_counter_(0), is_clock_calculated_(false)
{
}

/**
 * \brief Adds the given module in the modules_ vector
 * \param module Module to be added to the modules_ vector
 */
void Parser::AddModule(Module module)
{
	modules_.push_back(module);
}

/**
 * \brief Displays the report of the parsed data
 */
void Parser::ShowReport()
{
	std::cout << "-########################################-" << std::endl;
	std::cout << "-###  Value Change Dump (VCD) Parser  ###-" << std::endl;
	std::cout << "-########################################-" << std::endl;
	std::cout << std::endl;

	std::cout << "----------------------------------------------------------------------------" << std::endl;
	std::cout << "VCD File Path: " << vcd_file_path_ << std::endl;
	std::cout << "Simulation Date: " << simulation_date_ << std::endl;
	std::cout << "Version: " << version_ << std::endl;
	std::cout << "Clock frequency: " << clock_ << " Hz" << std::endl;
	std::cout << "----------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;

	for (auto &module : modules_)
	{
		std::cout << "Module: " << module.Name << std::endl;
		std::cout << "Signals count: " << module.SignalCounter << std::endl << std::endl;

		std::cout << std::setw(20) << std::left << "Name" << std::setw(30) << std::left << "Switching activity" << std::endl;
		std::cout << std::setw(20) << std::left << "----" << std::setw(30) << std::left << "------------------" << std::endl;
		for (auto &signal : module.Signals)
		{	
			std::cout << std::setw(20) << std::left << signal.Name << std::setw(30) << std::left <<((signal.SwitchingActivityCounter > 0) ? signal.SwitchingActivityCounter : 0) << std::endl;
		}

		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << std::endl;
	}
		
	std::cout << "--------End report--------" << std::endl;
}

double Parser::GetTimeScale() const
{
	auto scale = 10e-9;
	if (time_scale_.compare("1ns") == 0)
		scale = 10e-9;
	else if (time_scale_.compare("1us") == 0)
		scale = 10e-6;
	else if (time_scale_.compare("1ms") == 0)
		scale = 10e-3;

	return scale;
}

/**
 * \brief Calculates the clock frequency 
 */
void Parser::CalculeClockFrequency()
{
	if ((line_.size() > 0) && (line_.at(0) == '#') && (clock_counter_ < 2))
	{	
		std::string::size_type sz;
		auto numberStr = line_.substr(1, std::string::npos);

		if (clock_counter_ == 1)
		{
			auto period = std::stoi(numberStr, &sz);
			clock_ = 1.0 / (period*GetTimeScale());
			is_clock_calculated_ = true;
		}

		clock_counter_++;
	}
}

void Parser::RemoveTabFromString(std::string& str) const
{
	str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
}

/**
 * \brief Parses the Date when the simulation was executed
 */
void Parser::ParseDate()
{
	if (line_.find("$date") != std::string::npos)
	{
		while (getline(vcd_file_, line_))
		{
			if (line_.find(":") != std::string::npos)
			{
				if (line_.find('\t') != std::string::npos)
				{
					simulation_date_ = line_;
					RemoveTabFromString(simulation_date_);
				}
				else
				{
					simulation_date_ = line_;
				}
				break;
			}
		}
	}
}


/**
 * \brief Parses the Simulation tool version
 */
void Parser::ParseVersion()
{
	if (line_.find("$version") != std::string::npos)
	{
		while (getline(vcd_file_, line_))
		{
			if (line_.find("Version") != std::string::npos)
			{
				if (line_.find('\t') != std::string::npos)
				{
					version_ = line_;
					RemoveTabFromString(version_);
				}
				else
				{
					version_ = line_;
				}
				break;
			}
		}
	}
}


/**
 * \brief Parses the time scale used for the simulation
 */
void Parser::ParseTimeScale()
{
	if(line_.find("$timescale") != std::string::npos)
	{
		while (getline(vcd_file_, line_))
		{
			if ((line_.find("1") != std::string::npos) || (line_.find("s") != std::string::npos))
			{
				time_scale_ = line_;
				break;
			}
		}
	}
}

/**
 * \brief Parses the provide .vcd file
 * \param vcdFilePath Full path of .vcd file
 * \return true whether the parsing was succesful else false
 */
bool Parser::Parse(std::string vcdFilePath)
{
	auto result = false;
	vcd_file_.open(vcdFilePath);

	if (vcd_file_.is_open())
	{
		vcd_file_path_ = vcdFilePath;
		while (getline(vcd_file_, line_))
		{
			ParseDate();

			ParseVersion();

			ParseTimeScale();

			if (line_.find("$scope") != std::string::npos)
			{
				auto splittedLine = SplitBySpace(line_);
				Module module;
				module.Name = splittedLine[2];

				while (getline(vcd_file_, line_))
				{
					if (line_.find("$var") != std::string::npos)
					{
						Signal signal;
						signal.SwitchingActivityCounter = -1;
						auto splittedLineSignal = SplitBySpace(line_);
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
			else if (line_.find("$dumpvars") != std::string::npos)
			{
				char switchingActivitySignals[SwitchingActivitySignalsSize] = { '0', '1', 'z' };

				auto foundSwitchingActivity = false;

				while (getline(vcd_file_, line_))
				{
					if (!IsClockCalculated())
						CalculeClockFrequency();

					if ((line_[0] == '0') || (line_[0] == '1') || (line_[0] == 'z'))
					{
						foundSwitchingActivity = false;
						
						for (std::vector<Module>::iterator modulesIterator = modules_.begin();
							modulesIterator != modules_.end() && !foundSwitchingActivity;
							++modulesIterator)
						{
							for (std::vector<Signal>::iterator signalsIterator = modulesIterator->Signals.begin();
								signalsIterator != modulesIterator->Signals.end() && !foundSwitchingActivity;
								++signalsIterator)
							{
								for (auto i = 0; i < SwitchingActivitySignalsSize && !foundSwitchingActivity; i++)
								{
									auto lineSubStr = line_.substr(1, line_.size());

									if (lineSubStr == signalsIterator->Symbol)
									{
										char currentSignalActivity = ' ';
										if (line_[0] == '0')
										{
											currentSignalActivity = '0';
										}
										else if (line_[0] == '1')
										{
											currentSignalActivity = '1';
										}
										else if (line_[0] == 'z')
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

		vcd_file_.close();
		CalculateSignalCounter();
		result = true;
	}

	return result;
}

void Parser::CalculateSignalCounter()
{
	for (auto &module : modules_)
	{
		unsigned signalCounter = 0;
		auto previousSignal = module.Signals[0];
		for (auto it = module.Signals.begin(); it != module.Signals.end(); ++it)
		{	
			if (signalCounter == 0)
			{
				signalCounter++;
			}
			else
			{
				if (it->Name.find(" ") != std::string::npos)
				{
					auto signalName = SplitBySpace(it->Name)[0];
					if (previousSignal.Name.find(" ") != std::string::npos)
					{
						auto previossignalName = SplitBySpace(previousSignal.Name)[0];
						if (previossignalName.compare(signalName) != 0)
						{
							signalCounter++;
						}
					}
					else
					{
						signalCounter++;
					}
				}					
				else
				{
					signalCounter++;
				}
			}
			previousSignal = *it;
		}
		module.SignalCounter = signalCounter;
	}
}
