#include <blurhash.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

static uint8_t linear_to_srgb(float value);
static float srgb_to_linear(uint8_t value);
static float sign_pow(float value, float exp);
static int base83_decode(const char *string, const int length);

void blurhash_free(uint8_t *pixels) {
  free(pixels);
}

size_t blurhash_decode(
  char *blurhash,
  int width,
  int height,
  float punch,
  float *transform,
  uint8_t **out_pointer
) {
  uint8_t *result = (uint8_t *) malloc(sizeof(uint8_t) * width * height * 3);

  int number_of_components = base83_decode(blurhash, 1);

  int nx = (number_of_components % 9) + 1;
  int ny = (number_of_components / 9) + 1;

  float colors[ny * nx][3];
  memset(colors, 0, sizeof(colors));

  float real_max_value = ((float) (base83_decode(blurhash + 1, 1) + 1) / 166.0f) * punch;

  int dc_value = base83_decode(blurhash + 2, 4);

  colors[0][0] = srgb_to_linear(dc_value >> 16);
  colors[0][1] = srgb_to_linear((dc_value >> 8) & 255);
  colors[0][2] = srgb_to_linear(dc_value & 255);

  for (uint8_t i = 0; i < (nx * ny) - 1; i += 1) {
    int ac_value = base83_decode(blurhash + 6 + i * 2, 2);

    colors[i + 1][0] = real_max_value * sign_pow(((float) (ac_value / (19 * 19)) - 9.0f) / 9.0f, 2.0f);
    colors[i + 1][1] = real_max_value * sign_pow(((float) ((ac_value / 19) % 19) - 9.0f) / 9.0f, 2.0f);
    colors[i + 1][2] = real_max_value * sign_pow(((float) (ac_value % 19) - 9.0f) / 9.0f, 2.0f);
  }

  uint8_t *pointer = result;

  for (int y = 0; y < height; y += 1) {
    for (int x = 0; x < width; x += 1) {
      float r = 0, g = 0, b = 0;

      float px = (float) x / width;
      float py = (float) y / height;

      float tx = px;
      float ty = py;

      if (transform) {
        tx = transform[0] * px + transform[1] * py + transform[2];
        ty = transform[3] * px + transform[4] * py + transform[5];
      }

      for (int j = 0; j < ny; j += 1) {
        for (int i = 0; i < nx; i += 1) {
          float basis = cosf(M_PI * i * tx) * cosf(M_PI * j * ty);

          r += colors[j * nx + i][0] * basis;
          g += colors[j * nx + i][1] * basis;
          b += colors[j * nx + i][2] * basis;
        }
      }


      *(pointer++) = linear_to_srgb(r);
      *(pointer++) = linear_to_srgb(g);
      *(pointer++) = linear_to_srgb(b);
    }
  }

  *out_pointer = result;

  return width * height * 3;
}

void free_pixels(uint8_t *pixels) {
  free(pixels);
}

static uint8_t linear_to_srgb(float value) {
  float v = fmaxf(0, fminf(1, value));
  if(v <= 0.0031308) return v * 12.92 * 255 + 0.5;
  else return (1.055 * powf(v, 1 / 2.4) - 0.055) * 255 + 0.5;
}

static float srgb_to_linear(uint8_t value) {
  float v = (float) value / 255;
  if(v <= 0.04045) return v / 12.92;
  else return powf((v + 0.055) / 1.055, 2.4);
}

static float sign_pow(float value, float exp) {
  return copysignf(powf(fabsf(value), exp), value);
}

static char characters[83] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#$%*+,-.:;=?@[]^_{|}~";

static int base83_decode(const char *string, const int length) {
  int acc = 0;

  for (int i = 0; i < length; i++) {
    char *ptr = strchr(characters, string[i]);
    acc = acc * 83 + (ptr - characters);
  }

  return acc;
}
