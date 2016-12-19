#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <GL/freeglut.h>

/**
 * @author: Hendrik Werner
 */

#define BOARD_HEIGHT 50
#define BOARD_WIDTH 50
#define STEP_DELAY 500

typedef struct Position {
	int row;
	int col;
} Position;

typedef enum Direction {LEFT, UP, RIGHT, DOWN} Direction;

typedef struct Game {
	Position head;
	Position *tail;
	size_t tailLength;
	size_t tailAllocated;
	Position food;
	Direction movement;
} Game;

Game game;

/**
 * @param d The direction.
 *
 * @return The row delta of a given direction.
 */
int rowDelta(const Direction d) {
	switch (d) {
		case UP: return 1;
		case DOWN: return -1;
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
 * @return Whether the snake is dead at the given position on the given board.
 */
bool snakeDead(const Position *tail, size_t length, const Position position) {
	return position.col >= BOARD_WIDTH
		|| position.col < 0
		|| position.row >= BOARD_HEIGHT
		|| position.row < 0
		|| tailAt(tail, length, position);
}

/**
 * Shift the tail by moving all elements down and putting the head at the top.
 */
void shiftTail(Position *tail, const size_t length, const Position head) {
	if (length) {
		for (size_t i = 0; i < length - 1; i++) {
			tail[i] = tail[i + 1];
		}
		tail[length - 1].col = head.col;
		tail[length - 1].row = head.row;
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
 * Initialize a game.
 */
void initGame(Game *game) {
	game->head.col = 0;
	game->head.row = 0;
	game->tail = calloc(BOARD_WIDTH, sizeof(Position));
	game->tailLength = 0;
	game->tailAllocated = BOARD_WIDTH;
	game->food.col = random() % BOARD_WIDTH;
	game->food.row = random() % BOARD_HEIGHT;
	game->movement = RIGHT;
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
	if (snakeDead(game->tail, game->tailLength, next)) {
		return false;
	}
	if (next.row == game->food.row && next.col == game->food.col) {
		game->food.col = random() % BOARD_WIDTH;
		game->food.row = random() % BOARD_HEIGHT;
		extendTail(game);
	} else {
		shiftTail(game->tail, game->tailLength, game->head);
	}
	game->head = next;
	return true;
}

void drawCell(Position position, float r, float g, float b) {
	glColor3f(r, g, b);
	glRectf(
		position.col / (float) BOARD_WIDTH * 2 - 1
		,position.row / (float) BOARD_HEIGHT * 2 - 1
		,(position.col + 1) / (float) BOARD_WIDTH * 2 - 1
		,(position.row + 1) / (float) BOARD_HEIGHT * 2 - 1
	);
}

void display() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	drawCell(game.head, .5, .5, .5);
	for (size_t i = 0; i < game.tailLength; i++) {
		drawCell(game.tail[i], 1, 1, 1);
	}
	drawCell(game.food, 1, 0, 0);
	glFlush();
}

void handleKeyboard(unsigned char key, int x, int y) {
	(void) x; (void) y;
	switch (key) {
		case 'a': game.movement = LEFT; break;
		case 'w': game.movement = UP; break;
		case 'd': game.movement = RIGHT; break;
		case 's': game.movement = DOWN; break;
	}
}

void step(int value) {
	(void) value;
	if (!tick(&game)) {
		glutLeaveMainLoop();
	}
	glutPostRedisplay();
	glutTimerFunc(STEP_DELAY, step, 0);
}

int main(int argc, char **argv) {
	srand(time(NULL));
	initGame(&game);
	glutInit(&argc, argv);
	glutCreateWindow("Snake");
	glutInitWindowSize(500, 500);
	glutDisplayFunc(display);
	glutKeyboardFunc(handleKeyboard);
	glutTimerFunc(STEP_DELAY, step, 0);
	glutMainLoop();
	free(game.tail);
	return EXIT_SUCCESS;
}
