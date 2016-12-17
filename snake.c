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
 * Check whether there is a tail at some position.
 *
 * @return Whether there is a tail at the specified position.
 */
bool tailAt(
	const Position *tail
	,const size_t length
	,const Position position
) {
	for (size_t i = 0; i < length; i++) {
		if (tail[i].row == position.row && tail[i].col == position.col) {
			return true;
		}
	}
	return false;
}

/**
 * @param board The board.
 * @param postition The position.
 *
 * @return Whether the snake is dead at the given position on the given board.
 */
bool snakeDead(const Board board, const Position position) {
	return position.col >= BOARD_WIDTH
		|| position.col < 0
		|| position.row >= BOARD_HEIGHT
		|| position.row < 0
		|| board[position.row][position.col] == SNAKE;
}

/**
 * Shift the tail by moving all elements down and putting the head at the top.
 */
void shiftTail(Game *game) {
	size_t length;
	if ((length = game->tailLength)) {
		Position *tail = game->tail;
		for (size_t i = 0; i < length - 1; i++) {
			tail[i] = tail[i + 1];
		}
		tail[length - 1].col = game->head.col;
		tail[length - 1].row = game->head.row;
	}
}

/**
 * Extend the tail by appending the head. This function will reallocate the tail
 * if it is not big enough to hold another element.
 */
void extendTail(Game *game) {
	game->tailLength++;
	if (game->tailLength > game->tailAllocated) {
		game->tailAllocated += BOARD_WIDTH;
		game->tail = realloc(
			game->tail, game->tailAllocated * sizeof(Position)
		);
	}
	game->tail[game->tailLength - 1] = game->head;
}

/**
 * Draw the board with the current game state. This clears the board before
 * writing to it.
 */
void drawBoard(Game *game) {
	Position *tail = game->tail;
	size_t length = game->tailLength;
	clearBoard(game->board);
	game->board[game->head.row][game->head.col] = SNAKE;
	for (size_t i = 0; i < length; i++) {
		game->board[tail[i].row][tail[i].col] = SNAKE;
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

/**
 * Advance the game state one tick.
 *
 * @param game The game.
 *
 * @return Whether the snake is still alive.
 */
bool tick(Game *game) {
	Position next = {
		game->head.row + rowDelta(game->movement)
		,game->head.col + colDelta(game->movement)
	};
	if (snakeDead(game->board, next)) {
		return false;
	}
	if (next.row == game->food.row && next.col == game->food.col) {
		game->food.col = random() % BOARD_WIDTH;
		game->food.row = random() % BOARD_HEIGHT;
		extendTail(game);
	} else {
		shiftTail(game);
	}
	game->head = next;
	drawBoard(game);
	return true;
}

int main() {
	return EXIT_SUCCESS;
}
