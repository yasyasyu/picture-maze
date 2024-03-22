#pragma once
# include "PictureMaze.hpp"



namespace InputSystem
{
	void FileInput(PictureMaze&);
};

class OutputSystem
{
private:
	String defaultFileName;

public:
	OutputSystem();

	Optional<FilePath> parentFolder;

	bool ParentFolderSelect();

	void FileOutPut(PictureMaze& pictureMaze, bool isOrigin = false);
	void FileSave(PictureMaze& pictureMaze, String folderPath, String fileName);
};
