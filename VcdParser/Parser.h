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
	std::vector<Module> modules;
	double clock;
	unsigned clockCounter;
	bool isClockCalculated;
	std::ifstream vcdFile;
	std::string line;

public:
	Parser();
	void AddModule(Module module);
	void ShowReport();
	void CalculeClockFrequency();
	bool Parse(std::string vcdFilePath);

	inline bool IsClockCalculated() const
	{
		return isClockCalculated;
	}

	inline std::vector<Module> GetModules() const
	{
		return modules;
	}
};