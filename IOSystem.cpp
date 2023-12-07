
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
		if (not json || !json.hasElement(U"picture"))
		{
			System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
			return;
		}

		// picture
		if (!json[U"picture"].isArray())
		{
			System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
			return;
		}
		// picture
		{
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

		// around maze
		if (json.hasElement(U"maze"))
		{
			pictureMaze.isExistMaze = true;
			// maze
			{
				if (!json[U"maze"].isArray())
				{
					System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
					return;
				}
				auto loadGrid = json[U"maze"].arrayView();

				Grid<int> loadMazeGrid(pictureMaze.mazeGrid.size().y, pictureMaze.mazeGrid.size().x, 0);
				for (int i = 0; i < pictureMaze.mazeGrid.size().y; i++)
				{
					if (!loadGrid[i].isArray())
					{
						System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
						return;
					}

					auto row = loadGrid[i].arrayView();
					for (int j = 0; j < pictureMaze.mazeGrid.size().x; j++)
					{
						if(!row[j].isInteger())
						{
							System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
							return;
						}

						loadMazeGrid[i][j] = row[j].get<int>();
					}
				}

				pictureMaze.mazeGrid = loadMazeGrid;
			}

			// NgBorder
			{
				if (!json.hasElement(U"NgBorder"))
				{
					System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
					return;
				}

				const JSON loadGrid = json[U"NgBorder"];
				Grid<Array<bool>> loadNgBorder(pictureMaze.pictureGrid.size().y, pictureMaze.pictureGrid.size().x, Array<bool>(2));
				for (int i = 0; i < pictureMaze.pictureGrid.size().y; i++)
				{
					if (!loadGrid[i].isArray())
					{
						System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
						return;
					}

					const JSON row = loadGrid[i];
					for (int j = 0; j < pictureMaze.pictureGrid.size().x; j++)
					{
						if (!row[j].isArray())
						{
							System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
							return;
						}
						const JSON cell = row[j];
						for (int k = 0; k < 2; k++)
						{
							if (!cell[k].isBool())
							{
								System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
								return;
							}

							loadNgBorder[i][j][k] = cell[k].get<bool>();
						}
					}
				}

				pictureMaze.SetNgBorder(loadNgBorder);
			}

			// spanningTree
			{
				int vertex = pictureMaze.pictureGrid.size().y * pictureMaze.pictureGrid.size().x;
				Array<Array<int32>> loadSpanningTree(vertex);
				Array<Array<int32>> loadAnsSpanningTree(vertex);

				{ // ans
					if (!json.hasElement(U"spanningTree"))
					{
						System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
						return;
					}
					const JSON loadGrid = json[U"spanningTree"];
					for (int i = 0; i < vertex; i++)
					{
						if (!loadGrid[i].isArray())
						{
							System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
							return;
						}
						const JSON row = loadGrid[i];

						for (int j = 0; j < row.size(); j++)
						{
							loadSpanningTree[i] << row[j].get<int>();
						}
					}
				}

				{ // outs
					if (!json.hasElement(U"ansSpanningTree"))
					{
						System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
						return;
					}
					const JSON loadGrid = json[U"ansSpanningTree"];
					for (int i = 0; i < vertex; i++)
					{
						if (!loadGrid[i].isArray())
						{
							System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
							return;
						}
						const JSON row = loadGrid[i];

						for (int j = 0; j < row.size(); j++)
						{
							loadAnsSpanningTree[i] << row[j].get<int>();
						}
					}
				}

				pictureMaze.SetSpanningTree(loadAnsSpanningTree, loadSpanningTree);
			}

			// start, goal
			{
				{ // start
					if (!json.hasElement(U"start") || !json.hasElement(U"goal"))
					{
						System::MessageBoxOK(U"JSON ParseError", U"不正なファイルです。", MessageBoxStyle::Error);
						return;
					}
					
					Point start = json[U"start"].get<Point>();
					Point goal = json[U"goal"].get<Point>();
					pictureMaze.SetStartGoal(start, goal);
				}
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



		Grid<int> mazeGrid = pictureMaze.mazeGrid;
		Array <Array<int>> mazeArray(mazeGrid.size().y, Array<int>(mazeGrid.size().x, 0));
		for (int i = 0; i <mazeGrid.size().y; i++)
		{
			for (int j = 0; j < mazeGrid.size().x; j++)
			{
				mazeArray[i][j] = mazeGrid[i][j];
			}
		}
		json[U"maze"] = mazeArray;

		Grid <Array< bool >> ngBorderGrid = pictureMaze.GetNgBorder();
		Array<Array < Array< bool >>> ngBorder(
			ngBorderGrid.size().y,
			Array<Array<bool>>(ngBorderGrid.size().x,
				Array<bool>(2, false)
			)
		);
		for (int i = 0; i < ngBorderGrid.size().y; i++)
		{
			for (int j = 0; j < ngBorderGrid.size().x; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					ngBorder[i][j][k] = ngBorderGrid[i][j][k];
				}
			}
		}
		json[U"NgBorder"] = ngBorder;

		json[U"spanningTree"] = pictureMaze.spanningTree;
		json[U"ansSpanningTree"] = pictureMaze.ansSpanningTree;

		json[U"start"] = pictureMaze.start;
		json[U"goal"] = pictureMaze.goal;
	}

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
