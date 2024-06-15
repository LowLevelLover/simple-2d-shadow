#include "constants.h"
#include <stdio.h>

static const pixel_point block_cells[] = {
    {2, 1}, {3, 1}, {2, 2}, {3, 2}, {1, 4},
    {2, 4}, {3, 4}, {7, 4}, {7, 5}, {8, 6},
};

gsl_matrix *init_matrix() {
  gsl_matrix *m = gsl_matrix_alloc(ROWS, COLS);

  gsl_matrix_set_all(m, Empty);

  unsigned int block_cells_length =
      sizeof(block_cells) / sizeof(block_cells[0]);

  for (unsigned int i = 0; i < block_cells_length; i++) {
    gsl_matrix_set(m, block_cells[i].row, block_cells[i].col, Block);
  }

  gsl_matrix_set(m, INIT_LAMP_ROW, INIT_LAMP_COL, Light); // Lamp

  return m;
}

void print_matrix(gsl_matrix *m) {
  printf("\n");
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLS; y++) {
      printf(" %.0f ", gsl_matrix_get(m, x, y));
    }
    printf("\n");
  }
}

int in_matrix_row(int row) { return row < ROWS && row >= 0; }

int in_matrix_col(int col) { return col < COLS && col >= 0; }

pixel_vector get_circle_vector(unsigned int r) {
  /*
    LEFT:
      col = colL-r
      row = [rowL-r, rowL+r]

    TOP:
      row = rowL-r
      col = [colL-r+1, colL+r]

    RIGHT:
      col = colL+r
      row = [rowL-r+1, rowL+r]

    BOTTOM:
      row = rowL+r
      col = [colL-r+1, colL+r-1]
  */

  unsigned int left_length = 2 * r + 1;
  unsigned int top_length = 2 * r;
  unsigned int right_length = 2 * r;
  unsigned int bottom_length = 2 * r - 1;

  unsigned int current_pos = 0;

  pixel_point *circle = (pixel_point *)malloc(8 * r * sizeof(pixel_point));

  for (unsigned int i = 0; i < left_length; i++) {
    if (!in_matrix_row(INIT_LAMP_ROW - r + i)) {
      continue;
    }

    if (!in_matrix_col(INIT_LAMP_COL - r)) {
      break;
    }

    circle[current_pos].row = INIT_LAMP_ROW - r + i;
    circle[current_pos].col = INIT_LAMP_COL - r;
    current_pos++;
  }

  for (unsigned int i = 0; i < top_length; i++) {
    if (!in_matrix_row(INIT_LAMP_ROW - r)) {
      break;
    }

    if (!in_matrix_col(INIT_LAMP_COL - r + i + 1)) {
      continue;
    }

    circle[current_pos].row = INIT_LAMP_ROW - r;
    circle[current_pos].col = INIT_LAMP_COL - r + i + 1;
    current_pos++;
  }

  for (unsigned int i = 0; i < right_length; i++) {
    if (!in_matrix_row(INIT_LAMP_ROW - r + i + 1)) {
      continue;
    }

    if (!in_matrix_col(INIT_LAMP_COL + r)) {
      break;
    }

    circle[current_pos].row = INIT_LAMP_ROW - r + i + 1;
    circle[current_pos].col = INIT_LAMP_COL + r;
    current_pos++;
  }

  for (unsigned int i = 0; i < bottom_length; i++) {
    if (!in_matrix_row(INIT_LAMP_ROW + r)) {
      break;
    }

    if (!in_matrix_col(INIT_LAMP_COL - r + i + 1)) {
      continue;
    }

    circle[current_pos].row = INIT_LAMP_ROW + r;
    circle[current_pos].col = INIT_LAMP_COL - r + i + 1;
    current_pos++;
  }

  pixel_vector cv = {
      circle,
      current_pos,
  };

  return cv; // SIZE: 8 * r
}

pixel_vector get_line_vector(pixel_point *target_point) {
  int y = target_point->row;
  int x = target_point->col;

  unsigned int size = ROWS > COLS ? ROWS : COLS;
  pixel_point *points = (pixel_point *)malloc(size * sizeof(pixel_point));

  pixel_vector line_vector = {
      points,
      size,
  };

  if (x == INIT_LAMP_COL) {
    int is_up = y < INIT_LAMP_ROW ? -1 : 1;
    unsigned int current_vector_pos = 0;

    for (int y_k = INIT_LAMP_ROW + is_up; in_matrix_row(y_k); y_k += is_up) {
      points[current_vector_pos].col = INIT_LAMP_COL;
      points[current_vector_pos].row = y_k;
      current_vector_pos++;
    }

    line_vector.size = current_vector_pos;

    return line_vector;
  }

  int is_right = x < INIT_LAMP_COL ? -1 : 1;
  unsigned int current_vector_pos = 0;

  float m = (float)(y - INIT_LAMP_ROW) / (x - INIT_LAMP_COL);
  float b = (INIT_LAMP_ROW - m * INIT_LAMP_COL);

  for (int x_k = x; in_matrix_col(x_k);
       x_k += is_right) {

    float y1 = m * x_k + b;

    if (y1 != (int)y1) {
      points[current_vector_pos].col = (int)y1 + 1;
      points[current_vector_pos].row = x_k;
      current_vector_pos++;
    }

    points[current_vector_pos].col = (int)y1;
    points[current_vector_pos].row = x_k;
    current_vector_pos++;
  }

  line_vector.size = current_vector_pos;

  return line_vector;
}

int main() {
  gsl_matrix *m = init_matrix();

  print_matrix(m);

  return 0;
}
