
#include "IOSystem.hpp"


String DefaultFileName::SerialFileName(String fileName, int number) {
	return fileName + U"({})"_fmt(number);
}

void InputSystem::InputFile()
{

}

OutputSystem::OutputSystem()
{
	this->createCount = 0;
	this->defaultFileName = U"PictureMaze";
}

void OutputSystem::IncrementCreateCount()
{
	this->createCount++;
}

bool OutputSystem::ParentFolderSelect()
{
	if (!this->parentFolder.has_value())
	{
		this->parentFolder = Dialog::SelectFolder();
	}
	Print << this->parentFolder;

	if (this->parentFolder)
	{
		return true;
	}

	return false;
}
/*/
void OutputSystem::FileOutPut(PictureMaze& pictureMaze, String fileName)
{

	if (!this->ParentFolderSelect())
	{
		return;
	}

	FilePath folderPath = FileSystem::PathAppend(FileSystem::FullPath(this->parentFolder.value()), fileName);
	if (!FileSystem::CreateDirectories(folderPath))
	{
		// error
		return;
	}
	pictureMaze.pictureImage.save(
		FileSystem::PathAppend(folderPath, fileName + FileName::pictureImageSuffix), ImageFormat::PNG
	);
	pictureMaze.mazeImage.save(
		FileSystem::PathAppend(folderPath, fileName + FileName::mazeImageSuffix), ImageFormat::PNG
	);

	JSON json;
	json[U"seed"] = pictureMaze.seed;
	json.saveMinimum(
		FileSystem::PathAppend(folderPath, fileName + FileName::infoJsonSuffix)
	);
}
*/
void OutputSystem::DefaultFileOutPut(
	//PictureMaze pictureMaze
)
{
	//this->FileOutPut(pictureMaze, FileName::SerialFileName(application.defaultFileName, this->createCount));

	if (!this->ParentFolderSelect())
	{
		return;
	}

	String fileName = DefaultFileName::SerialFileName(this->defaultFileName, this->createCount);

	FilePath folderPath = FileSystem::PathAppend(FileSystem::FullPath(this->parentFolder.value()), fileName);
	if (!FileSystem::CreateDirectories(folderPath))
	{
		// error
		return;
	}
	Print << U"Save";
	//pictureMaze.pictureImage.save(
	//	FileSystem::PathAppend(folderPath, fileName + DefaultFileName::pictureImageSuffix), ImageFormat::PNG
	//);
	//pictureMaze.mazeImage.save(
	//	FileSystem::PathAppend(folderPath, fileName + DefalutFileName::mazeImageSuffix), ImageFormat::PNG
	//);

	//JSON json;
	//json[U"seed"] = pictureMaze.SeedOutput();
	//json.saveMinimum(
	//	FileSystem::PathAppend(folderPath, fileName + DefalutFileName::infoJsonSuffix)
	//);

	this->IncrementCreateCount();
}
