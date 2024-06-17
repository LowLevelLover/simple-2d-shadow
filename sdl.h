#pragma once

#include "constants.h"
#include "helper.h"

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Event event;

void draw(gsl_matrix *m);
void process_input(gsl_matrix *m);
