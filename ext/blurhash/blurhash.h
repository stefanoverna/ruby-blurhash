#ifndef __BLURHASH_H__
#define __BLURHASH_H__

#include <stdint.h>
#include <stdlib.h>

size_t blurhash_decode(
  char *blurhash,
  int width,
  int height,
  float punch,
  uint8_t **pixels
);

void blurhash_free(
  uint8_t *pixels
);

#endif