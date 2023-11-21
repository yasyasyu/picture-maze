# include "MazeUtility.hpp"

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

			return false;
		}
		int32 first = exist[0];
		for (const auto& p : exist.slice(1))
		{
			if (!dsu.connected(first, p))
			{

				return false;
			}
		}

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
		Point A;
		Point B;
		do {
			A = Point(
				Random<int32>(0, ngBorder.size().x - 1),
				Random<int32>(0, ngBorder.size().y - 1)
			);
			do {
				B = Point(
					Random<int32>(0, ngBorder.size().x - 1),
					Random<int32>(0, ngBorder.size().y - 1)
				);
			} while (A == B);
		} while ((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y) < 150);

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
		}
		else
		{
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

	std::tuple< Array<Array<int>>, Array<Array<int>>, DisjointSet<int>, int>
		MakeSpanningTree(const Grid<bool>& grid)
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
		int ans = 0;
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
				ans = edge.first;
			}
			else
			{
				spanningTree[edge.first] << edge.second;
			}
		}

		return { ansSpanningTree, spanningTree, dsu, dsu.find(ans) };
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

			for (int di = 0; di < 2; di++)
			{
				mazeGrid[li + di][lj + 1] &= 15 - (1 << 1);
				mazeGrid[li + di][lj + 1] |= 1 << (2 - 2 * di);

				mazeGrid[ri + di][rj + 0] &= 15 - (1 << 3);
				mazeGrid[ri + di][rj + 0] |= 1 << (2 - 2 * di);
			}
		}
		else
		{
			int ti = (u / (mazeGrid.size().x / 2)) * 2, tj = (u % (mazeGrid.size().x / 2)) * 2;
			int bi = (v / (mazeGrid.size().x / 2)) * 2, bj = (v % (mazeGrid.size().x / 2)) * 2;

			for (int dj = 0; dj < 2; dj++)
			{
				mazeGrid[ti + 1][tj + dj] &= 15 - (1 << 2);
				mazeGrid[ti + 1][tj + dj] |= 1 << (1 + 2 * dj);

				mazeGrid[bi + 0][bj + dj] &= 15 - (1 << 0);
				mazeGrid[bi + 0][bj + dj] |= 1 << (1 + 2 * dj);
			}
		}
	}

	void JointCell(Grid<int>& mazeGrid, int u, int v)
	{
		if (u > v)
		{
			std::swap(u, v);
		}

		if (abs(v - u) == 1)
		{
			int li = u / mazeGrid.size().x, lj = u % mazeGrid.size().x;
			int ri = v / mazeGrid.size().x, rj = v % mazeGrid.size().x;

			mazeGrid[li][lj] &= 15 - (1 << 1);
			mazeGrid[ri][rj] &= 15 - (1 << 3);
		}
		else
		{
			int ti = u / mazeGrid.size().x, tj = u % mazeGrid.size().x;
			int bi = v / mazeGrid.size().x, bj = v % mazeGrid.size().x;

			mazeGrid[ti][tj] &= 15 - (1 << 2);
			mazeGrid[bi][bj] &= 15 - (1 << 0);
		}
	}

	/**
	 * @brief 該当座標の中の4分割したマスのいずれかを袋小路にする。
	 * @param mazeGrid
	 * @param dsu
	*/
	std::tuple<Point, Point>
		MakePathWall(Grid<int>& mazeGrid, DisjointSet<int>& dsu, int ans)
	{

		int vertex = mazeGrid.size().y / 2 * mazeGrid.size().x / 2;
		Array<Array<int>> connectedElements(vertex);

		Point start;
		Point goal;
		int ansLeader = dsu.find(ans);
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

								if (u == ansLeader)
								{
									start = Point(j + dj, i + di);
									goal = Point(j + dj + ddj, i + di + ddi);
								}

								break;
							}
							wall_cnt++;
						}
					}
				}
			}
		}

		return { start, goal };
	}

	void JointSpanningTree(
		Grid<int>& mazeGrid, DisjointSet<int>& dsu, const Grid<bool>& pictureGrid)
	{
		int D[] = { 0, 1, 0, -1, 0 };

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
						int frm = i * mazeGrid.size().x + j;
						int to = ni * mazeGrid.size().x + nj;

						edgeWeight[frm][to] = (int)(Random() * 10000);
						pq.push(std::pair<int, int>(frm, to));
					}
				}
			}
		}
		while (!pq.empty())
		{
			std::pair<int, int> edge = pq.top();
			pq.pop();
			int fi = edge.first / mazeGrid.size().x, fj = edge.first % mazeGrid.size().x;
			int ni = edge.second / mazeGrid.size().x, nj = edge.second % mazeGrid.size().x;

			int frm = fi / 2 * mazeGrid.size().x / 2 + fj / 2;
			int to = ni / 2 * mazeGrid.size().x / 2 + nj / 2;

			if (dsu.connected(frm, to))
			{
				continue;
			}

			dsu.merge(frm, to);

			JointCell(mazeGrid, edge.first, edge.second);
		}
	}

	/**
	 * @brief 描画用グリッドから迷路を作成する。
	 * @param[in] pictureGrid 絵用のグリッド
	 * @param[out] mazeGrid 迷路用のグリッド
	 * @return Array<Array<int32>> 全域木
	 */
	std::tuple<Array<Array<int32>>, Array<Array<int32>>, Point, Point>
		CreateMaze(const Grid<bool>& pictureGrid, Grid<int>& mazeGrid, uint64 seed)
	{
		Reseed(seed);

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

		auto [ansSpanningTree, spanningTree, dsu, ans] = MakeSpanningTree(pictureGrid);

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

		auto [start, goal] = MakePathWall(mazeGrid, dsu, ans);

		JointSpanningTree(mazeGrid, dsu, pictureGrid);

		return
		{
			ansSpanningTree, spanningTree, start, goal
		};
	}

	Array<Point> Solve(Grid<int>& mazeGrid, Point start, Point goal)
	{
		Array<Point> stack;
		stack << start;
		int32 INF = mazeGrid.size().x * mazeGrid.size().y + 100;
		Grid<int> dist(mazeGrid.size(), INF);
		Array <std::pair<int, int>> D = {
			{ 0,  -1},
			{ 1,  0},
			{ 0,  1},
			{-1,  0}
		};

		dist[start] = 0;
		while (stack) {
			Point frm = stack.back();
			stack.pop_back();
			int posWall = mazeGrid[frm];

			for (int d = 0; d < 4; d++)
			{

				if (((posWall >> d) & 1) == 1)	continue;

				int dx = D[d].first, dy = D[d].second;
				if (
					!(
						0 <= frm.x + dx && frm.x + dx < mazeGrid.size().x &&
						0 <= frm.y + dy && frm.y + dy < mazeGrid.size().y
						))
				{
					continue;
				}

				Point to = Point(frm.x + dx, frm.y + dy);

				if (dist[to] < dist[frm] + 1)	continue;

				dist[to] = std::min(dist[to], dist[frm] + 1);
				if (to == goal)
				{
					stack.clear();
					break;
				}
				stack << to;
			}
		}

		stack << goal;
		Array<Point> route{ goal };
		while (stack) {
			Point frm = stack.back();
			stack.pop_back();
			int posWall = mazeGrid[frm];
			for (int d = 0; d < 4; d++)
			{
				if (((posWall >> d) & 1) == 1)	continue;

				int dx = D[d].first, dy = D[d].second;
				if (
					!(
						0 <= frm.x + dx && frm.x + dx < mazeGrid.size().x &&
						0 <= frm.y + dy && frm.y + dy < mazeGrid.size().y
						))
				{
					continue;
				}

				Point to = Point(frm.x + dx, frm.y + dy);

				if (dist[to] != dist[frm] - 1)	continue;

				route << to;
				if (to == start)
				{
					stack.clear();
					break;
				}
				stack << to;
			}
		}
		route.reverse();
		return route;
	}
}
