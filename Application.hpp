#pragma once
# include "AppMode.hpp"

class Application
{
private:
	AppMode applicationMode;
	bool _init;
	bool _mouseReverse;
public:
	Application();

	AppMode mode();

	void ModeChange(AppMode);
	void ModeChange();

	bool init();

	void InitBreak();
	void MouseReverse();
	bool isReverse();
};
