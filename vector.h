#pragma once

#include "constants.h"
#include "helper.h"

pixel_vector get_circle_vector(pixel_point *center_point, int r);
pixel_vector get_smaller_m_line_vector(float m, float b,
                                       pixel_point *target_point);
pixel_vector get_bigger_m_line_vector(float m, float b,
                                      pixel_point *target_point);
pixel_vector get_line_vector(pixel_point *target_point);
