# include "Application.hpp"

Application::Application()
{
	this->applicationMode = AppMode::Paint;
	this->_init = true;
	this->_mouseReverse = false;
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
		this->_mouseReverse = false;
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

void Application::MouseReverse()
{
	this->_mouseReverse = !this->_mouseReverse;
}
bool Application::isReverse()
{
	return this->_mouseReverse;
}
