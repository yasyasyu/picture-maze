#pragma once

namespace MazeUtillity {
	/**
	 * @fn gridのTrueの部分が連結かを判定する。
	 * @brief 連結グラフ判定
	 * @param[in] (grid) 判定するグラフ
	 * @return bool 連結グラフか否か
	 */
	bool isConnectedGraph(const Grid<bool>& grid);

	HashSet<Point> Bresenham(Point previous, Point now);

	void setNgBorder(Grid< Array<bool>>& ngBorder);

	std::tuple< Array<Array<int>>, Array<Array<int>>, DisjointSet<int>, int>
		MakeSpanningTree(const Grid<bool>& grid);

	/**
	 * @brief 座標uと座標vを繋げる。
	 * @param mazeGrid
	 * @param u
	 * @param v
	*/
	void MakeWall(Grid<int>& mazeGrid, int u, int v);

	void JointCell(Grid<int>& mazeGrid, int u, int v);

	/**
	 * @brief 該当座標の中の4分割したマスのいずれかを袋小路にする。
	 * @param mazeGrid
	 * @param dsu
	*/
	std::tuple<Point, Point>
		MakePathWall(Grid<int>& mazeGrid, DisjointSet<int>& dsu, int ans);

	void JointSpanningTree(
		Grid<int>& mazeGrid, DisjointSet<int>& dsu, const Grid<bool>& pictureGrid);

	/**
	 * @brief 描画用グリッドから迷路を作成する。
	 * @param[in] pictureGrid 絵用のグリッド
	 * @param[out] mazeGrid 迷路用のグリッド
	 * @return Array<Array<int32>> 全域木
	 */
	std::tuple<Array<Array<int32>>, Array<Array<int32>>, Point, Point>
		CreateMaze(const Grid<bool>& pictureGrid, Grid<int>& mazeGrid, uint64 seed);

	Array<Point> Solve(Grid<int>& mazeGrid, Point start, Point goal);
}
