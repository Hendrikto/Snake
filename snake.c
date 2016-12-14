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

int main() {
	return EXIT_SUCCESS;
}
