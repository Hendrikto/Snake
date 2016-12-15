#include <stdlib.h>

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

int main() {
	return EXIT_SUCCESS;
}
