#pragma once
# include "PictureMaze.hpp"

class InputSystem
{
public:
	InputSystem();
};

class OutputSystem
{
private:
	Grid<bool> pictureGrid;
	Optional<FilePath> selectFolder;

public:
	void OutPutFile(PictureMaze&);
};
