#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static inline uint8_t clampU8(int32_t v) {
	return v < 0 ? 0 : (v > 255 ? 255 : v);
}

typedef struct {
	int8_t offs[4][2];
	int8_t fac[4];
	uint8_t div;
	uint8_t count;
} DitherScheme;

static const DitherScheme DITHER_NONE = { .count = 0 };
static const DitherScheme DITHER_FS = {
	.offs = { {1,0}, {-1,1}, {0,1}, {1,1} },
	.fac  = { 7, 3, 5, 1 },
	.div  = 16,
	.count= 4
};
static const DitherScheme DITHER_2D = {
	.offs = { {1,0}, {0,1} },
	.fac  = { 1, 1 },
	.div  = 2,
	.count= 2
};

static uint8_t quantize(uint8_t pix, uint8_t bits) {
	switch (bits) {
		case 0:  return 255;
		case 1:  return (pix >> 7) * 255;
		case 2:  return (pix >> 6) * 255 / 3;
		case 26: return (pix / 51) * 51;
		case 27: return (pix * 6 / 255 * 85) >> 1;
		case 3:  return (pix >> 5) * 255 / 7;
		case 4:  return (pix >> 4) * 255 / 15;
		case 5:  return (pix >> 3) * 255 / 31;
		case 6:  return (pix >> 2) * 255 / 63;
		case 7:  return (pix >> 1) * 255 / 127;
		case 8:  return pix;
		default: return 255;
	}
}

int dither(uint8_t *img, int32_t w, int32_t h,
           uint8_t ditherType, uint8_t quantBits,
           uint8_t numOfChannels, uint8_t channel)
{
	if (!img || w <= 0 || h <= 0 || channel >= numOfChannels) return 0;

	size_t n = (size_t)w * (size_t)h;
	uint8_t *tmp = malloc(n);
	if (!tmp) { fprintf(stderr, "%s:%d malloc failed\n", __FILE__, __LINE__); return 0; }

	for (size_t i = 0; i < n; ++i) tmp[i] = img[i * numOfChannels + channel];

	const DitherScheme *scheme = &DITHER_NONE;
	if (ditherType == 1) scheme = &DITHER_FS;
	else if (ditherType == 2) scheme = &DITHER_2D;

	for (int32_t y = 0; y < h; ++y) {
		for (int32_t x = 0; x < w; ++x) {
			size_t i = (size_t)y * w + x;
			uint8_t pix = tmp[i];
			uint8_t pixq = quantize(pix, quantBits);
			int16_t err = pix - pixq;
			img[i * numOfChannels + channel] = pixq;

			for (uint8_t k = 0; k < scheme->count; ++k) {
				int16_t nx = x + scheme->offs[k][0];
				int16_t ny = y + scheme->offs[k][1];
				if ((unsigned)nx < (unsigned)w && (unsigned)ny < (unsigned)h) {
					size_t ni = (size_t)ny * w + nx;
					int32_t val = tmp[ni] + err * scheme->fac[k] / scheme->div;
					tmp[ni] = clampU8(val);
				}
			}
		}
	}

	free(tmp);
	return 1;
}
