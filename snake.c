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

typedef enum SnakeStatus {ALIVE, DEAD, HAS_EATEN} SnakeStatus;

typedef GLfloat Color[3];

typedef struct Snake {
	Position head;
	Position *tail;
	size_t tailLength;
	size_t tailAllocated;
	Direction movement;
	Color color;
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
bool positionsEqual(const Position p1, const Position p2) {
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
void shiftTail(Snake *snake) {
	if (snake->tailLength) {
		for (size_t i = 0; i < snake->tailLength - 1; i++) {
			snake->tail[i] = snake->tail[i + 1];
		}
		snake->tail[snake->tailLength - 1].col = snake->head.col;
		snake->tail[snake->tailLength - 1].row = snake->head.row;
	}
}

/**
 * Extend the tail by appending the head. This function will reallocate the tail
 * if it is not big enough to hold another element.
 */
void extendTail(Snake *snake) {
	snake->tailLength++;
	if (snake->tailLength > snake->tailAllocated) {
		snake->tailAllocated += BOARD_WIDTH;
		snake->tail = realloc(
			snake->tail
			,snake->tailAllocated * sizeof(Position)
		);
	}
	snake->tail[snake->tailLength - 1] = snake->head;
}

/**
 * Initialize a snake according to its index. This function only provides 4
 * distinct states.
 */
void initSnakePositional(Snake *snake, const size_t index) {
	switch (index) {
		case 1:
			snake->head.col = BOARD_WIDTH - 1;
			snake->head.row = BOARD_HEIGHT - 1;
			snake->movement = LEFT;
			snake->color[0] = .5;
			snake->color[1] = 0;
			snake->color[2] = .5;
			break;
		case 2:
			snake->head.col = 0;
			snake->head.row = BOARD_HEIGHT - 1;
			snake->movement = DOWN;
			snake->color[0] = 0;
			snake->color[1] = 1;
			snake->color[2] = 0;
			break;
		case 3:
			snake->head.col = BOARD_WIDTH - 1;
			snake->head.row = 0;
			snake->movement = UP;
			snake->color[0] = 0;
			snake->color[1] = 0;
			snake->color[2] = 1;
			break;
		default:
			snake->head.col = 0;
			snake->head.row = 0;
			snake->movement = RIGHT;
			snake->color[0] = .5;
			snake->color[1] = .5;
			snake->color[2] = .5;
	}
}

/**
 * Initialize the snakes.
 */
void initSnakes(Snake *snakes) {
	Snake *snake;
	for (size_t i = 0; i < NR_SNAKES; i++) {
		snake = (snakes + i);
		snake->tail = calloc(BOARD_WIDTH, sizeof(Position));
		snake->tailAllocated = BOARD_WIDTH;
		snake->tailLength = 0;
		initSnakePositional(snake, i);
	}
}

/**
 * Initialize a game.
 */
void initGame(Game *game) {
	initSnakes(game->snakes);
	game->food.col = random() % BOARD_WIDTH;
	game->food.row = random() % BOARD_HEIGHT;
}

/**
 * Free the memory allocated for all snakes.
 */
void freeSnakes(Snake *snakes) {
	for (size_t i = 0; i < NR_SNAKES; i++) {
		free(snakes[i].tail);
	}
}

/**
 * Get the status of a snake after taking the next step and set next to the
 * position its head will occupy after taking that step.
 */
SnakeStatus stepSnake(
	Snake *snakes
	,const size_t snakeIndex
	,const Position food
	,Position *next
) {
	next->col = snakes[snakeIndex].head.col
		+ colDelta(snakes[snakeIndex].movement);
	next->row = snakes[snakeIndex].head.row
		+ rowDelta(snakes[snakeIndex].movement);
	if (snakeDead(snakes, *next)) {
		return DEAD;
	}
	if (positionsEqual(*next, food)) {
		return HAS_EATEN;
	}
	return ALIVE;
}

/**
 * Advance the game state one tick.
 *
 * @param game The game.
 *
 * @return Whether the snakes are still alive.
 */
bool tick(Game *game) {
	for (size_t i = 0; i < NR_SNAKES; i++) {
		Position next;
		switch (stepSnake(game->snakes, i, game->food, &next)) {
			case DEAD: return false;
			case HAS_EATEN:
				game->food.col = random() % BOARD_WIDTH;
				game->food.row = random() % BOARD_HEIGHT;
				extendTail(&game->snakes[i]);
				break;
			case ALIVE:
				shiftTail(&game->snakes[i]);
				break;
		}
		game->snakes[i].head = next;
	}
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
	for (size_t k = 0; k < NR_SNAKES; k++) {
		glColor3fv(game.snakes[k].color);
		drawCell(game.snakes[k].head);
		glColor3f(1, 1, 1);
		for (size_t i = 0; i < game.snakes[k].tailLength; i++) {
			drawCell(game.snakes[k].tail[i]);
		}
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
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutMainLoop();
	freeSnakes(game.snakes);
	return EXIT_SUCCESS;
}
