#pragma once

constexpr Color PALETTE[] = {
	Palette::White,
	Palette::Black,
	Palette::Red,
	Palette::Green,
	Palette::Orange,
	Palette::Blue,
	Palette::Orchid,
	Palette::Gray,
	Palette::Mediumvioletred,
};
constexpr Color WALL_COLOR = PALETTE[7];

enum class AppMode
{
	Paint,
	Maze
};


