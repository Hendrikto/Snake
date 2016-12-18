all: snake

snake: snake.c
	gcc -Wall -Wextra -O3 -o snake snake.c -lglut -lGL

clean:
	rm snake
