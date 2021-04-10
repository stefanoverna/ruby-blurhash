#include <blurhash.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

static uint8_t linear_to_srgb(float value);
static float srgb_to_linear(uint8_t value);
static float sign_pow(float value, float exp);
static int base83_decode(const char *string, const int length);
static void fill_pixel_with_blurhash(float px, float py, int nx, int ny, float colors[][3], float *r, float *g, float *b);
static void rgb2hsv(uint8_t r255, uint8_t g255, uint8_t b255, float *hsv);
static void hsv2rgb(float *hsv, uint8_t *rgb);

void blurhash_free(uint8_t *pixels)
{
  free(pixels);
}

size_t blurhash_decode(
    char *blurhash,
    int width,
    int height,
    float punch,
    float *transform,
    enum FillMode fill_mode,
    uint8_t *fill_color,
    int saturation,
    uint8_t **rgb_pointer)
{
  uint8_t *result = (uint8_t *)malloc(sizeof(uint8_t) * width * height * 3);

  int number_of_components = base83_decode(blurhash, 1);

  int nx = (number_of_components % 9) + 1;
  int ny = (number_of_components / 9) + 1;

  float linear_fill_color[3];

  if (fill_color)
  {
    linear_fill_color[0] = srgb_to_linear(fill_color[0]);
    linear_fill_color[1] = srgb_to_linear(fill_color[1]);
    linear_fill_color[2] = srgb_to_linear(fill_color[2]);
  }

  float colors[ny * nx][3];
  memset(colors, 0, sizeof(colors));

  float real_max_value = ((float)(base83_decode(blurhash + 1, 1) + 1) / 166.0f) * punch;

  int dc_value = base83_decode(blurhash + 2, 4);

  colors[0][0] = srgb_to_linear(dc_value >> 16);
  colors[0][1] = srgb_to_linear((dc_value >> 8) & 255);
  colors[0][2] = srgb_to_linear(dc_value & 255);

  for (uint8_t i = 0; i < (nx * ny) - 1; i += 1)
  {
    int ac_value = base83_decode(blurhash + 6 + i * 2, 2);

    colors[i + 1][0] = real_max_value * sign_pow(((float)(ac_value / (19 * 19)) - 9.0f) / 9.0f, 2.0f);
    colors[i + 1][1] = real_max_value * sign_pow(((float)((ac_value / 19) % 19) - 9.0f) / 9.0f, 2.0f);
    colors[i + 1][2] = real_max_value * sign_pow(((float)(ac_value % 19) - 9.0f) / 9.0f, 2.0f);
  }

  uint8_t *pointer = result;

  for (int y = 0; y < height; y += 1)
  {
    for (int x = 0; x < width; x += 1)
    {
      float r = 0, g = 0, b = 0;

      float px = (float)x / width;
      float py = (float)y / height;

      float tx = px;
      float ty = py;

      if (transform)
      {
        tx = transform[0] * px + transform[1] * py + transform[2];
        ty = transform[3] * px + transform[4] * py + transform[5];
      }

      if (tx >= 0 && tx < 1.0 && ty >= 0 && ty < 1.0)
      {
        /* for (int j = 0; j < ny; j += 1) { */
        /*   for (int i = 0; i < nx; i += 1) { */
        /*     float basis = cosf(M_PI * i * tx) * cosf(M_PI * j * ty); */

        /*     r += colors[j * nx + i][0] * basis; */
        /*     g += colors[j * nx + i][1] * basis; */
        /*     b += colors[j * nx + i][2] * basis; */
        /*   } */
        /* } */
        fill_pixel_with_blurhash(tx, ty, nx, ny, colors, &r, &g, &b);
      }
      else
      {
        if (fill_mode == SOLID)
        {
          r = linear_fill_color[0];
          g = linear_fill_color[1];
          b = linear_fill_color[2];
        }
        else if (fill_mode == BLUR)
        {
          fill_pixel_with_blurhash(px, py, nx, ny, colors, &r, &g, &b);
        }
        else
        {
          if (tx < 0)
          {
            fill_pixel_with_blurhash(0, ty, nx, ny, colors, &r, &g, &b);
          }
          else if (tx >= 1)
          {
            fill_pixel_with_blurhash(1, ty, nx, ny, colors, &r, &g, &b);
          }
          else if (ty < 0)
          {
            fill_pixel_with_blurhash(tx, 0, nx, ny, colors, &r, &g, &b);
          }
          else if (ty >= 1)
          {
            fill_pixel_with_blurhash(tx, 1, nx, ny, colors, &r, &g, &b);
          }
        }
      }

      uint8_t sr = linear_to_srgb(r);
      uint8_t sg = linear_to_srgb(g);
      uint8_t sb = linear_to_srgb(b);

      if (saturation)
      {
        float hsv[3] = {0};
        rgb2hsv(sr, sg, sb, hsv);
        float mult = ((float) saturation + 100.0f) / 200.0f * 1.4f;
        hsv[1] = fminf(fmaxf(hsv[1] * mult, 0), 1.0f);
        hsv2rgb(hsv, pointer);
        pointer += 3;
      }
      else
      {
        *(pointer++) = sr;
        *(pointer++) = sg;
        *(pointer++) = sb;
      }
    }
  }

  *rgb_pointer = result;

  return width * height * 3;
}

void fill_pixel_with_blurhash(
    float px,
    float py,
    int nx,
    int ny,
    float colors[][3],
    float *r,
    float *g,
    float *b)
{
  for (int j = 0; j < ny; j += 1)
  {
    for (int i = 0; i < nx; i += 1)
    {
      float basis = cosf(M_PI * i * px) * cosf(M_PI * j * py);

      *r += colors[j * nx + i][0] * basis;
      *g += colors[j * nx + i][1] * basis;
      *b += colors[j * nx + i][2] * basis;
    }
  }
}

void free_pixels(uint8_t *pixels)
{
  free(pixels);
}

static uint8_t linear_to_srgb(float value)
{
  float v = fmaxf(0, fminf(1, value));
  if (v <= 0.0031308)
    return v * 12.92 * 255 + 0.5;
  else
    return (1.055 * powf(v, 1 / 2.4) - 0.055) * 255 + 0.5;
}

static float srgb_to_linear(uint8_t value)
{
  float v = (float)value / 255;
  if (v <= 0.04045)
    return v / 12.92;
  else
    return powf((v + 0.055) / 1.055, 2.4);
}

static float sign_pow(float value, float exp)
{
  return copysignf(powf(fabsf(value), exp), value);
}

static void rgb2hsv(uint8_t r255, uint8_t g255, uint8_t b255, float *hsv)
{
  float min, max, delta;

  float r = r255 / 255.0;
  float g = g255 / 255.0;
  float b = b255 / 255.0;

  min = r < g ? r : g;
  min = min < b ? min : b;

  max = r > g ? r : g;
  max = max > b ? max : b;

  hsv[2] = max;

  delta = max - min;

  if (delta < 0.00001)
  {
    hsv[1] = 0;
    hsv[0] = 0; // undefined, maybe nan?
    return;
  }
  if (max > 0.0)
  {                         // NOTE: if Max is == 0, this divide would cause a crash
    hsv[1] = (delta / max); // s
  }
  else
  {
    // if max is 0, then r = g = b = 0
    // s = 0, h is undefined
    hsv[1] = 0.0;
    hsv[0] = NAN; // its now undefined
    return;
  }

  if (r >= max)               // > is bogus, just keeps compilor happy
    hsv[0] = (g - b) / delta; // between yellow & magenta
  else if (g >= max)
    hsv[0] = 2.0 + (b - r) / delta; // between cyan & yellow
  else
    hsv[0] = 4.0 + (r - g) / delta; // between magenta & cyan

  hsv[0] *= 60.0; // degrees

  if (hsv[0] < 0.0)
    hsv[0] += 360.0;
}

static void hsv2rgb(float *hsv, uint8_t *rgb)
{
  float hh, p, q, t, ff;
  long i;

  if (hsv[1] <= 0.0)
  { // < is bogus, just shuts up warnings
    rgb[0] = hsv[2] * 255.0;
    rgb[1] = hsv[2] * 255.0;
    rgb[2] = hsv[2] * 255.0;
    return;
  }

  hh = hsv[0];

  if (hh >= 360.0)
    hh = 0.0;

  hh /= 60.0;

  i = (long)hh;

  ff = hh - i;
  p = hsv[2] * (1.0 - hsv[1]);
  q = hsv[2] * (1.0 - (hsv[1] * ff));
  t = hsv[2] * (1.0 - (hsv[1] * (1.0 - ff)));

  switch (i)
  {
  case 0:
    rgb[0] = hsv[2] * 255.0;
    rgb[1] = t * 255.0;
    rgb[2] = p * 255.0;
    return;
  case 1:
    rgb[0] = q * 255.0;
    rgb[1] = hsv[2] * 255.0;
    rgb[2] = p * 255.0;
    return;
  case 2:
    rgb[0] = p * 255.0;
    rgb[1] = hsv[2] * 255.0;
    rgb[2] = t * 255.0;
    return;

  case 3:
    rgb[0] = p * 255.0;
    rgb[1] = q * 255.0;
    rgb[2] = hsv[2] * 255.0;
    return;
  case 4:
    rgb[0] = t * 255.0;
    rgb[1] = p * 255.0;
    rgb[2] = hsv[2] * 255.0;
    return;
  case 5:
  default:
    rgb[0] = hsv[2] * 255.0;
    rgb[1] = p * 255.0;
    rgb[2] = q * 255.0;
    return;
  }
}

static char characters[83] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#$%*+,-.:;=?@[]^_{|}~";

static int base83_decode(const char *string, const int length)
{
  int acc = 0;

  for (int i = 0; i < length; i++)
  {
    char *ptr = strchr(characters, string[i]);
    acc = acc * 83 + (ptr - characters);
  }

  return acc;
}
