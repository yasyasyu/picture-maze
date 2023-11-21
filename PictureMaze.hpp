#pragma once
# include "MazeUtility.hpp"
# include "AppMode.hpp"

class PictureMaze
{
private:
	int CELL_SIZE = 3;
	int CELL_CNT = 8;
	int HARF_CELL_CNT = CELL_CNT / 2 + CELL_CNT % 2;

	int FIELD_HEIGHT = 20;
	int FIELD_WIDTH = 20;

	int FIELD_OFFSET_LEFT = 8;
	int FIELD_OFFSET_UP = 8;

	Point FIELD_OFFSET = Point(
		FIELD_OFFSET_LEFT,
		FIELD_OFFSET_UP
	);

	int FIELD_OFFSET_RIGHT = 200;
	int FIELD_OFFSET_DOWN = 8;

	int BUTTON_HEIGHT = 30;
	int BUTTON_LEFT_PADDING = 30;
	int BUTTON_PADDING = 20;



	Image pictureImage = Image(CELL_CNT * FIELD_WIDTH, CELL_CNT * FIELD_HEIGHT, Palette::White);
	Image mazeImage = Image(CELL_CNT * FIELD_WIDTH, CELL_CNT * FIELD_HEIGHT, Palette::White);
	DynamicTexture texture = DynamicTexture(pictureImage);

	bool spanningTreeView = false;

	Point start = Point(0, 0), goal = Point(0, 0);
	Array<Point> ansRoute = {};

	void SetRoute(const Array<Point>& route);

	bool isRoute = false;


	int index = 0;
	int count = 0;
	int visualizeFrame = 20;
	int overCount = 0;

	int visualSpanningTreeFlag = 0;

public:
	Grid<bool> pictureGrid = Grid<bool>(FIELD_WIDTH, FIELD_HEIGHT, false);
	Grid<int> mazeGrid = Grid<int>(FIELD_WIDTH * 2, FIELD_HEIGHT * 2, 0);

	Point WindowSize() const;

	Color ansSpanningColor = PALETTE[3];
	Color outAnsSpanningColor = PALETTE[5];

	void TextureScaled();

	void TextureFill(AppMode application);

	void DrawGrid();

	void SetStartGoal(Point _start, Point _goal);

	/**
	 * @fn 描画モードに戻る。
	 * @param[out] image 描画キャンバス
	 * @param[in] pictureGrid 描画用のグリッド
	 * @return bool 描画モードにできるか。
	 */
	bool ReturnPaint();

	void SolveMaze();

	bool ReMaze();

	bool DrawDot(const Input& mouse, Point& previousMousePoint);

	void SaveImage();

	void ResetCanvas();

	bool PrintSpanningTreeButton();

	/**
	 * @fn 全域木を盤面上に表示。
	 * @brief 全域木を迷路上に表示する。
	 * @param[in] (spanningTree) 全域木
	 */
	void PrintSpanningTree(Array<Array<int32>> spanningTree, Color color);

	void DrawMaze();

	void DrawRouteDot(const Point& drawPoint);

	void DrawRouteBetweenDot(const Point& drawPointFrom, const Point& drawPointTo);

	void VisualizeFrameSpeedSlider();

	void VisualizeRoute();

	/**
	 * @fn 迷路モードにする。
	 * @param[in] pictureGrid 連結グラフ判定用グリッド
	 * @return bool 迷路モードにするかどうか
	 */
	bool ChangeMazeMode();

};
