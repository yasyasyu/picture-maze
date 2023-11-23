# include "Application.hpp"

Application::Application()
{
	this->applicationMode = AppMode::Paint;
	this->_init = true;
}

AppMode Application::mode()
{
	return applicationMode;
}

void Application::ModeChange(AppMode mode)
{
	if (applicationMode != mode)
	{
		_init = true;
	}
	applicationMode = mode;
}

void Application::ModeChange()
{
	_init = true;
	switch (applicationMode)
	{
	case AppMode::Paint:
		applicationMode = AppMode::Maze;
		break;
	case AppMode::Maze:
		applicationMode = AppMode::Paint;
		break;
	default:
		break;
	}
}

bool Application::init() {
	return _init;
}

void Application::InitBreak()
{
	_init = false;
}
