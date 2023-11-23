#pragma once
# include "AppMode.hpp"

class Application
{
private:
	AppMode applicationMode;
	bool _init;
public:
	Application();

	AppMode mode();

	void ModeChange();

	bool init();

	void InitBreak();
};
