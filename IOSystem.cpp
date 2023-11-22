
#include "PictureMaze.hpp"
#include "IOSystem.hpp"
class InputSystem
{
public:
	InputSystem()
	{

	}
};




void OutputSystem::OutPutFile(PictureMaze& pictureMaze)
{
	JSON json;
	//this->pictureGrid = pictureMaze.pictureGrid;
	if (this->selectFolder.has_value())
	{
		this->selectFolder = Dialog::SelectFolder();
	}
	Print << this->selectFolder.value();
	// ファイル名指定



	json[U"seed"] = pictureMaze.SeedOutput();
	pictureMaze.pictureImage.save(path);
	pictureMaze.mazeImage.save(path);
	json.saveMinimum(U"");
}
