#pragma once

#include <SDL2/SDL.h>
#include <gsl/gsl_matrix.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define ROWS 10
#define COLS 7

#define INIT_LAMP_ROW 5
#define INIT_LAMP_COL 3

#define SCALE_FACTOR 50

#define WIDTH COLS *SCALE_FACTOR
#define HEIGHT ROWS *SCALE_FACTOR

typedef struct {
  unsigned int row;
  unsigned int col;
} pixel_point;

typedef struct {
  pixel_point *data;
  unsigned int size;
} pixel_vector;

typedef enum {
  Empty,
  Lamp,
  Light,
  Block,
  SeenBlock,
  Shadow,
} pixel_type;
