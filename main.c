#include "constants.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;

int lamp_row;
int lamp_col;

gsl_matrix *init_matrix() {
  srand(time(NULL));

  lamp_row = rand() % ROWS;
  lamp_col = rand() % COLS;

  gsl_matrix *m = gsl_matrix_alloc(ROWS, COLS);

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (rand() % 50 == 0) {
        gsl_matrix_set(m, row, col, Block);
      } else {
        gsl_matrix_set(m, row, col, Empty);
      }
    }
  }

  gsl_matrix_set(m, lamp_row, lamp_col, Lamp); // Lamp

  return m;
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

int in_matrix_row(int row) { return row < ROWS && row >= 0; }

int in_matrix_col(int col) { return col < COLS && col >= 0; }

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

pixel_vector get_line_vector_vertical(pixel_point *target_point) {
  int y = target_point->row;

  int is_up = y < lamp_row ? -1 : 1;

  int current_vector_pos = 0;
  int size = ROWS;

  pixel_point **points = malloc(size * sizeof(pixel_point *));

  for (int y_k = lamp_row + is_up; in_matrix_row(y_k); y_k += is_up) {
    points[current_vector_pos] = malloc(sizeof(pixel_point));
    points[current_vector_pos]->col = lamp_col;
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

pixel_vector get_line_vector(pixel_point *target_point) {
  int yp = target_point->row;
  int xp = target_point->col;

  if (xp == lamp_col)
    return get_line_vector_vertical(target_point);

  int size = COLS;
  pixel_point **points = malloc(size * sizeof(pixel_point *));

  int direction = xp < lamp_col ? -1 : 1;
  int current_vector_pos = 0;

  float m = (float)(yp - lamp_row) / (float)(xp - lamp_col);
  float b = ((float)lamp_row - m * (float)lamp_col);

  for (int x = xp; in_matrix_col(x); x += direction) {

    int y = round(m * x + b);

    if (!in_matrix_row(y)) {
      break;
    }

    points[current_vector_pos] = malloc(sizeof(pixel_point));
    points[current_vector_pos]->col = x;
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

gsl_matrix *get_expanded_shadow_matrix(gsl_matrix *m) {
  gsl_matrix *m2 = gsl_matrix_alloc(ROWS, COLS);
  gsl_matrix_memcpy(m2, m);

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (gsl_matrix_get(m, row, col) == Shadow) {
        pixel_point current_point = {row, col};
        pixel_vector shadow_cricle = get_circle_vector(&current_point, 1);

        for (int i = 0; i < shadow_cricle.size; i++) {
          if (gsl_matrix_get(m, shadow_cricle.data[i]->row,
                             shadow_cricle.data[i]->col) == Light) {
            gsl_matrix_set(m2, shadow_cricle.data[i]->row,
                           shadow_cricle.data[i]->col, Shadow);
          }
          free(shadow_cricle.data[i]);
        }
        free(shadow_cricle.data);
      }
    }
  }

  return m2;
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

void sdl_error_logger(const char *msg) {
  fprintf(stderr, "\nError: %s\n", msg);
  fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
}

int init_sdl() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    sdl_error_logger("SDL initialization failed");
    return 1;
  }

  if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_FOREIGN, &window,
                                  &renderer) != 0) {

    sdl_error_logger("Window and renderer initialization failed");
    return 1;
  }

  return 0;
}

void destroy_sdl() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int convert_to_256(double pixel_value) { return pixel_value * 255; }

void draw(gsl_matrix *m) {
  SDL_Rect pixel_rect = {0, 0, COLS * SCALE_FACTOR, ROWS * SCALE_FACTOR};

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &pixel_rect);

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      double pixel_value = gsl_matrix_get(m, row, col);

      if (pixel_value == Light) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      } else if (pixel_value < 1.0) {
        int pixel_color = convert_to_256(pixel_value);
        SDL_SetRenderDrawColor(renderer, pixel_color, pixel_color, pixel_color,
                               255);
      } else if (pixel_value == Lamp) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
      } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      }

      SDL_Rect pixel_rect = {col * SCALE_FACTOR, row * SCALE_FACTOR,
                             SCALE_FACTOR, SCALE_FACTOR};

      SDL_RenderFillRect(renderer, &pixel_rect);
    }
  }

  SDL_RenderPresent(renderer);
}

void process_input(gsl_matrix *m) {
  int close = FALSE;

  while (!close) {
    draw(m);

    while (SDL_PollEvent(&event)) {
      switch (event.type) {

      case SDL_QUIT:
        close = TRUE;
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
          close = TRUE;
          break;
        default:
          break;
        }
        break;
      }
    }
  }
}

int main() {
  gsl_matrix *m = init_matrix();

  set_brightness(m);

  gsl_matrix *m2 = get_expanded_shadow_matrix(m);
  gsl_matrix_free(m);

  gsl_matrix *m3 = get_smooth_shadow_matrix(m2);
  gsl_matrix_free(m2);

  // SDL START HERE
  if (init_sdl() != 0) {
    return 1;
  };

  process_input(m3);

  gsl_matrix_free(m3);
  destroy_sdl();
  return 0;
}
