#pragma once

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <cglm/struct.h>


vec4s hex_to_rgba(char colorcode[7], float alpha);
