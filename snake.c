#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <GL/freeglut.h>

#include <GL/glut.h>

/**
 * @author: Hendrik Werner
 */

#define BOARD_HEIGHT 50
#define BOARD_WIDTH 50
#define STEP_DELAY 150
#define NR_SNAKES 1

typedef struct Position {
	int row;
	int col;
} Position;

typedef enum Direction {LEFT, UP, RIGHT, DOWN} Direction;

typedef struct Snake {
	Position head;
	Position *tail;
	size_t tailLength;
	size_t tailAllocated;
	Direction movement;
} Snake;

typedef struct Game {
	Snake snakes[NR_SNAKES];
	Position food;
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
 * @return Whether both positions are equal.
 */
bool positionsEqual(Position p1, Position p2) {
	return p1.col == p2.col && p1.row == p2.row;
}

/**
 * @return Whether there is a snake at the specified position.
 */
bool snakeAt(
	const Snake snakes[]
	,const Position position
) {
	for (size_t k = 0; k < NR_SNAKES; k++) {
		if (positionsEqual(position, snakes[k].head)) {
			return true;
		}
		for (size_t i = 0; i < snakes[k].tailLength; i++) {
			if (positionsEqual(position, snakes[k].tail[i])) {
				return true;
			}
		}
	}
	return false;
}

/**
 * @return Whether the snake is dead at the given position on the given board.
 */
bool snakeDead(
	const Snake *snakes
	,const Position position
) {
	return position.col >= BOARD_WIDTH
		|| position.col < 0
		|| position.row >= BOARD_HEIGHT
		|| position.row < 0
		|| snakeAt(snakes, position);
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
void extendTail(
	Position *tail
	,size_t *length
	,size_t *allocated
	,const Position head
) {
	(*length)++;
	if ((*length) > (*allocated)) {
		(*allocated) += BOARD_WIDTH;
		tail = realloc(tail, (*allocated) * sizeof(Position));
	}
	tail[(*length) - 1] = head;
}

/**
 * Initialize a snake.
 */
void initSnake(Snake *snake) {
	snake->head.col = 0;
	snake->head.row = 0;
	snake->tail = calloc(BOARD_WIDTH, sizeof(Position));
	snake->tailAllocated = BOARD_WIDTH;
	snake->tailLength = 0;
	snake->movement = RIGHT;
}

/**
 * Initialize a game.
 */
void initGame(Game *game) {
	for (size_t i = 0; i < NR_SNAKES; i++) {
		initSnake(&game->snakes[i]);
	}
	game->food.col = random() % BOARD_WIDTH;
	game->food.row = random() % BOARD_HEIGHT;
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
	if (snakeDead(game->snakes, next)) {
		return false;
	}
	if (positionsEqual(next, game->food)) {
		game->food.col = random() % BOARD_WIDTH;
		game->food.row = random() % BOARD_HEIGHT;
		extendTail(
			game->tail
			,&game->tailLength
			,&game->tailAllocated
			,game->head
		);
	} else {
		shiftTail(game->tail, game->tailLength, game->head);
	}
	game->head = next;
	return true;
}

/**
 * Draw a rectangle at the given position. The color etc. must be set before
 * calling this.
 *
 * @param position The position to draw a rectangle at.
 */
void drawCell(const Position position) {
	glRecti(position.col, position.row, position.col + 1, position.row + 1);
}

/**
 * @return Pointer to a string containing information about the game. The caller
 * must take care to free this after usage.
 */
char *gameInfo(const size_t score) {
	size_t allocated = sizeof("Score: ")
		+ (score == 0 ? 1 : (int) log10(score) + 1);
	char *str = malloc(allocated);
	snprintf(str, allocated, "Score: %lu", score);
	return str;
}

void drawGameInfo(const size_t score) {
	glRasterPos2i(1, BOARD_HEIGHT - 2);
	char *info = gameInfo(score);
	glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char*) info);
	free(info);
}

char *gameInfo(size_t score) {
	size_t allocated = sizeof("Score: ")
		+ (score == 0 ? 1 : (int) log10(score) + 1) + 1;
	char *str = malloc(allocated);
	snprintf(str, allocated, "Score : %lu", score);
	return str;
}

void display() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(.5, .5, .5);
	drawCell(game.head);
	glColor3f(1, 1, 1);
	for (size_t i = 0; i < game.tailLength; i++) {
		drawCell(game.tail[i]);
	}
	glColor3f(1, 0, 0);
	drawCell(game.food);
	glColor3f(0, 1, 0);
	drawGameInfo(game.tailLength);
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

void handleSpecial(int key, int x, int y) {
	(void) x; (void) y;
	switch (key) {
		case GLUT_KEY_LEFT: game.movement = LEFT; break;
		case GLUT_KEY_UP: game.movement = UP; break;
		case GLUT_KEY_RIGHT: game.movement = RIGHT; break;
		case GLUT_KEY_DOWN: game.movement = DOWN; break;
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
	glutSpecialFunc(handleSpecial);
	glutTimerFunc(STEP_DELAY, step, 0);
	gluOrtho2D(0, BOARD_WIDTH, 0, BOARD_HEIGHT);
	glutMainLoop();
	free(game.tail);
	return EXIT_SUCCESS;
}
