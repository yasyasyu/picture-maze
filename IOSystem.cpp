
#include "IOSystem.hpp"

namespace DefaultFileName
{

	String PictureImageSuffix() { return U"_picture.png"; }
	String MazeImageSuffix() { return U"_maze.png"; }
	String AnsImageSuffix() { return U"_ans.png"; }
	String InfoJsonSuffix() { return U"_info.json"; }

	String SerialFileName(String fileName, int number)
	{
		return fileName + U"({})"_fmt(number);
	}
};

namespace InputSystem {
	void FileInput(PictureMaze& pictureMaze)
	{
		Optional<FilePath> loadJsonFile = Dialog::OpenFile({ FileFilter::JSON() });
		if (!loadJsonFile.has_value())
		{
			return;
		}
		const JSON json = JSON::Load(loadJsonFile.value());
		// TODO error処理？
		// keyの存在、valueのデータ・構造
		if (not json || !json.hasElement(U"picture"))
		{
			System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
			return;
		}

		if (!json[U"picture"].isArray())
		{
			System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
			return;
		}

		auto loadGrid = json[U"picture"].arrayView();
		Array<Array<bool>> loadPictureGrid(pictureMaze.pictureGrid.size().y, Array<bool>(pictureMaze.pictureGrid.size().x, false));
		for (int i = 0; i < pictureMaze.pictureGrid.size().y; i++)
		{
			if (!loadGrid[i].isArray())
			{
				System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
				return;
			}

			auto row = loadGrid[i].arrayView();
			for (int j = 0; j < pictureMaze.pictureGrid.size().x; j++)
			{
				if (!row[j].isBool())
				{
					System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
					return;
				}

				loadPictureGrid[i][j] = row[j].get<bool>();
			}
		}

		for (int i = 0; i < pictureMaze.pictureGrid.size().y; i++)
		{
			for (int j = 0; j < pictureMaze.pictureGrid.size().x; j++)
			{
				pictureMaze.pictureGrid[i][j] = loadPictureGrid[i][j];
				pictureMaze.UpdateDot(Point(j, i), (loadPictureGrid[i][j] ? PALETTE[1] : PALETTE[0]));
			}
		}
	}
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
	// File重複ずらし
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

	{// picture
		pictureMaze.pictureImage.save(
			FileSystem::PathAppend(folderPath, fileName + DefaultFileName::PictureImageSuffix()), ImageFormat::PNG
		);
	}

	if (pictureMaze.isRoute)
	{
		// ans
		pictureMaze.VisualizeRoute(2);
		pictureMaze.mazeImage.save(
			FileSystem::PathAppend(folderPath, fileName + DefaultFileName::AnsImageSuffix()), ImageFormat::PNG
		);
	}

	if (pictureMaze.isExistMaze)
	{
		// maze
		pictureMaze.VisualizeRoute(1);
		pictureMaze.mazeImage.save(
			FileSystem::PathAppend(folderPath, fileName + DefaultFileName::MazeImageSuffix()), ImageFormat::PNG
		);
	}


	JSON json;
	Array<Array<bool>> outPictureGrid(pictureMaze.pictureGrid.size().y, Array<bool>(pictureMaze.pictureGrid.size().x, false));
	for (int i = 0; i < pictureMaze.pictureGrid.size().y; i++)
	{
		for (int j = 0; j < pictureMaze.pictureGrid.size().x; j++)
		{
			outPictureGrid[i][j] = pictureMaze.pictureGrid[i][j];
		}
	}

	json[U"picture"] = outPictureGrid;
	json.saveMinimum(
		FileSystem::PathAppend(folderPath, fileName + DefaultFileName::InfoJsonSuffix())
	);
}

void OutputSystem::FileOutPut(PictureMaze& pictureMaze, bool isOrigin)
{
	Optional<FilePath> saveFolder;
	String fileName;

	if (isOrigin)
	{
		Optional<FilePath> filePath = Dialog::SaveFile({});
		if (!filePath.has_value())
		{
			return;
		}
		saveFolder = FileSystem::ParentPath(filePath.value());
		fileName = FileSystem::BaseName(filePath.value());

	}
	else {
		if (!this->ParentFolderSelect())
		{
			return;
		}
		saveFolder = this->parentFolder;
		fileName = this->defaultFileName;
	}

	this->FileSave(
		pictureMaze,
		FileSystem::PathAppend(FileSystem::FullPath(saveFolder.value()), fileName),
		fileName
	);
}
