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
	Array<Array<int32>> spanningTree;
	Array<Array<int32>> outSpanningTree;
	Point previousMousePoint = Point(-1, -1);
	OutputSystem output = OutputSystem();

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

			if (pictureMaze.DrawDot(MouseL, previousMousePoint))
				pictureMaze.TextureFill(AppMode::Paint);

			if (pictureMaze.DrawDot(MouseR, previousMousePoint))
				pictureMaze.TextureFill(AppMode::Paint);

			if (pictureMaze.ChangeMazeMode())
			{
				application.ModeChange();
				break;
			}

			pictureMaze.ResetCanvas();

			break;

		case AppMode::Maze:
			// 迷路モード
			if (application.init() || pictureMaze.ReMaze())
			{
				auto [_ansSpanningTree, _spanningTree, start, goal]
					= MazeUtillity::CreateMaze(pictureMaze.pictureGrid, pictureMaze.mazeGrid, pictureMaze.seed);
				spanningTree = _ansSpanningTree;
				outSpanningTree = _spanningTree;
				pictureMaze.SetStartGoal(start, goal);
				pictureMaze.DrawMaze();
				application.InitBreak();
				pictureMaze.TextureFill(AppMode::Maze);
				pictureMaze.MazeTerminate();
			}
			if (pictureMaze.PrintSpanningTreeButton())
			{
				pictureMaze.PrintSpanningTree(spanningTree, pictureMaze.ansSpanningColor);
				pictureMaze.PrintSpanningTree(outSpanningTree, pictureMaze.outAnsSpanningColor);
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
		if (pictureMaze.SaveFile())
		{
			output.DefaultFileOutPut(
				//pictureMaze
			);
		}
		pictureMaze.RandomCheckBox();
		pictureMaze.SeedInputBox(application.mode() == AppMode::Paint);
	}
}
