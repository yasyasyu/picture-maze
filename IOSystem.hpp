#pragma once

namespace DefaultFileName
{

	const String pictureImageSuffix = U"_picture";
	const String mazeImageSuffix = U"_maze";
	const String infoJsonSuffix = U"_info";

	String SerialFileName(String fileName, int number);
};

class InputSystem
{
public:
	void InputFile();
};

class OutputSystem
{
private:
	Optional<FilePath> parentFolder;
	int createCount;
	void IncrementCreateCount();
	String defaultFileName;

public:
	OutputSystem();

	bool ParentFolderSelect();

	void DefaultFileOutPut(
		//PictureMaze pictureMaze
	);

	//void FileOutPut(PictureMaze& pictureMaze, String fileName);
};
