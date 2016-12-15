#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * @author: Hendrik Werner
 */

#define BOARD_HEIGHT 50
#define BOARD_WIDTH 50

typedef struct Position {
	int row;
	int col;
} Position;

typedef enum Cell {
	EMPTY
	,SNAKE
	,FOOD
} Cell;

typedef Cell Board[BOARD_HEIGHT][BOARD_WIDTH];

typedef enum Direction {LEFT, UP, RIGHT, DOWN} Direction;

typedef struct Game {
	Board board;
	Position head;
	Position *tail;
	size_t tailLength;
	size_t tailAllocated;
	Position food;
	Direction movement;
} Game;

/**
 * Clear a board by setting all cells to EMPTY.
 */
void clearBoard(Board board) {
	memset(board, EMPTY, sizeof(Board));
}

/**
 * @param d The direction.
 *
 * @return The row delta of a given direction.
 */
int rowDelta(const Direction d) {
	switch (d) {
		case UP: return -1;
		case DOWN: return 1;
		default: return 0;
	}
}

/**
 * @param d The direction.
 *
 * @return The column delta of a given direction.
 */
int colDelta(const Direction d) {
	switch (d) {
		case LEFT: return -1;
		case RIGHT: return 1;
		default: return 0;
	}
}

/**
 * Represent a cell using a character.
 *
 * @return A character representing the content of the cell provided.
 */
char representCell(const Cell c) {
	switch (c) {
		case SNAKE: return 'S';
		case FOOD: return 'F';
		default: return '.';
	}
}

/**
 * Create a string representation of some board.
 *
 * @param board The board that is to be represented.
 *
 * @return A string representation of the given board. The caller must take care
 * to free the string returned.
 */
char *stringify(const Board board) {
	char *str = malloc(BOARD_WIDTH * BOARD_HEIGHT + BOARD_HEIGHT + 1);
	size_t pos = 0;
	for (size_t row = 0; row < BOARD_HEIGHT; row++) {
		for (size_t col = 0; col < BOARD_WIDTH; col++) {
			str[pos++] = representCell(board[row][col]);
		}
		str[pos++] = '\n';
	}
	str[pos] = '\0';
	return str;
}

/**
 * @return Whether the snake is dead.
 */
bool snakeDead(Game *game) {
	int col = game->head.col;
	int row = game->head.row;
	return col >= BOARD_WIDTH
		|| col < 0
		|| row >= BOARD_HEIGHT
		|| row < 0
		|| game->board[row][col] == SNAKE;
}

/**
 * Draw the board with the current game state. This clears the board before
 * writing to it.
 */
void drawBoard(Game *game) {
	clearBoard(game->board);
	game->board[game->head.row][game->head.col] = SNAKE;
	for (size_t i = 0; i < game->tailLength; i++) {
		game->board[game->tail[i].row][game->tail[i].col] = SNAKE;
	}
	game->board[game->food.row][game->food.col] = FOOD;
}

/**
 * Initialize a game.
 */
void initGame(Game *game) {
	game->head.col = 0;
	game->head.row = 0;
	game->tail = calloc(BOARD_WIDTH, sizeof(Position));
	game->tailLength = 0;
	game->tailAllocated = BOARD_WIDTH;
	game->food.col = BOARD_WIDTH / 2;
	game->food.row = BOARD_HEIGHT / 2;
	game->movement = RIGHT;
	drawBoard(game);
}

int main() {
	return EXIT_SUCCESS;
}
