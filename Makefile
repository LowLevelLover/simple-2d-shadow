building:main.c
	gcc -Wall -Wextra -std=c11 -g -lgsl -lm -lgslcblas -lSDL2 -o build/main main.c

run:building
	./build/main

clean:
	rm -rf build/*
