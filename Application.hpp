#pragma once
# include "AppMode.hpp"

class Application
{
private:
	AppMode applicationMode = AppMode::Paint;
	bool _init = true;

public:
	AppMode mode() const;

	void ModeChange();

	bool init() const;

	void InitBreak();
};
