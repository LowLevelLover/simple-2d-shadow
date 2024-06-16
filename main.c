#include "constants.h"
#include <gsl/gsl_matrix_double.h>
#include <math.h>
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

  gsl_matrix_set(m, INIT_LAMP_ROW, INIT_LAMP_COL, Lamp); // Lamp

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

pixel_vector get_line_vector_vertical(pixel_point *target_point) {
  int y = target_point->row;

  int is_up = y < INIT_LAMP_ROW ? -1 : 1;

  unsigned int current_vector_pos = 0;
  unsigned int size = ROWS > COLS ? ROWS : COLS;

  pixel_point *points = (pixel_point *)malloc(size * sizeof(pixel_point));

  for (int y_k = INIT_LAMP_ROW + is_up; in_matrix_row(y_k); y_k += is_up) {
    points[current_vector_pos].col = INIT_LAMP_COL;
    points[current_vector_pos].row = y_k;
    current_vector_pos++;
  }

  pixel_vector line_vector = {
      points,
      current_vector_pos,
  };

  return line_vector;
}

pixel_vector get_line_vector(pixel_point *target_point) {
  int yp = target_point->row;
  int xp = target_point->col;

  if (xp == INIT_LAMP_COL)
    return get_line_vector_vertical(target_point);

  unsigned int size = ROWS > COLS ? ROWS : COLS;
  pixel_point *points = (pixel_point *)malloc(2 * size * sizeof(pixel_point));

  int is_right = xp < INIT_LAMP_COL ? -1 : 1;
  unsigned int current_vector_pos = 0;

  float m = (float)(yp - INIT_LAMP_ROW) / (xp - INIT_LAMP_COL);
  float b = (INIT_LAMP_ROW - m * INIT_LAMP_COL);

  for (int x_k = INIT_LAMP_COL + is_right; in_matrix_col(x_k); x_k += is_right) {

    float y1 = m * x_k + b;

    if (!in_matrix_row(floor(y1))) {
      break;
    }

    if (ceil(y1) != floor(y1) && in_matrix_row(ceil(y1))) {
      points[current_vector_pos].col = x_k;
      points[current_vector_pos].row = ceil(y1);
      current_vector_pos++;
    }

    points[current_vector_pos].col = x_k;
    points[current_vector_pos].row = floor(y1);
    current_vector_pos++;
  }

  pixel_vector line_vector = {
      points,
      size,
  };

  line_vector.size = current_vector_pos;

  return line_vector;
}

unsigned int get_longest_radius() {
  unsigned int r_array[4] = {INIT_LAMP_COL, INIT_LAMP_ROW,
                             COLS - INIT_LAMP_COL - 1,
                             ROWS - INIT_LAMP_ROW - 1};

  unsigned int longest_r = r_array[0];

  for (int i = 1; i < 4; i++) {
    if (r_array[i] > longest_r) {
      longest_r = r_array[i];
    }
  }

  return longest_r;
}

void set_brightness(gsl_matrix *m) {
  unsigned int longest_r = get_longest_radius();

  for (unsigned int r = 1; r <= longest_r; r++) {
    pixel_vector cv = get_circle_vector(r);

    for (unsigned int i = 0; i < cv.size; i++) {
      pixel_vector lv = get_line_vector(&cv.data[i]);
      pixel_type base_point_value =
          gsl_matrix_get(m, lv.data[0].row, lv.data[0].col);

      for (unsigned int k = 0; k < lv.size; k++) {
        unsigned int row = lv.data[k].row;
        unsigned int col = lv.data[k].col;

        pixel_type current_value = gsl_matrix_get(m, row, col);

        if (k == 0) {
          if (base_point_value == Empty) {
            gsl_matrix_set(m, lv.data[0].row, lv.data[0].col, Light);
          }

          if (base_point_value == Block) {
            gsl_matrix_set(m, lv.data[0].row, lv.data[0].col, SeenBlock);
          }

          continue;
        }

        unsigned int pre_row = lv.data[k - 1].row;
        unsigned int pre_col = lv.data[k - 1].col;

        pixel_type previous_value = gsl_matrix_get(m, pre_row, pre_col);

        if (current_value == Block) {
          gsl_matrix_set(m, row, col, SeenBlock);
          continue;
        }

        if (current_value == Empty) {
          if (previous_value == Light) {
            gsl_matrix_set(m, row, col, Light);
          } else {
            gsl_matrix_set(m, row, col, Shadow);
          }
        }
      }
    }
  }
}

int main() {
  gsl_matrix *m = init_matrix();

  print_matrix(m);

  printf("\n*************\n");

  set_brightness(m);
  print_matrix(m);

  gsl_matrix_free(m);
  return 0;
}
