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

	Array<Point> ansRoute = {};

	void DrawStartGoal();

	// 境界辺
	Grid< Array<bool>> ngBorder;

	int index = 0;
	int count = 0;
	int visualizeFrame = 20;
	int overCount = 0;

	int visualSpanningTreeFlag = 0;

	Font parentFolderFont{ FontMethod::MSDF,48 };
	Rect parentFolderRect;

public:
	PictureMaze();
	// Get FieldSize from IniFile
	int FIELD_HEIGHT, FIELD_WIDTH;

	Image pictureImage, mazeImage;

	Grid<bool> pictureGrid;
	Grid<int> mazeGrid;

	Point WindowSize() const;

	Color ansSpanningColor = PALETTE[9];
	Color outAnsSpanningColor = PALETTE[6];

	void SetSpanningTree(Array<Array<int32>> ans, Array<Array<int32>> out);

	Array<Array<int32>> spanningTree, ansSpanningTree;
	Point start = Point(0, 0), goal = Point(0, 0);
	void SetRoute(const Array<Point>& route);

	bool isExistMaze = false;
	bool isRoute = false;

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
	bool DrawDot(const Input& mouse, Point& previousMousePoint, bool isReverse);

	bool LoadFile();
	bool SaveFile();
	bool SaveAsOriginFile();

	bool MouseReverse(bool isReverseMouse);

	void ParentFolderText(Optional<String> parentFolder);

	void ResetCanvas();

	bool PrintSpanningTreeButton();

	Grid< Array<bool>> GetNgBorder();
	void SetNgBorder(Grid<Array<bool>>);

	/**
	 * @fn 全域木を盤面上に表示。
	 * @brief 全域木を迷路上に表示する。
	 * @param[in] (spanningTree) 全域木
	 */
	void PrintSpanningTree();
	void PrintNgBorder();

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
};
