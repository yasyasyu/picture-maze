# include <Siv3D.hpp> // OpenSiv3D v0.6.10
# include "AppMode.hpp"
# include "Application.hpp"
# include "PictureMaze.hpp"
# include "IOSystem.hpp"

void Main()
{
	PictureMaze pictureMaze;
	Application application = Application();
	// Window Size
	Window::Resize(pictureMaze.WindowSize());

	// BackGround Color
	Scene::SetBackground(Palette::Peachpuff);
	const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };

	// Image Canvas
	pictureMaze.ResetCanvas();

	Point previousMousePoint = Point(-1, -1);
	OutputSystem output = OutputSystem();
	Reseed((uint64)time(NULL));
	while (System::Update())
	{
		pictureMaze.TextureScaled();
		switch (application.mode())
		{
		case AppMode::Paint:
			//描画モード
			Cursor::RequestStyle(CursorStyle::Hand);
			if (application.init())
			{
				pictureMaze.DrawGrid();
				application.InitBreak();
				pictureMaze.TextureFill(AppMode::Paint);
				previousMousePoint = Point(-1, -1);
			}

			if (pictureMaze.DrawDot(MouseL, previousMousePoint, application.isReverse()))
			{
				pictureMaze.TextureFill(AppMode::Paint);
				pictureMaze.isExistMaze = false;
			}

			if (pictureMaze.DrawDot(MouseR, previousMousePoint, application.isReverse()))
			{
				pictureMaze.TextureFill(AppMode::Paint);
				pictureMaze.isExistMaze = false;
			}

			if (pictureMaze.MouseReverse(application.isReverse()))
			{
				application.MouseReverse();
			}

			if (pictureMaze.ChangeMazeMode())
			{
				application.ModeChange();
				break;
			}

			pictureMaze.ResetCanvas();

			break;

		case AppMode::Maze:
			// 迷路モード
			if (application.init())
			{
				application.InitBreak();
				if (!pictureMaze.isExistMaze)
				{
					auto [ansSpanningTree, spanningTree, start, goal, ngBorder]
						= MazeUtillity::CreateMaze(pictureMaze.pictureGrid, pictureMaze.mazeGrid);
					pictureMaze.SetSpanningTree(ansSpanningTree, spanningTree);
					pictureMaze.SetStartGoal(start, goal);
					pictureMaze.SetNgBorder(ngBorder);
					pictureMaze.isExistMaze = true;
				}

				pictureMaze.DrawMaze();
				pictureMaze.TextureFill(AppMode::Maze);
			}

			if (pictureMaze.ReMaze())
			{
				auto [ansSpanningTree, spanningTree, start, goal, ngBorder]
					= MazeUtillity::CreateMaze(pictureMaze.pictureGrid, pictureMaze.mazeGrid);
				pictureMaze.SetSpanningTree(ansSpanningTree, spanningTree);
				pictureMaze.SetStartGoal(start, goal);
				pictureMaze.SetNgBorder(ngBorder);
				pictureMaze.isExistMaze = true;

				pictureMaze.DrawMaze();
				pictureMaze.TextureFill(AppMode::Maze);
			}

			if (pictureMaze.PrintSpanningTreeButton())
			{
				pictureMaze.PrintSpanningTree();
				pictureMaze.PrintNgBorder();
			}

			pictureMaze.SolveMaze();
			pictureMaze.VisualizeRoute();
			pictureMaze.VisualizeFrameSpeedSlider();
			if (pictureMaze.ReturnPaint())
			{
				application.ModeChange();
				break;
			}
			break;
		default:
			break;
		}

		if (pictureMaze.LoadFile())
		{
			InputSystem::FileInput(pictureMaze);
			pictureMaze.TextureFill(AppMode::Paint);
			application.ModeChange(AppMode::Paint);
		}

		pictureMaze.ParentFolderText(output.parentFolder);

		if (pictureMaze.SaveFile())
		{
			output.FileOutPut(pictureMaze);
		}
		
		if (pictureMaze.SaveAsOriginFile())
		{
			output.FileOutPut(pictureMaze, true);
		}
	}
}
