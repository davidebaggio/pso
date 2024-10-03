all:
	gcc -I./include/ main.c -o swarm -O1 -Wall -std=c99 -Wno-missing-braces -L./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm