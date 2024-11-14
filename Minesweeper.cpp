#include <iostream>
#include "raylib.h"
#include <vector>
#include <random>
#include <set>

struct Tile
{
	static enum Contents
	{
		BOMB = -1,
		EMPTY = 0,
		ONE = 1,
		TWO = 2,
		THREE = 3,
		FOUR = 4,
		FIVE = 5,
		SIX = 6,
		SEVEN = 7,
		EIGHT = 8
	};

	bool visible = false;
	Contents tileContent = EMPTY;
	Vector2 dimensions = { 10,10 };
	Color colour = RED;
};

typedef std::vector<std::vector<Tile>> TileGrid;

int generateRandomNumber(int min, int max) {
	// Create a random device and a Mersenne Twister random number generator
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define a distribution in the specified range [min, max]
	std::uniform_int_distribution<int> dis(min, max);

	// Generate and return the random number
	return dis(gen);
}

int getNumberOfBombsAroundPoint(TileGrid board, Vector2 point)
{
	int count = 0;

	if (point.y - 1 > 0 && point.x - 1 > 0 && board[point.y - 1][point.x - 1].tileContent == Tile::Contents::BOMB) count++; // Top Left

	if (point.y - 1 > 0 && board[point.y - 1][point.x].tileContent == Tile::Contents::BOMB) count++; // Top Middle

	if (point.y - 1 > 0 && point.x + 1 < board[point.y].size() && board[point.y - 1][point.x + 1].tileContent == Tile::Contents::BOMB) count++; // Top Right


	if (point.x - 1 > 0 && board[point.y][point.x - 1].tileContent == Tile::Contents::BOMB) count++; // Middle Left

	if (point.x + 1 < board[point.y].size() && board[point.y][point.x + 1].tileContent == Tile::Contents::BOMB) count++; // Middle Right


	if (point.y + 1 < board.size() && point.x - 1 > 0 && board[point.y + 1][point.x - 1].tileContent == Tile::Contents::BOMB) count++; // Bottem Left

	if (point.y + 1 < board.size() && board[point.y + 1][point.x].tileContent == Tile::Contents::BOMB) count++; // Bottem Middle

	if (point.y + 1 < board.size() && point.x + 1 < board[point.y].size() && board[point.y + 1][point.x + 1].tileContent == Tile::Contents::BOMB) count++; // Bottem Right


	return count;
}

TileGrid generateBoard(Vector2 dimensions)
{
	TileGrid board;

	// Populate board with tiles.
	for (int rows = 0; rows < dimensions.y; rows++)
	{
		std::vector<Tile> row;

		for (int columns = 0; columns < dimensions.x; columns++)
		{
			row.push_back(Tile{});
		}

		board.push_back(row);
	}

	// Place bombs on board.

	std::set<Vector2> bombCoordinates;

	while (bombCoordinates.size() != 10)
	{
		bombCoordinates.insert(Vector2{ (float)generateRandomNumber(0,9), (float)generateRandomNumber(0,9) });
	}

	for (Vector2 coordinates : bombCoordinates)
	{
		board[coordinates.y][coordinates.x].tileContent = Tile::Contents::BOMB;
	}


	// Place numbers
	for (int y = 0; y < board.size(); y++)
	{
		for (int x = 0; x < board[0].size(); x++)
		{
			int numberOfBombs = getNumberOfBombsAroundPoint(board, Vector2{ (float)x,(float)y });

			if (numberOfBombs == 1) board[y][x].tileContent = Tile::Contents::ONE;
			else if (numberOfBombs == 2) board[y][x].tileContent = Tile::Contents::TWO;
			else if (numberOfBombs == 3) board[y][x].tileContent = Tile::Contents::THREE;
			else if (numberOfBombs == 4) board[y][x].tileContent = Tile::Contents::FOUR;
			else if (numberOfBombs == 5) board[y][x].tileContent = Tile::Contents::FIVE;
			else if (numberOfBombs == 6) board[y][x].tileContent = Tile::Contents::SIX;
			else if (numberOfBombs == 7) board[y][x].tileContent = Tile::Contents::SEVEN;
			else if (numberOfBombs == 8) board[y][x].tileContent = Tile::Contents::EIGHT;
		}
	}


	return board;
}

int main()
{
	InitWindow(800, 600, "Minesweeper");
	SetTargetFPS(60);

	TileGrid board = generateBoard(Vector2{ 9,9 });

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(PURPLE);



		EndDrawing();
	}

	CloseWindow();
	return 0;
}
