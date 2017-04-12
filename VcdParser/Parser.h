#pragma once

struct Module
{
	std::string Name;
	std::vector<Signal> Signals;
	//unsigned Signals;
};

struct Signal
{
	std::string Name;
	std::string Symbol;
};

