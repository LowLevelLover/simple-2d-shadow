#pragma once

#include "constants.h"
#include "helper.h"
#include "vector.h"

void adjust_brightness(gsl_matrix *m, pixel_point *tp);
void remove_single_light_points(gsl_matrix *m);
gsl_matrix *get_smooth_shadow_matrix(gsl_matrix *m);
void apply_brightness_logic(gsl_matrix *m, pixel_vector *lv,
                            pixel_type base_point_value);
void set_brightness(gsl_matrix *m);
