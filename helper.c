#include "helper.h"

int in_matrix_row(int row) { return row < ROWS && row >= 0; }
int in_matrix_col(int col) { return col < COLS && col >= 0; }

void set_rect(gsl_matrix *m, int row, int col, int w, int h) {
  if (!in_matrix_row(row))
    row = 0;

  if (!in_matrix_col(col))
    col = 0;

  if (!in_matrix_row(row + h))
    h = ROWS - row - 1;

  if (!in_matrix_col(col + w))
    w = COLS - col - 1;

  for (int r = row; r <= row + h; r++) {
    for (int c = col; c <= col + w; c++) {
      gsl_matrix_set(m, r, c, Block);
    }
  }
}

float distance(pixel_point *target_point) {
  int dx = abs(target_point->col - lamp_col);
  int dy = abs(target_point->row - lamp_row);

  return sqrt(pow(dy, 2) + pow(dx, 2));
}

int in_lamp_range(pixel_point *target_point) {
  return distance(target_point) <= MAX_LAMP_RANGE;
}

int get_longest_radius() {
  int r_array[4] = {lamp_col, lamp_row, COLS - lamp_col - 1,
                    ROWS - lamp_row - 1};

  int longest_r = r_array[0];

  for (int i = 1; i < 4; i++) {
    if (r_array[i] > longest_r) {
      longest_r = r_array[i];
    }
  }

  return longest_r;
}

void print_matrix(gsl_matrix *m) {
  printf("\n");
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLS; y++) {
      printf(" %.2f ", gsl_matrix_get(m, x, y));
    }
    printf("\n");
  }
}

void sdl_error_logger(const char *msg) {
  fprintf(stderr, "\nError: %s\n", msg);
  fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
}

int convert_to_256(double pixel_value) { return pixel_value * 255; }
