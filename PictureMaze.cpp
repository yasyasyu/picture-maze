# include "PictureMaze.hpp"

void PictureMaze::SetRoute(const Array<Point>& route)
{
	isRoute = true;
	this->ansRoute = route;
}

Point PictureMaze::WindowSize() const {
	return Point(
		CELL_SIZE * FIELD_WIDTH * CELL_CNT + FIELD_OFFSET_LEFT + FIELD_OFFSET_RIGHT,
		CELL_SIZE * FIELD_HEIGHT * CELL_CNT + FIELD_OFFSET_UP + FIELD_OFFSET_DOWN
	);
}

void PictureMaze::TextureScaled()
{
	texture.scaled(CELL_SIZE).draw(FIELD_OFFSET_LEFT, FIELD_OFFSET_UP);
}

void PictureMaze::TextureFill(AppMode application)
{
	switch (application)
	{
	case AppMode::Paint:
		texture.fill(pictureImage);
		break;
	case AppMode::Maze:
		DrawStartGoal();
		texture.fill(mazeImage);
		break;
	default:
		break;
	}
}

void PictureMaze::DrawGrid()
{
	for (const auto& p : step({ FIELD_WIDTH, FIELD_HEIGHT }))
	{
		RectF(p * CELL_CNT, CELL_CNT).overwriteFrame(pictureImage, 1, 1, WALL_COLOR);
	}
}

void PictureMaze::SetStartGoal(Point _start, Point _goal) {
	this->start = _start;
	this->goal = _goal;
}

void PictureMaze::DrawStartGoal()
{
	Color paintColor[] = { PALETTE[2], PALETTE[5] };
	Point drawPoint[] = { this->start, this->goal };
	for (int p = 0; p < 2; p++)
	{
		for (int32 i = drawPoint[p].y * HARF_CELL_CNT + 1; i < (drawPoint[p].y + 1) * HARF_CELL_CNT - 1; i++)
		{
			for (int32 j = drawPoint[p].x * HARF_CELL_CNT + 1; j < (drawPoint[p].x + 1) * HARF_CELL_CNT - 1; j++)
			{

				mazeImage[i][j] = paintColor[p];
			}
		}
	}
}

/**
	* @fn 描画モードに戻る。
	* @param[out] image 描画キャンバス
	* @param[in] pictureGrid 描画用のグリッド
	* @return bool 描画モードにできるか。
	*/
bool PictureMaze::ReturnPaint()
{
	if (SimpleGUI::Button(U"ReturnPaint",
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 0
		}, 120))
	{
		for (int32 y = 0; y < FIELD_HEIGHT; y++)
		{
			for (int32 x = 0; x < FIELD_WIDTH; x++)
			{
				Color pictureColor = pictureGrid[y][x] ? PALETTE[1] : PALETTE[0];
				for (int32 i = y * CELL_CNT + 1; i < (y + 1) * CELL_CNT - 1; i++)
				{
					for (int32 j = x * CELL_CNT + 1; j < (x + 1) * CELL_CNT - 1; j++)
					{
						pictureImage[i][j] = pictureColor;
					}
				}
			}
		}

		this->isRoute = false;

		return true;
	}
	return false;
}

void PictureMaze::SolveMaze()
{
	if (SimpleGUI::Button(U"SolveMaze",
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 2
		}, 120))
	{
		Array<Point>route = MazeUtillity::Solve(mazeGrid, start, goal);
		SetRoute(route);
	}
}

bool PictureMaze::ReMaze()
{
	if (SimpleGUI::Button(U"ReMaze",
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 4
		}, 120))
	{
		return true;
	}

	return false;
}

void PictureMaze::UpdateDot(Point target, Color color)
{
	for (int32 i = target.y * CELL_CNT + 1; i < (target.y + 1) * CELL_CNT - 1; i++)
	{
		for (int32 j = target.x * CELL_CNT + 1; j < (target.x + 1) * CELL_CNT - 1; j++)
		{
			pictureImage[i][j] = color;
		}
	}
}

bool PictureMaze::DrawDot(const Input& mouse, Point& previousMousePoint)
{
	Point nowPoint = (Cursor::Pos() - FIELD_OFFSET) / (CELL_SIZE * CELL_CNT);
	if ((mouse.pressed() &&
		!(
			0 <= nowPoint.x && nowPoint.x < FIELD_WIDTH &&
			0 <= nowPoint.y && nowPoint.y < FIELD_HEIGHT
			)) || mouse.up()
		)
	{
		previousMousePoint = Point(-1, -1);
		return false;
	}
	bool returnFlag = false;
	if (mouse.down() &&
		(
			0 <= nowPoint.x && nowPoint.x < FIELD_WIDTH &&
			0 <= nowPoint.y && nowPoint.y < FIELD_HEIGHT
			)
	)
	{
		Color paintColor = (mouse == MouseL) ? PALETTE[1] : PALETTE[0];
		if (0 <= nowPoint.x && nowPoint.x < FIELD_WIDTH && 0 <= nowPoint.y && nowPoint.y < FIELD_HEIGHT)
		{
			this->UpdateDot(nowPoint, paintColor);
			pictureGrid[nowPoint] = mouse == MouseL;
			returnFlag = true;
		}
	}

	if (mouse.pressed() &&
		(
			0 <= nowPoint.x && nowPoint.x < FIELD_WIDTH &&
			0 <= nowPoint.y && nowPoint.y < FIELD_HEIGHT
			)
	)
	{
		if ((
			0 <= previousMousePoint.x && previousMousePoint.x < FIELD_WIDTH &&
			0 <= previousMousePoint.y && previousMousePoint.y < FIELD_HEIGHT
			)
		)
			if (nowPoint != previousMousePoint)
			{
				HashSet<Point> draw = MazeUtillity::Bresenham(
					previousMousePoint,
					nowPoint
				);
				Color paintColor = (mouse == MouseL) ? PALETTE[1] : PALETTE[0];
				for (Point cellPoint : draw)
				{
					if (0 <= cellPoint.x && cellPoint.x < FIELD_WIDTH && 0 <= cellPoint.y && cellPoint.y < FIELD_HEIGHT)
					{

						this->UpdateDot(cellPoint, paintColor);
						pictureGrid[cellPoint] = mouse == MouseL;
					}
				}

				returnFlag = true;
			}
	}

	if (mouse.pressed())
	{
		previousMousePoint = nowPoint;
	}

	return returnFlag;
}


bool PictureMaze::LoadFile()
{
	if (SimpleGUI::Button(U"Load",
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 6
		},
		120))
	{
		const MessageBoxResult result = System::MessageBoxOKCancel(U"読み込み", U"データを読み込みしますか");
		return result == MessageBoxResult::OK;
	}

	return false;
}

bool PictureMaze::SaveFile()
{
	if (SimpleGUI::Button(U"Save",
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 7
		},
		120))
	{
		const MessageBoxResult result = System::MessageBoxOKCancel(U"保存", U"データを保存しますか");
		return result == MessageBoxResult::OK;
	}

	return false;
}

bool PictureMaze::SaveAsOriginFile()
{
	if (SimpleGUI::Button(U"Save as",
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 8
		},
		120))
	{
		const MessageBoxResult result = System::MessageBoxOKCancel(U"保存", U"データを保存しますか");
		return result == MessageBoxResult::OK;
	}

	return false;
}

void PictureMaze::ResetCanvas()
{
	if (SimpleGUI::Button(U"ResetCanvas",
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 1
		},
		120))
	{
		for (const auto& p : step({ FIELD_WIDTH, FIELD_HEIGHT }))
		{
			pictureGrid[p] = false;
			for (int32 i = p.y * CELL_CNT + 1; i < (p.y + 1) * CELL_CNT - 1; i++)
			{
				for (int32 j = p.x * CELL_CNT + 1; j < (p.x + 1) * CELL_CNT - 1; j++)
				{
					pictureImage[i][j] = PALETTE[0];
				}
			}
		}
		TextureFill(AppMode::Paint);
		this->isExistMaze = false;
	}
}

bool PictureMaze::PrintSpanningTreeButton()
{
	if (SimpleGUI::Button(U"PrintSpanningTree",
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 1
		},
		180))
	{
		this->visualSpanningTreeFlag--;
		if (this->visualSpanningTreeFlag < 0)
		{
			this->visualSpanningTreeFlag = 7;
		}
	}

	return this->visualSpanningTreeFlag > 0;
}
void PictureMaze::SetSpanningTree(Array<Array<int32>> ansSpanningTree, Array<Array<int32>> spanningTree)
{
	this->ansSpanningTree = ansSpanningTree;
	this->spanningTree = spanningTree;
}
/**
	* @fn 全域木を盤面上に表示。
	* @brief 全域木を迷路上に表示する。
	* @param[in] (spanningTree) 全域木
	*/
void PictureMaze::PrintSpanningTree()
{
	for (int spanningTreeNum = 0; spanningTreeNum < 2; spanningTreeNum++)
	{
		if ((this->visualSpanningTreeFlag & 1 << spanningTreeNum) == 0)
		{
			continue;
		}

		Array<Array<int32>> spanningTree = (spanningTreeNum == 0) ? this->ansSpanningTree : this->spanningTree;
		Color color = (spanningTreeNum == 0) ? this->ansSpanningColor : this->outAnsSpanningColor;

		const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };
		for (int frm = 0; frm < spanningTree.size(); frm++)
		{
			for (auto to : spanningTree[frm])
			{
				int _frm = frm, _to = to;
				if (_frm > _to)
				{
					std::swap(_frm, _to);
				}

				Rect::FromPoints(
					(
						Point(
							_frm % FIELD_WIDTH,
							_frm / FIELD_WIDTH
						) * CELL_CNT + Point(1, 1) * (CELL_CNT / 2 - 1)
					) * CELL_SIZE + FIELD_OFFSET,
					(
						Point(
							_to % FIELD_WIDTH,
							_to / FIELD_WIDTH
						) * CELL_CNT + Point(1, 1) * (CELL_CNT / 2 + 1)
					) * CELL_SIZE + FIELD_OFFSET
				).draw(color);
			}
		}
	}
}

void PictureMaze::SetNgBorder(Grid< Array<bool>> ngBorder)
{
	this->ngBorder = ngBorder;
}

Grid< Array<bool>> PictureMaze::GetNgBorder()
{
	return this->ngBorder;
}

void PictureMaze::PrintNgBorder()
{
	if ((this->visualSpanningTreeFlag & (1 << 2)) == 0)
	{
		return;
	}
	for (int i = 0; i < this->ngBorder.size().y; i++)
	{
		for (int j = 0; j < this->ngBorder.size().x; j++)
		{
			if (this->ngBorder[i][j][0] && j < ngBorder.size().x - 1)
			{
				// right
				Rect::FromPoints(
					(
						Point(j, i) * CELL_CNT
						+ Point(1, 1) * CELL_CNT
						- Point(1, 0)
						- Point(0, 1) * (CELL_CNT)
						-Point(0, 1) * (!(i == 0 || i + 1 == this->ngBorder.size().y) ? 1 : 0)
						) * CELL_SIZE
					+ FIELD_OFFSET,

					(
						Point(j, i) * CELL_CNT
						+ Point(1, 1) * CELL_CNT
						+ Point(1, 0)
						+ Point(0, 1) * (!(i == 0 || i + 1 == this->ngBorder.size().y) ? 1 : 0)
						) * CELL_SIZE
					+ FIELD_OFFSET

				).draw(PALETTE[8]);
			}

			if (this->ngBorder[i][j][1] && i < ngBorder.size().y - 1)
			{
				// bottom
				Rect::FromPoints(
					(
						Point(j, i) * CELL_CNT
						+ Point(1, 1) * CELL_CNT
						- Point(0, 1)
						- Point(1, 0) * (CELL_CNT)
						-Point(1, 0) * (!(j == 0 || j + 1 == this->ngBorder.size().x) ? 1 : 0)
						) * CELL_SIZE
					+ FIELD_OFFSET,

					(
						Point(j, i) * CELL_CNT
						+ Point(1, 1) * CELL_CNT
						+ Point(0, 1)
						+ Point(1, 0) * (!(j == 0 || j + 1 == this->ngBorder.size().x) ? 1 : 0)
						) * CELL_SIZE
					+ FIELD_OFFSET

				).draw(PALETTE[8]);
			}
		}
	}
}

void PictureMaze::DrawMaze()
{
	this->isRoute = false;
	for (int32 i = 0; i < FIELD_HEIGHT * 2; i++)
	{
		int top = i * HARF_CELL_CNT, bottom = (i + 1) * HARF_CELL_CNT - 1;
		for (int32 j = 0; j < FIELD_WIDTH * 2; j++)
		{
			for (int ii = top; ii <= bottom; ii++)
			{
				int left = j * HARF_CELL_CNT, right = (j + 1) * HARF_CELL_CNT - 1;
				for (int jj = left; jj <= right; jj++)
				{
					mazeImage[ii][jj] = PALETTE[0];
				}
			}
		}
	}

	for (int32 i = 0; i < FIELD_HEIGHT * 2; i++)
	{
		int top = i * HARF_CELL_CNT, bottom = (i + 1) * HARF_CELL_CNT - 1;
		for (int32 j = 0; j < FIELD_WIDTH * 2; j++)
		{
			int left = j * HARF_CELL_CNT, right = (j + 1) * HARF_CELL_CNT - 1;

			int top_over = Max(top - 1, 0);
			int left_over = Max(left - 1, 0);
			int bottom_over = Min(bottom + 1, (FIELD_HEIGHT * 2) * HARF_CELL_CNT - 1);
			int right_over = Min(right + 1, (FIELD_WIDTH * 2) * HARF_CELL_CNT - 1);

			for (int32 digit = 0; digit < 4; digit++)
			{
				if (!((mazeGrid[i][j] >> digit & 1) == 1))
					continue;

				switch (digit)
				{
				case 0:
					// ↑ image[top][left:right] = WALL_COLOR;
					for (int jj = left_over; jj <= right_over; jj++)
					{
						mazeImage[top][jj] = WALL_COLOR;
					}
					break;

				case 1:
					// → image[top:bottom][right] = WALL_COLOR;
					for (int ii = top_over; ii <= bottom_over; ii++)
					{
						mazeImage[ii][right] = WALL_COLOR;
					}
					break;

				case 2:
					// ↓ image[bottom][left:right] = WALL_COLOR;
					for (int jj = left_over; jj <= right_over; jj++)
					{
						mazeImage[bottom][jj] = WALL_COLOR;
					}
					break;

				case 3:
					// ← image[top:bottom][left] = WALL_COLOR;
					for (int ii = top_over; ii <= bottom_over; ii++)
					{
						mazeImage[ii][left] = WALL_COLOR;
					}
					break;

				default:
					break;
				}
			}
		}
	}
}

void PictureMaze::DrawRouteDot(const Point& drawPoint)
{
	Color paintColor = PALETTE[4];

	for (int32 i = drawPoint.y * HARF_CELL_CNT + 1; i < (drawPoint.y + 1) * HARF_CELL_CNT - 1; i++)
	{
		for (int32 j = drawPoint.x * HARF_CELL_CNT + 1; j < (drawPoint.x + 1) * HARF_CELL_CNT - 1; j++)
		{

			mazeImage[i][j] = paintColor;
		}
	}
}

void PictureMaze::DrawRouteBetweenDot(const Point& drawPointFrom, const Point& drawPointTo)
{
	Color paintColor = PALETTE[4];
	int si = std::min(drawPointFrom.y, drawPointTo.y), gi = std::max(drawPointFrom.y, drawPointTo.y);
	int sj = std::min(drawPointFrom.x, drawPointTo.x), gj = std::max(drawPointFrom.x, drawPointTo.x);
	for (int32 i = si * HARF_CELL_CNT + 1; i < (gi + 1) * HARF_CELL_CNT - 1; i++)
	{
		for (int32 j = sj * HARF_CELL_CNT + 1; j < (gj + 1) * HARF_CELL_CNT - 1; j++)
		{
			mazeImage[i][j] = paintColor;
		}
	}
}

void PictureMaze::VisualizeFrameSpeedSlider()
{
	double changeFrame = 10 - this->visualizeFrame;
	if (SimpleGUI::Slider(U"{}"_fmt(changeFrame), changeFrame, 1, 10,
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 3
		},
		25, 100))
	{
		visualizeFrame = 10 - changeFrame;
	}
}

void PictureMaze::VisualizeRoute(int isFull)
{
	if (!this->isRoute)
	{
		this->index = 0;
		this->count = 0;
		return;
	};

	if (isFull  == 0) {
		// update
		this->count++;
		if (this->count > this->visualizeFrame)
		{
			this->count = 0;
			this->index++;

			if (this->index > this->ansRoute.size())
			{
				if (this->overCount < 10)
				{
					this->index = this->ansRoute.size();
					this->overCount++;
				}
				else {
					this->index = 0;
					this->overCount = 0;
					DrawMaze();
					this->isRoute = true;
				}
			}
		}
	}
	else
	{
		if (isFull == 1)
		{
			DrawMaze();
			this->index = 0;
		}
		if (isFull  == 2)
		{
			this->index = this->ansRoute.size();
		}
	}

	//draw
	for (int i = 0; i < this->index; i++)
	{
		if (i == 0)
			DrawRouteDot(ansRoute[i]);
		else
			DrawRouteBetweenDot(ansRoute[i - 1], ansRoute[i]);
	}
	TextureFill(AppMode::Maze);
}

/**
	* @fn 迷路モードにする。
	* @param[in] pictureGrid 連結グラフ判定用グリッド
	* @return bool 迷路モードにするかどうか
	*/
bool PictureMaze::ChangeMazeMode()
{
	if (SimpleGUI::Button(U"CreateMaze",
		Vec2{
			FIELD_OFFSET_LEFT + CELL_SIZE * FIELD_WIDTH * CELL_CNT + BUTTON_LEFT_PADDING,
			FIELD_OFFSET_UP + (BUTTON_HEIGHT + BUTTON_PADDING) * 0
		}, 120)
	)
	{
		if (!MazeUtillity::isConnectedGraph(pictureGrid))
		{
			System::MessageBoxOK(U"UnConnectedError", U"絵が全て繋がっていません。", MessageBoxStyle::Warning);
			return false;
		}

		return true;
	}

	return false;
}
