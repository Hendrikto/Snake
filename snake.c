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

int main() {
	return EXIT_SUCCESS;
}
