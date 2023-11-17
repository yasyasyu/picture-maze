#pragma once

constexpr Color PALETTE[] = {
	Palette::White,
	Palette::Black,
	Palette::Red,
	Palette::Green,
	Palette::Blue,
	Palette::Orange,
	Palette::Hotpink,
	Palette::Gray
};
constexpr Color WALL_COLOR = PALETTE[7];

enum class AppMode
{
	Paint,
	Maze
};
