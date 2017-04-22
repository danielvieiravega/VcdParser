#pragma once

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
