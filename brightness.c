#include "brightness.h"

void adjust_brightness(gsl_matrix *m, pixel_point *tp) {
  int row = tp->row;
  int col = tp->col;

  if (gsl_matrix_get(m, row, col) == Light) {
    if (!in_lamp_range(tp)) {
      gsl_matrix_set(m, row, col, Shadow);
    } else {
      gsl_matrix_set(m, row, col, 1 - (distance(tp) / MAX_LAMP_RANGE));
    }
  }
}

void remove_single_light_points(gsl_matrix *m) {
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (gsl_matrix_get(m, row, col) == Light) {
        int is_single = TRUE;
        pixel_point tp = {row, col};
        pixel_vector cv = get_circle_vector(&tp, 1);

        for (int i = 0; i < cv.size; i++) {
          if (gsl_matrix_get(m, cv.data[i]->row, cv.data[i]->col) == Light) {
            is_single = FALSE;
            break;
          }
        }

        if (is_single) {
          gsl_matrix_set(m, row, col, Shadow);
        }

        adjust_brightness(m, &tp);

        free(cv.data);
      }
    }
  }
}

gsl_matrix *get_smooth_shadow_matrix(gsl_matrix *m) {
  gsl_matrix *m2 = gsl_matrix_alloc(ROWS, COLS);
  gsl_matrix_memcpy(m2, m);

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (gsl_matrix_get(m, row, col) == Shadow) {
        pixel_point current_point = {row, col};
        pixel_vector shadow_cricle = get_circle_vector(&current_point, 1);
        int light_pixels = 0;
        int all_pixels = 0;

        for (int i = 0; i < shadow_cricle.size; i++) {
          if (gsl_matrix_get(m, shadow_cricle.data[i]->row,
                             shadow_cricle.data[i]->col) != SeenBlock) {
            all_pixels++;
          }
          if (gsl_matrix_get(m, shadow_cricle.data[i]->row,
                             shadow_cricle.data[i]->col) == Light) {
            light_pixels++;
          }
          free(shadow_cricle.data[i]);
        }
        free(shadow_cricle.data);

        gsl_matrix_set(m2, row, col, (double)light_pixels / all_pixels);
      }
    }
  }

  return m2;
}

void apply_brightness_logic(gsl_matrix *m, pixel_vector *lv,
                            pixel_type base_point_value) {

  for (int k = 0; k < lv->size; k++) {
    int row = lv->data[k]->row;
    int col = lv->data[k]->col;

    pixel_type current_value = gsl_matrix_get(m, row, col);

    if (k == 0) {
      if (base_point_value == Empty) {
        gsl_matrix_set(m, lv->data[0]->row, lv->data[0]->col, Light);
      }

      if (base_point_value == Block) {
        gsl_matrix_set(m, lv->data[0]->row, lv->data[0]->col, SeenBlock);
      }

      continue;
    }

    if (current_value == Block) {
      gsl_matrix_set(m, row, col, SeenBlock);
      continue;
    }

    int pre_row = lv->data[k - 1]->row;
    int pre_col = lv->data[k - 1]->col;

    free(lv->data[k - 1]);

    pixel_type previous_value = gsl_matrix_get(m, pre_row, pre_col);

    if (current_value == Empty) {
      if (previous_value == Light) {
        gsl_matrix_set(m, row, col, Light);
      } else {
        gsl_matrix_set(m, row, col, Shadow);
      }
    }
  }
  if (lv->size >= 1)
    free(lv->data[lv->size - 1]);
}

void set_brightness(gsl_matrix *m) {
  int longest_r = get_longest_radius();

  for (int r = 1; r <= longest_r; r++) {
    pixel_point light_point = {lamp_row, lamp_col};
    pixel_vector cv = get_circle_vector(&light_point, r);

    for (int i = 0; i < cv.size; i++) {
      pixel_vector lv = get_line_vector(cv.data[i]);
      pixel_type base_point_value =
          gsl_matrix_get(m, cv.data[i]->row, cv.data[i]->col);

      free(cv.data[i]);

      if (base_point_value == Empty)
        apply_brightness_logic(m, &lv, base_point_value);

      free(lv.data);
    }

    free(cv.data);
  }
}
