#include "vector.h"

pixel_vector get_circle_vector(pixel_point *center_point, int r) {
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

  int left_length = 2 * r + 1;
  int top_length = 2 * r;
  int right_length = 2 * r;
  int bottom_length = 2 * r - 1;

  int current_pos = 0;

  pixel_point **circle = malloc(8 * r * sizeof(pixel_point *));

  for (int i = 0; i < left_length; i++) {
    if (!in_matrix_row(center_point->row - r + i)) {
      continue;
    }

    if (!in_matrix_col(center_point->col - r)) {
      break;
    }
    circle[current_pos] = malloc(sizeof(pixel_point));
    circle[current_pos]->row = center_point->row - r + i;
    circle[current_pos]->col = center_point->col - r;
    current_pos++;
  }

  for (int i = 0; i < top_length; i++) {
    if (!in_matrix_row(center_point->row - r)) {
      break;
    }

    if (!in_matrix_col(center_point->col - r + i + 1)) {
      continue;
    }

    circle[current_pos] = malloc(sizeof(pixel_point));
    circle[current_pos]->row = center_point->row - r;
    circle[current_pos]->col = center_point->col - r + i + 1;
    current_pos++;
  }

  for (int i = 0; i < right_length; i++) {
    if (!in_matrix_row(center_point->row - r + i + 1)) {
      continue;
    }

    if (!in_matrix_col(center_point->col + r)) {
      break;
    }

    circle[current_pos] = malloc(sizeof(pixel_point));
    circle[current_pos]->row = center_point->row - r + i + 1;
    circle[current_pos]->col = center_point->col + r;
    current_pos++;
  }

  for (int i = 0; i < bottom_length; i++) {
    if (!in_matrix_row(center_point->row + r)) {
      break;
    }

    if (!in_matrix_col(center_point->col - r + i + 1)) {
      continue;
    }

    circle[current_pos] = malloc(sizeof(pixel_point));
    circle[current_pos]->row = center_point->row + r;
    circle[current_pos]->col = center_point->col - r + i + 1;
    current_pos++;
  }

  pixel_point **result = malloc(current_pos * sizeof(pixel_point *));

  pixel_vector cv = {
      memcpy(result, circle, current_pos * sizeof(pixel_point *)),
      current_pos,
  };

  free(circle);

  return cv; // SIZE: 8 * r
}

pixel_vector get_bigger_m_line_vector(float m, float b,
                                      pixel_point *target_point) {
  int yp = target_point->row;
  int direction = yp < lamp_row ? -1 : 1;

  int current_vector_pos = 0;
  int size = ROWS;

  pixel_point **points = malloc(size * sizeof(pixel_point *));

  for (int y_k = yp; in_matrix_row(y_k); y_k += direction) {
    int x = round(((float)y_k - b) / m);

    if (target_point->col == lamp_col) {
      x = lamp_col;
    }

    if (!in_matrix_col(x)) {
      break;
    }

    points[current_vector_pos] = malloc(sizeof(pixel_point));
    points[current_vector_pos]->col = x;
    points[current_vector_pos]->row = y_k;
    current_vector_pos++;
  }

  pixel_point **result = malloc(current_vector_pos * sizeof(pixel_point *));

  pixel_vector line_vector = {
      memcpy(result, points, current_vector_pos * sizeof(pixel_point *)),
      current_vector_pos,
  };

  free(points);

  return line_vector;
}

pixel_vector get_smaller_m_line_vector(float m, float b,
                                       pixel_point *target_point) {
  int xp = target_point->col;
  int direction = xp < lamp_col ? -1 : 1;

  int current_vector_pos = 0;
  int size = COLS;

  pixel_point **points = malloc(size * sizeof(pixel_point *));

  for (int x_k = xp; in_matrix_col(x_k); x_k += direction) {
    int y = round(m * (float)x_k + b);

    if (!in_matrix_row(y)) {
      break;
    }

    points[current_vector_pos] = malloc(sizeof(pixel_point));
    points[current_vector_pos]->col = x_k;
    points[current_vector_pos]->row = y;
    current_vector_pos++;
  }

  pixel_point **result = malloc(current_vector_pos * sizeof(pixel_point *));

  pixel_vector line_vector = {
      memcpy(result, points, current_vector_pos * sizeof(pixel_point *)),
      current_vector_pos,
  };

  free(points);

  return line_vector;
}

pixel_vector get_line_vector(pixel_point *target_point) {
  int yp = target_point->row;
  int xp = target_point->col;

  float m = (float)(yp - lamp_row) / (float)(xp - lamp_col);
  float b = ((float)lamp_row - m * (float)lamp_col);

  pixel_vector (*line_vector_f[2])(float, float, pixel_point *) = {
      get_smaller_m_line_vector, get_bigger_m_line_vector};

  return line_vector_f[fabs(m) > 1.0](m, b, target_point);
}
