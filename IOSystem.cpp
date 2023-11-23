
#include "IOSystem.hpp"

namespace DefaultFileName
{

	String PictureImageSuffix(){ return U"_picture.png";}
	String MazeImageSuffix(){ return U"_maze.png";}
	String InfoJsonSuffix() { return U"_info.json"; }

	String SerialFileName(String fileName, int number)
	{
		return fileName + U"({})"_fmt(number);
	}
};

void InputSystem::InputFile()
{

}

OutputSystem::OutputSystem()
{
	this->defaultFileName = U"PictureMaze";
}

bool OutputSystem::ParentFolderSelect()
{
	if (!this->parentFolder.has_value())
	{
		this->parentFolder = Dialog::SelectFolder();
	}

	if (this->parentFolder)
	{
		return true;
	}

	return false;
}

void OutputSystem::FileSave(PictureMaze& pictureMaze, FilePath folderPath, String fileName)
{
	//TODO File重複ずらし
	if (FileSystem::Exists(folderPath))
	{
		FilePath tmpFolderPath;
		int number = 1;
		do {
			tmpFolderPath = DefaultFileName::SerialFileName(folderPath, number);
			number++;
		} while (FileSystem::Exists(tmpFolderPath));

		folderPath = tmpFolderPath;
		fileName = FileSystem::BaseName(folderPath);
	}

	if (!FileSystem::CreateDirectories(folderPath))
	{
		// error
		return;
	}

	Print << folderPath + U"/" + fileName + U"_";

	pictureMaze.pictureImage.save(
		FileSystem::PathAppend(folderPath, fileName + DefaultFileName::PictureImageSuffix()), ImageFormat::PNG
	);
	pictureMaze.mazeImage.save(
		FileSystem::PathAppend(folderPath, fileName + DefaultFileName::MazeImageSuffix()), ImageFormat::PNG
	);

	JSON json;
	json[U"seed"] = pictureMaze.SeedOutput();
	json.saveMinimum(
		FileSystem::PathAppend(folderPath, fileName + DefaultFileName::InfoJsonSuffix())
	);
}

void OutputSystem::FileOutPut(PictureMaze& pictureMaze, bool isOrigin)
{
	Optional<FilePath> parentFolder;
	String fileName;

	if (isOrigin)
	{
		Optional<FilePath> filePath = Dialog::SaveFile({});
		if (!filePath.has_value())
		{
			return;
		}
		parentFolder = FileSystem::ParentPath(filePath.value());
		fileName = FileSystem::BaseName(filePath.value());

	}
	else{
		if (!this->ParentFolderSelect())
		{
			return;
		}
		parentFolder = this->parentFolder;
		fileName = this->defaultFileName;
	}

	this->FileSave(
		pictureMaze,
		FileSystem::PathAppend(FileSystem::FullPath(parentFolder.value()), fileName),
		fileName
	);
	Print << U"Save";
}
