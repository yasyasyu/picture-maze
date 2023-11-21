# include <Siv3D.hpp> // OpenSiv3D v0.6.10
# include "AppMode.hpp"
# include "PictureMaze.hpp"
# include "Application.hpp"

void Main()
{
	PictureMaze pictureMaze;
	Application app;
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
	while (System::Update())
	{
		pictureMaze.TextureScaled();
		switch (app.mode())
		{
		case AppMode::Paint:
			//描画モード
			Cursor::RequestStyle(CursorStyle::Hand);
			if (app.init())
			{
				pictureMaze.DrawGrid();
				app.InitBreak();
				pictureMaze.TextureFill(AppMode::Paint);
				previousMousePoint = Point(-1, -1);
			}

			if (pictureMaze.DrawDot(MouseL, previousMousePoint))
				pictureMaze.TextureFill(AppMode::Paint);

			if (pictureMaze.DrawDot(MouseR, previousMousePoint))
				pictureMaze.TextureFill(AppMode::Paint);

			if (pictureMaze.ChangeMazeMode())
			{
				app.ModeChange();
				break;
			}

			pictureMaze.ResetCanvas();

			break;

		case AppMode::Maze:
			// 迷路モード
			if (app.init() || pictureMaze.ReMaze())
			{
				auto [_ansSpanningTree, _spanningTree, start, goal]
					= MazeUtillity::CreateMaze(pictureMaze.pictureGrid, pictureMaze.mazeGrid);
				spanningTree = _ansSpanningTree;
				outSpanningTree = _spanningTree;
				pictureMaze.SetStartGoal(start, goal);
				pictureMaze.DrawMaze();
				app.InitBreak();
				pictureMaze.TextureFill(AppMode::Maze);
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
				app.ModeChange();
				break;
			}
			break;
		default:
			break;
		}
		pictureMaze.SaveImage();
	}
}
