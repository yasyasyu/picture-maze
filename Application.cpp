# include "Application.hpp"

AppMode Application::mode() const
{
	return applicationMode;
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

bool Application::init() const {
	return _init;
}

void Application::InitBreak()
{
	_init = false;
}
