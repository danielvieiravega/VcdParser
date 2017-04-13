#pragma once

struct Signal
{
	std::string Name;
	std::string Symbol;
	unsigned SwitchingActivityCounter;
	std::string CurrentActivity;
};

struct Module
{
	std::string Name;
	std::vector<Signal> Signals;
};
