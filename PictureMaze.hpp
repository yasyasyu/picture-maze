#pragma once
# include "MazeUtility.hpp"
# include "AppMode.hpp"
# include "Application.hpp"

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



	DynamicTexture texture = DynamicTexture(pictureImage);

	bool spanningTreeView = false;

	Point start = Point(0, 0), goal = Point(0, 0);
	Array<Point> ansRoute = {};

	void SetRoute(const Array<Point>& route);
	void SetSeed();
	void SetSeed(uint64);

	void DrawStartGoal();



	int index = 0;
	int count = 0;
	int visualizeFrame = 20;
	int overCount = 0;

	int visualSpanningTreeFlag = 0;

	bool isRandomizeSeed = false;

	TextEditState seedText;
	bool isRandomSeed = true;
	uint64 seed;

public:
	Image pictureImage = Image(CELL_CNT * FIELD_WIDTH, CELL_CNT * FIELD_HEIGHT, Palette::White);
	Image mazeImage = Image(CELL_CNT * FIELD_WIDTH, CELL_CNT * FIELD_HEIGHT, Palette::White);

	Grid<bool> pictureGrid = Grid<bool>(FIELD_WIDTH, FIELD_HEIGHT, false);
	Grid<int> mazeGrid = Grid<int>(FIELD_WIDTH * 2, FIELD_HEIGHT * 2, 0);

	Point WindowSize() const;

	Color ansSpanningColor = PALETTE[3];
	Color outAnsSpanningColor = PALETTE[5];


	int isExistMaze = false;
	bool isRoute = false;

	uint64 GetSeed() { return seed; };

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

	void UpdateDot(Point target, Color color);
	bool DrawDot(const Input& mouse, Point& previousMousePoint);

	bool LoadFile();
	bool SaveFile();
	bool SaveAsOriginFile();

	void ResetCanvas();

	bool PrintSpanningTreeButton();

	void RandomCheckBox();

	void SeedInputBox(bool isActive);
	void SeedInput(String);
	String SeedOutput();

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

	void VisualizeRoute(int isFull = 0);

	/**
	 * @fn 迷路モードにする。
	 * @param[in] pictureGrid 連結グラフ判定用グリッド
	 * @return bool 迷路モードにするかどうか
	 */
	bool ChangeMazeMode();


	void MazeTerminate();
};
