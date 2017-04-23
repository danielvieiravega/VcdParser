#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

const unsigned int SwitchingActivitySignalsSize = 3;

struct Signal
{
	std::string Name;
	std::string Symbol;
	int SwitchingActivityCounter;
	char CurrentActivity;
};

struct Module
{
	std::string Name;
	std::vector<Signal> Signals;
};

class Parser
{
private:
	std::vector<Module> modules_;
	double clock_;
	unsigned clock_counter_;
	bool is_clock_calculated_;
	std::ifstream vcd_file_;
	std::string line_;
	std::string vcd_file_path_;

public:
	Parser();
	void AddModule(Module module);
	void ShowReport();
	void CalculeClockFrequency();
	bool Parse(std::string vcdFilePath);

	inline bool IsClockCalculated() const
	{
		return is_clock_calculated_;
	}

	inline std::vector<Module> GetModules() const
	{
		return modules_;
	}
};
