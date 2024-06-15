#include "constants.h"
#include <gsl/gsl_matrix_double.h>
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

int main() {
  gsl_matrix *m = init_matrix();
  print_matrix(m);

  return 0;
}
