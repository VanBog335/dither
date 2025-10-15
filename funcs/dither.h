int dither(uint8_t *img, int32_t w, int32_t h,
		   uint8_t ditherType, uint8_t quantBits,
		   uint8_t numOfChannels, uint8_t channel)
{
	uint8_t *tmpImg = malloc(w * h * sizeof(uint8_t));

	if (!tmpImg) {
		printf("%s:%d Failed to malloc()", __FILE__, __LINE__);
		return 0;
	}

	for (int32_t i = 0; i < w * h; i++)
		tmpImg[i] = img[i*numOfChannels + channel];

	// Floyd-Steinberg
	int8_t floydOffsets[4][2] = { {1,0},{-1,1},{0,1},{1,1} };
	int8_t floydFactors[4] = { 7, 3, 5, 1 };

	// Two-dimensional
	int8_t twoD_Offsets[2][2] = { {1,0},{0,1} };
	int8_t twoD_Factors[2] = { 1, 1 };

	uint8_t divider;
	uint8_t numOfIters;
	int8_t offsets[4][2];
	int8_t factors[4];

	switch (ditherType) {
		case 0: // no dither
			numOfIters = 0;
			break;
		case 1: // Floyd-Steinberg
			default:
		memcpy(offsets, floydOffsets, sizeof(floydOffsets));
		memcpy(factors, floydFactors, sizeof(floydFactors));
			divider = 16; numOfIters = 4;
			break;
		case 2: // 2d
		memcpy(offsets, twoD_Offsets, sizeof(twoD_Offsets));
		memcpy(factors, twoD_Factors, sizeof(twoD_Factors));
			divider = 2; numOfIters = 2;
	}

	for (uint16_t y = 0; y < h; y++) for (uint16_t x = 0; x < w; x++) {
	uint32_t i = (y * w + x);

	uint8_t pix = tmpImg[i];
	uint8_t pixx = 0;
	switch(quantBits) {
		case 0:  pixx = 0;
			break;
		case 1:  pixx = (pix>>7)*255;
			break;
		case 2:  pixx = (pix>>6)*255/3;
			break;
		case 26: pixx = pix/51*51;
			break;
		case 27: pixx = (pix*6/255*85)>>1;
			break;
		case 3:  pixx = (pix>>5)*255/7;
			break;
		case 4:  pixx = (pix>>4)*255/15;
			break;
		case 5:  pixx = (pix>>3)*255/31;
			break;
		case 6:  pixx = (pix>>2)*255/63;
			break;
		case 7:  pixx = (pix>>1)*255/127;
			break;
		case 8:  pixx = pix;
			break;
		default: pixx = 255;
	}
	int16_t err = pix - pixx;

	img[i * numOfChannels + channel] = pixx;

	for (uint8_t k = 0; k < numOfIters; k++) {
		int16_t nx = x + offsets[k][0];
		int16_t ny = y + offsets[k][1];
		if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
			int32_t ni = ny * w + nx;
			tmpImg[ni] = min(255,
				max(0, tmpImg[ni] + err*factors[k]/divider)
			);
		}
	}

	}

	free(tmpImg);
	return 1;
}