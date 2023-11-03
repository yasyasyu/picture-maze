# include <Siv3D.hpp> // OpenSiv3D v0.6.10
constexpr Color PALETTE[] = {
	Palette::White, Palette::Black, Palette::Red
};
constexpr Color WALL_COLOR = Palette::Gray;
enum class AppMode
{
	Paint,
	Maze
};

namespace MazeUtillity {
	/**
 * @fn gridのTrueの部分が連結かを判定する。
 * @brief 連結グラフ判定
 * @param[in] (grid) 判定するグラフ
 * @return bool 連結グラフか否か
 */
	bool isConnectedGraph(const Grid<bool>& grid)
	{
		DisjointSet<int32> dsu(grid.size().y * grid.size().x);
		int32 D[] = { 0, 1, 0, -1, 0 };
		Array<int32> exist(0);
		bool f = true;
		for (int32 i = 0; i < grid.size().y; i++)
		{
			for (int32 j = 0; j < grid.size().x; j++)
			{
				if (grid[i][j])
				{
					f = false;
					exist << i * grid.size().x + j;
					for (int32 d = 0; d < 4; d++)
					{
						int32 di = D[d], dj = D[d + 1];
						int32 ni = i + di, nj = j + dj;

						if (!(
							0 <= ni && ni < grid.size().y && 0 <= nj && nj < grid.size().x
							))
						{
							continue;
						}
						if (grid[ni][nj])
						{
							dsu.merge(i * grid.size().x + j, ni * grid.size().x + nj);
						}
					}
				}
			}
		}

		if (f)
		{
			//Print << U"Not Painted";
			return false;
		}
		int32 first = exist[0];
		for (const auto& p : exist.slice(1))
		{
			if (!dsu.connected(first, p))
			{
				//Print << U"Not Connected";
				return false;
			}
		}

		//Print << U"Connected";
		return true;
	}
	HashSet<Point> Bresenham(Point previous, Point now)
	{
		HashSet<Point> draw;
		Point delta = Point(now.x - previous.x, now.y - previous.y);

		if (previous.x > now.x)
		{
			std::swap(now, previous);
			delta = Point(now.x - previous.x, now.y - previous.y);
		}

		draw.insert(previous);
		if (previous.x <= now.x && previous.y <= now.y)
		{
			int y = previous.y;
			int error = 0;
			for (int x = previous.x; x <= now.x; x += 1)
			{
				draw.insert(Point(x, y));
				error += delta.y * 2;
				if (error > delta.x)
				{
					y += 1;
					error -= delta.x * 2;
				}
			}
		}
		else
		{
			int y = previous.y;
			int error = 0;
			for (int x = previous.x; x <= now.x; x += 1)
			{
				draw.insert(Point(x, y));
				error -= delta.y * 2;
				if (error > delta.x)
				{
					y -= 1;
					error += delta.x * 2;
				}
			}
		}
		draw.insert(now);
		return draw;
	}

	void setNgBorder(Grid< Array<bool>>& ngBorder)
	{
		Point A = Point(
			Random<int32>(0, ngBorder.size().x - 1),
			Random<int32>(0, ngBorder.size().y - 1)
		);
		Point B;
		do {
			B = Point(
				Random<int32>(0, ngBorder.size().x - 1),
				Random<int32>(0, ngBorder.size().y - 1)
			);
		} while (A == B);

		if (A.x == B.x)
		{
			if (A.y > B.y)
			{
				std::swap(A, B);
			}
			for (int x = A.x, y = A.y; y <= B.y; y++)
			{
				ngBorder[y][x][0] = true;
			}
			//Print << U"!" << A << U", " << B;
		}
		else if (A.y == B.y)
		{
			if (A.x > B.x)
			{
				std::swap(A, B);
			}
			for (int x = A.x, y = A.y; x <= B.x; x++)
			{
				ngBorder[y][x][1] = true;
			}
			//Print << U"!" << A << U", " << B;
		}
		else
		{
			//Print << A << U", " << B;
			HashSet<Point> borders = Bresenham(A, B);
			HashSet<Point> used;
			std::queue<Point> que;
			que.push(A);
			borders.erase(A);
			used.insert(A);
			bool slant = A.x <= B.x && A.y <= B.y;
			do {
				while (que.size()) {
					Point frm = que.front();
					que.pop();
					for (int dy = -1; dy <= 1; dy++)
					{
						for (int dx = -1; dx <= 1; dx++)
						{
							if (dx == 0 and dx == dy) continue;
							Point to = Point(frm.x + dx, frm.y + dy);
							if (
								!(
									0 <= to.x && to.x < ngBorder.size().x &&
									0 <= to.y && to.y < ngBorder.size().y)
								) continue;
							if (used.contains(to)) continue;
							if (borders.contains(to))
							{
								que.push(to);
								ngBorder[to.y][to.x][0] = true;
								if (slant && 0 <= to.x - 1)
									ngBorder[to.y][to.x - 1][1] = true;
								if (!slant && to.x + 1 < ngBorder.size().x)
									ngBorder[to.y][to.x + 1][1] = true;
								borders.erase(to);
								used.insert(to);
							}
						}
					}
				}
				if (borders.size())
				{
					Point nxt = *borders.begin();
					que.push(nxt);
					borders.erase(nxt);
					used.insert(nxt);
					if (nxt == B)
					{
						ngBorder[nxt.y][nxt.x][0] = true;
						if (slant && 0 <= nxt.x - 1)
							ngBorder[nxt.y][nxt.x - 1][1] = true;
						if (!slant && nxt.x + 1 < ngBorder.size().x)
							ngBorder[nxt.y][nxt.x + 1][1] = true;
					}
				}
			} while (!used.contains(B));
		}

	}

	std::tuple< Array<Array<int>>, Array<Array<int>>, DisjointSet<int> > MakeSpanningTree(const Grid<bool>& grid)
	{
		int vertex = grid.size().y * grid.size().x;

		int D[] = { 0, 1, 0, -1, 0 };
		Grid<int> graph(grid.size().y, grid.size().x);
		for (int i = 0; i < grid.size().y; i++)
		{
			for (int j = 0; j < grid.size().x; j++)
			{
				graph[i][j] = i * grid.size().x + j;
			}
		}

		// 境界辺
		Grid< Array<bool>> ngBorder(grid.size().y, grid.size().x, Array<bool>(2));
		for (int i = 0; i < grid.size().y; i++)
		{
			ngBorder[i][grid.size().x - 1][0] = true;
		}
		for (int j = 0; j < grid.size().x; j++)
		{
			ngBorder[grid.size().y - 1][j][1] = true;
		}

		for (int cnt = 0; cnt < 20; cnt++) setNgBorder(ngBorder);

		Grid<int> edgeWeight(vertex, vertex, 0);
		std::priority_queue pq(
			[&](std::pair<int, int> x, std::pair<int, int> y)
			{ return edgeWeight[x.first][x.second] < edgeWeight[y.first][y.second]; },
			Array<std::pair<int, int>>()
		);

		for (int i = 0; i < grid.size().y; i++)
		{
			for (int j = 0; j < grid.size().x; j++)
			{
				for (int d = 0; d < 4; d++)
				{
					int di = D[d], dj = D[d + 1];
					int ni = i + di, nj = j + dj;

					if (!(0 <= ni && ni < grid.size().y && 0 <= nj && nj < grid.size().x))
					{
						continue;
					}
					if (grid[i][j] == grid[ni][nj])
					{
						std::pair<int, int> edge(graph[i][j], graph[ni][nj]);

						int frm = i * grid.size().x + j;
						int to = ni * grid.size().x + nj;
						if (frm > to)
						{
							std::swap(frm, to);
						}

						// 辺を張ることがNGな境界のとき
						if (!grid[i][j] && ngBorder[frm / grid.size().x][frm % grid.size().x][(to - frm) / grid.size().y])
						{
							continue;
						}

						edgeWeight[edge.first][edge.second] = (int)(Random() * 10000);
						pq.push(edge);
					}
				}
			}
		}

		DisjointSet<int> dsu(vertex);
		Array<Array<int>> ansSpanningTree(vertex);
		Array<Array<int>> spanningTree(vertex);

		while (!pq.empty())
		{
			std::pair<int, int> edge = pq.top();
			pq.pop();

			if (dsu.connected(edge.first, edge.second))
				continue;

			dsu.merge(edge.first, edge.second);
			if (grid[edge.first / grid.size().x][edge.first % grid.size().x])
			{
				ansSpanningTree[edge.first] << edge.second;
			}
			else
			{
				spanningTree[edge.first] << edge.second;
			}
		}

		return { ansSpanningTree, spanningTree, dsu };
	}


	/**
	 * @brief 座標uと座標vを繋げる。
	 * @param mazeGrid
	 * @param u
	 * @param v
	*/
	void MakeWall(Grid<int>& mazeGrid, int u, int v) {
		if (u > v) {
			std::swap(u, v);
		}
		if (abs(v - u) == 1)
		{
			int li = (u / (mazeGrid.size().x / 2)) * 2, lj = (u % (mazeGrid.size().x / 2)) * 2;
			int ri = (v / (mazeGrid.size().x / 2)) * 2, rj = (v % (mazeGrid.size().x / 2)) * 2;

			/*
				#*|##
				##|##
			*/
			mazeGrid[li + 0][lj + 1] &= 13;
			mazeGrid[li + 0][lj + 1] |= 4;

			/*
				##|##
				#*|##
			*/
			mazeGrid[li + 1][lj + 1] &= 13;
			mazeGrid[li + 1][lj + 1] |= 1;

			/*
				##|*#
				##|##
			*/
			mazeGrid[ri + 0][rj + 0] &= 7;
			mazeGrid[ri + 0][rj + 0] |= 4;

			/*
				##|##
				##|*#
			*/
			mazeGrid[ri + 1][rj + 0] &= 7;
			mazeGrid[ri + 1][rj + 0] |= 1;
		}
		else
		{
			int ti = (u / (mazeGrid.size().x / 2)) * 2, tj = (u % (mazeGrid.size().x / 2)) * 2;
			int bi = (v / (mazeGrid.size().x / 2)) * 2, bj = (v % (mazeGrid.size().x / 2)) * 2;

			/*
				##
				*#
				--
				##
				##
			*/
			mazeGrid[ti + 1][tj + 0] &= 11;
			mazeGrid[ti + 1][tj + 0] |= 2;

			/*
				##
				#*
				--
				##
				##
			*/
			mazeGrid[ti + 1][tj + 1] &= 11;
			mazeGrid[ti + 1][tj + 1] |= 8;

			/*
				##
				##
				--
				*#
				##
			*/
			mazeGrid[bi + 0][bj + 0] &= 14;
			mazeGrid[bi + 0][bj + 0] |= 2;

			/*
				##
				##
				--
				#*
				##
			*/
			mazeGrid[bi + 0][bj + 1] &= 14;
			mazeGrid[bi + 0][bj + 1] |= 8;
		}
	}


	/**
	 * @brief 該当座標の中の4分割したマスのいずれかを袋小路にする。
	 * @param mazeGrid
	 * @param dsu
	*/
	void MakePathWall(Grid<int>& mazeGrid, DisjointSet<int>& dsu)
	{

		int vertex = mazeGrid.size().y / 2 * mazeGrid.size().x / 2;
		Array<Array<int>> connectedElements(vertex);

		for (int v = 0; v < vertex; v++)
		{
			int u = dsu.find(v);
			connectedElements[u] << v;
		}
		for (int u = 0; u < vertex; u++)
		{
			if (connectedElements[u].size() > 0)
			{
				int rndCell = Random<int>(0, connectedElements[u].size() - 1);
				int cell = connectedElements[u][rndCell];
				{
					int rnd = Random<int>(0, 3);
					int i = (cell / (mazeGrid.size().x / 2)) * 2, j = (cell % (mazeGrid.size().x / 2)) * 2;
					int di = rnd / 2, dj = rnd % 2;

					int wall = mazeGrid[i + di][j + dj] ^ 15;
					int wall_cnt = 0;
					for (int bit = 0; bit < 4; bit++)
					{
						if (((wall >> bit) & 1) == 1)
						{
							wall_cnt++;
						}
					}
					int rnd_wall = Random<int>(0, wall_cnt - 1);

					wall_cnt = 0;
					for (int bit = 0; bit < 4; bit++)
					{
						if (((wall >> bit) & 1) == 1)
						{
							if (wall_cnt == rnd_wall)
							{
								mazeGrid[i + di][j + dj] |= 1 << bit;

								// i := bit % 2 == 0
								// j := bit % 2 == 1
								// bit + 1
								// + := 10, 11
								// - := 01, 00
								int ddi = ((((bit + 1) >> 1) & 1) == 1 ? 1 : -1) * (bit % 2 == 0 ? 1 : 0);
								int ddj = ((((bit + 1) >> 1) & 1) == 1 ? 1 : -1) * (bit % 2 == 1 ? 1 : 0);

								mazeGrid[i + di + ddi][j + dj + ddj] |= 1 << ((bit + 2) % 4);

								break;
							}
							wall_cnt++;
						}
					}
				}
			}
		}
	}

	void JointSpanningTree(Grid<int>& mazeGrid, DisjointSet<int>& dsu)
	{
		int D[] = { 0, 1, 0, -1, 0 };
		Grid<int> graph(mazeGrid.size().y, mazeGrid.size().x);
		for (int i = 0; i < mazeGrid.size().y; i++)
		{
			for (int j = 0; j < mazeGrid.size().x; j++)
			{
				graph[i][j] = i * mazeGrid.size().x + j;
			}
		}

		int vertex = mazeGrid.size().y * mazeGrid.size().x;
		Grid<int> edgeWeight(vertex, vertex, 0);
		std::priority_queue pq(
			[&](std::pair<int, int> x, std::pair<int, int> y)
			{ return edgeWeight[x.first][x.second] < edgeWeight[y.first][y.second]; },
			Array<std::pair<int, int>>()
		);

		for (int i = 0; i < mazeGrid.size().y; i++)
		{
			for (int j = 0; j < mazeGrid.size().x; j++)
			{
				for (int d = 0; d < 4; d++)
				{
					int di = D[d], dj = D[d + 1];
					int ni = i + di, nj = j + dj;

					int _frm = i / 2 * mazeGrid.size().x / 2 + j / 2;
					int _to = ni / 2 * mazeGrid.size().x / 2 + nj / 2;

					if (!(0 <= ni && ni < mazeGrid.size().y && 0 <= nj && nj < mazeGrid.size().x))
					{
						continue;
					}



					// 接続していない集団同士であるとき
					if (dsu.find(_frm) != dsu.find(_to))
					{
						std::pair<int, int> edge(graph[i][j], graph[ni][nj]);
						edgeWeight[edge.first][edge.second] = (int)(Random() * 10000);
						pq.push(edge);
						Print << graph[i][j] << U":" << graph[ni][nj];
					}
					else
					{
						std::pair<int, int> edge(graph[i][j], graph[ni][nj]);
						edgeWeight[edge.first][edge.second] = -1;
					}
				}
			}
		}
		TextWriter writer(U"Debug.txt");
		if (not writer)
		{
			throw Error{ U"Failed to open" };
		}
		for (int i = 0; i < edgeWeight.size().y; i++)
		{
			for (int j = 0; j < edgeWeight.size().x; j++)
			{
				writer.write(U"|{}|"_fmt(edgeWeight[i][j]));
			}
			writer.writeln();
		}
	}

	/**
	 * @brief 描画用グリッドから迷路を作成する。
	 * @param[in] pictureGrid 絵用のグリッド
	 * @param[out] mazeGrid 迷路用のグリッド
	 * @return Array<Array<int32>> 全域木
	 */
	Array<Array<int32>> CreateMaze(const Grid<bool>& pictureGrid, Grid<int>& mazeGrid)
	{
		for (int i = 0; i < pictureGrid.size().y; i++)
		{
			for (int j = 0; j < pictureGrid.size().x; j++)
			{
				mazeGrid[i * 2 + 0][j * 2 + 0] = 9;
				mazeGrid[i * 2 + 0][j * 2 + 1] = 3;
				mazeGrid[i * 2 + 1][j * 2 + 0] = 12;
				mazeGrid[i * 2 + 1][j * 2 + 1] = 6;
			}
		}

		auto [ansSpanningTree, spanningTree, dsu] = MakeSpanningTree(pictureGrid);

		for (int u = 0; u < ansSpanningTree.size(); u++)
		{
			for (auto v : ansSpanningTree[u])
			{
				MakeWall(mazeGrid, u, v);
			}
		}
		for (int u = 0; u < spanningTree.size(); u++)
		{
			for (auto v : spanningTree[u])
			{
				MakeWall(mazeGrid, u, v);
			}
		}

		MakePathWall(mazeGrid, dsu);

		// TODO joint cell that are not connected to the group.
		JointSpanningTree(mazeGrid, dsu);
		return ansSpanningTree;
	}
}

class PictureMaze
{
private:
	int CELL_SIZE = 3;
	int CELL_CNT = 8;
	int HARF_CELL_CNT = CELL_CNT / 2 + CELL_CNT % 2;

	int FIELD_HEIGHT = 20;
	int FIELD_WIDTH = 20;

	int FIELD_OFFSET_LEFT = CELL_SIZE / 2;
	int FIELD_OFFSET_UP = 50;

	Point FIELD_OFFSET = Point(FIELD_OFFSET_LEFT + CELL_SIZE % 2,
								FIELD_OFFSET_UP + CELL_SIZE % 2);

	int FIELD_OFFSET_RIGHT = 200;
	int FIELD_OFFSET_DOWN = CELL_SIZE / 2;

	int BUTTON_HEIGHT = FIELD_OFFSET_UP - 10;

	Image pictureImage = Image(CELL_CNT * FIELD_WIDTH, CELL_CNT * FIELD_HEIGHT, Palette::White);
	Image mazeImage = Image(CELL_CNT * FIELD_WIDTH, CELL_CNT * FIELD_HEIGHT, Palette::White);
	DynamicTexture texture = DynamicTexture(pictureImage);

public:
	Grid<bool> pictureGrid = Grid<bool>(FIELD_WIDTH, FIELD_HEIGHT, false);
	Grid<int> mazeGrid = Grid<int>(FIELD_WIDTH * 2, FIELD_HEIGHT * 2, 0);

	Point WindowSize() const {
		return Point(CELL_SIZE * FIELD_WIDTH * CELL_CNT + FIELD_OFFSET_LEFT + FIELD_OFFSET_RIGHT,
						CELL_SIZE * FIELD_HEIGHT * CELL_CNT + FIELD_OFFSET_UP + FIELD_OFFSET_DOWN);
	}

	void TextureScaled()
	{
		texture.scaled(CELL_SIZE).draw(FIELD_OFFSET_LEFT, FIELD_OFFSET_UP);
	}

	void TextureFill(AppMode application)
	{
		switch (application)
		{
		case AppMode::Paint:
			texture.fill(pictureImage);
			break;
		case AppMode::Maze:
			texture.fill(mazeImage);
			break;
		default:
			break;
		}
	}

	void DrawGrid()
	{
		for (const auto& p : step({ FIELD_WIDTH, FIELD_HEIGHT }))
		{
			RectF(p * CELL_CNT, CELL_CNT).overwriteFrame(pictureImage, 1, 1, WALL_COLOR);
		}
	}

	/**
	 * @fn 描画モードに戻る。
	 * @param[out] image 描画キャンバス
	 * @param[in] pictureGrid 描画用のグリッド
	 * @return bool 描画モードにできるか。
	 */
	bool ReturnPaint()
	{
		if (SimpleGUI::Button(U"ReturnPaint",
			Vec2{
				FIELD_OFFSET_LEFT + 120,
				std::max(5, FIELD_OFFSET_UP / 2 - BUTTON_HEIGHT / 2)
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

			return true;
		}
		return false;
	}

	void SolveMaze()
	{
		if (SimpleGUI::Button(U"SolveMaze",
			Vec2{
				FIELD_OFFSET_LEFT + 120 + 120 + 20 + 20,
				std::max(5, FIELD_OFFSET_UP / 2 - BUTTON_HEIGHT / 2)
			}, 120))
		{
			//Print << U"Solve Maze";
		}
	}
	bool ReMaze()
	{
		if (SimpleGUI::Button(U"ReMaze",
			Vec2{
				FIELD_OFFSET_LEFT + 120 + 120 + 120 + 20 + 20 + 20,
				std::max(5, FIELD_OFFSET_UP / 2 - BUTTON_HEIGHT / 2)
			}, 120))
		{
			//Print << U"ReCreate Maze";
			return true;
		}
		return false;
	}
	bool DrawDot(const Input& mouse, Point& previousMousePoint)
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
				for (int32 i = nowPoint.y * CELL_CNT + 1; i < (nowPoint.y + 1) * CELL_CNT - 1; i++)
				{
					for (int32 j = nowPoint.x * CELL_CNT + 1; j < (nowPoint.x + 1) * CELL_CNT - 1; j++)
					{
						pictureImage[i][j] = paintColor;
					}
				}
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
							for (int32 i = cellPoint.y * CELL_CNT + 1; i < (cellPoint.y + 1) * CELL_CNT - 1; i++)
							{
								for (int32 j = cellPoint.x * CELL_CNT + 1; j < (cellPoint.x + 1) * CELL_CNT - 1; j++)
								{
									pictureImage[i][j] = paintColor;
								}
							}
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

	void SaveImage()
	{
		if (SimpleGUI::Button(U"SaveImage",
			Vec2{ FIELD_OFFSET_LEFT,
				std::max(5, FIELD_OFFSET_UP / 2 - BUTTON_HEIGHT / 2)
			},
			120))
		{
			pictureImage.saveWithDialog();

		}
	}

	void ResetCanvas()
	{
		if (SimpleGUI::Button(U"ResetCanvas",
			Vec2{ FIELD_OFFSET_LEFT + 120,
				std::max(5, FIELD_OFFSET_UP / 2 - BUTTON_HEIGHT / 2)
			}, 160))
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
		}
	}

	/**
	 * @fn 全域木を盤面上に表示。
	 * @brief 全域木を迷路上に表示する。
	 * @param[in] (spanningTree) 全域木
	 */
	void PrintSpanningTree(Array<Array<int32>> spanningTree)
	{
		const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };
		for (int frm = 0; frm < spanningTree.size(); frm++)
		{
			for (auto to : spanningTree[frm])
			{
				int _frm = frm, _to = to;
				if (_frm > _to)
					std::swap(_frm, _to);

				Line(
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
						) * CELL_CNT + Point(1, 1) * (CELL_CNT / 2 - 1)
						+ Point(
							std::abs(_frm % FIELD_WIDTH - _to % FIELD_WIDTH),
							std::abs(_frm / FIELD_WIDTH - _to / FIELD_WIDTH))
						) * CELL_SIZE + FIELD_OFFSET
				).draw(3, Palette::Red);

				Line(
					(
						Point(
							_frm % FIELD_WIDTH,
							_frm / FIELD_WIDTH
						) * CELL_CNT + Point(1, 1) * CELL_CNT / 2
						- Point(
							std::abs(_frm % FIELD_WIDTH - _to % FIELD_WIDTH),
							std::abs(_frm / FIELD_WIDTH - _to / FIELD_WIDTH))
						) * CELL_SIZE + FIELD_OFFSET,

					(
						Point(
							_to % FIELD_WIDTH,
							_to / FIELD_WIDTH
						) * CELL_CNT + Point(1, 1) * (CELL_CNT / 2)
						) * CELL_SIZE + FIELD_OFFSET
				).draw(3, Palette::Red);
			}
		}
	}

	void DrawMaze()
	{
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

	/**
	 * @fn 迷路モードにする。
	 * @param[in] pictureGrid 連結グラフ判定用グリッド
	 * @return bool 迷路モードにするかどうか
	 */
	bool ChangeMazeMode()
	{
		return SimpleGUI::Button(U"CreateMaze",
			Vec2{
				FIELD_OFFSET_LEFT + 120 + 120 + 20 + 20,
				std::max(5, FIELD_OFFSET_UP / 2 - BUTTON_HEIGHT / 2)
			}, 120)
			&& MazeUtillity::isConnectedGraph(pictureGrid);
	}


};

class Application
{
private:
	AppMode applicationMode = AppMode::Paint;
	bool _init = true;

public:
	AppMode mode() const
	{
		return applicationMode;
	}
	void ModeChange()
	{
		_init = true;
		switch (applicationMode)
		{
		case AppMode::Paint:
			applicationMode = AppMode::Maze;
			break;
		case AppMode::Maze:
			applicationMode = AppMode::Paint;
			break;
		default:
			break;
		}
	}

	bool init() const {
		return _init;
	}
	void InitBreak()
	{
		_init = false;
	}
};


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
				spanningTree = MazeUtillity::CreateMaze(pictureMaze.pictureGrid, pictureMaze.mazeGrid);
				pictureMaze.DrawMaze();
				app.InitBreak();
				pictureMaze.TextureFill(AppMode::Maze);
			}
			pictureMaze.PrintSpanningTree(spanningTree);

			//SolveMaze(mazeGrid);
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
