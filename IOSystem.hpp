#pragma once
# include "PictureMaze.hpp"



class InputSystem
{
public:
	void FileInput(PictureMaze&);
};

class OutputSystem
{
private:
	Optional<FilePath> parentFolder;
	String defaultFileName;

public:
	OutputSystem();

	bool ParentFolderSelect();

	void FileOutPut(PictureMaze& pictureMaze, bool isOrigin = false);
	void FileSave(PictureMaze& pictureMaze, String folderPath, String fileName);
};
