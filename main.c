#include "brightness.h"
#include "constants.h"
#include "helper.h"
#include "sdl.h"
#include "vector.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;

int lamp_row;
int lamp_col;

gsl_matrix *init_matrix() {
  srand(time(NULL));

  gsl_matrix *m = gsl_matrix_alloc(ROWS, COLS);
  gsl_matrix_set_all(m, Empty);

  for (int rect = 0; rect < MAX_RECT_COUNT; rect++) {
    int row = rand() % ROWS;
    int col = rand() % COLS;
    int w = rand() % (COLS / 4);
    int h = rand() % (ROWS / 4);

    set_rect(m, row, col, w, h);
  }

  int free_lamp = TRUE;

  do {
    lamp_row = rand() % ROWS;
    lamp_col = rand() % COLS;
    pixel_point lamp_point = {lamp_row, lamp_col};

    pixel_vector cv = get_circle_vector(&lamp_point, 1);

    free_lamp = TRUE;

    for (int i = 0; i < cv.size; i++) {
      if (gsl_matrix_get(m, cv.data[i]->row, cv.data[i]->col) == Block) {
        free_lamp = FALSE;
      }
      free(cv.data[i]);
    }

    if (!free_lamp) {
      continue;
    }

    gsl_matrix_set(m, lamp_row, lamp_col, Lamp); // Lamp

  } while (!free_lamp);

  return m;
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

int main() {
  gsl_matrix *m = init_matrix();
  set_brightness(m);
  remove_single_light_points(m);

  gsl_matrix *m2 = get_smooth_shadow_matrix(m);
  gsl_matrix_free(m);

  // SDL START HERE
  if (init_sdl() != 0) {
    return 1;
  };

  process_input(m2);

  gsl_matrix_free(m2);
  destroy_sdl();
  return 0;
}
