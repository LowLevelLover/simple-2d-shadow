#pragma once
#include "constants.h"

extern int lamp_row;
extern int lamp_col;

int in_matrix_row(int row);
int in_matrix_col(int col);

void set_rect(gsl_matrix *m, int row, int col, int w, int h);

float distance(pixel_point *target_point);
int in_lamp_range(pixel_point *target_point);

int get_longest_radius();

void print_matrix(gsl_matrix *m);
void sdl_error_logger(const char *msg);

int convert_to_256(double pixel_value);
