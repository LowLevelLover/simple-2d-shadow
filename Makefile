building:main.c
	gcc -Wall -Wextra -std=c11 -g -lgsl -lm -lgslcblas -o build/main main.c

run:building
	./build/main

clean:
	rm -rf build/*
