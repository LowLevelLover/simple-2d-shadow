#include <gsl/gsl_matrix.h>

#define TRUE 1
#define FALSE 0

#define ROWS 10
#define COLS 7

#define INIT_LAMP_ROW 5
#define INIT_LAMP_COL 3

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
