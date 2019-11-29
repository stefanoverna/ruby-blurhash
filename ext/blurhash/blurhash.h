#ifndef __BLURHASH_H__
#define __BLURHASH_H__

#include <stdint.h>
#include <stdlib.h>

enum FillMode {
  SOLID = 1,
  BLUR,
  CLAMP,
};

size_t blurhash_decode(
  char *blurhash,
  int width,
  int height,
  float punch,
  float *homogeneous_transform,
  enum FillMode fill_mode,
  uint8_t *fill_color,
  uint8_t **pixels
);

void blurhash_free(
  uint8_t *pixels
);

#endif
