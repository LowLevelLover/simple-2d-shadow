#pragma once

#include <SDL2/SDL.h>
#include <gsl/gsl_matrix.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

#define ROWS 200
#define COLS 200

#define SCALE_FACTOR 5

#define WIDTH COLS *SCALE_FACTOR
#define HEIGHT ROWS *SCALE_FACTOR

typedef struct {
  int row;
  int col;
} pixel_point;

typedef struct {
  pixel_point **data;
  int size;
} pixel_vector;

typedef enum {
  Empty,
  Lamp,
  Light,
  Block,
  SeenBlock,
  Shadow,
} pixel_type;
