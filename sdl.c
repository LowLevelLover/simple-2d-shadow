#include "sdl.h"

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
