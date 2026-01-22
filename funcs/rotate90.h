typedef enum {
	ROT_CW,
	ROT_CCW
} RotateDirection;

uint8_t *rotate_rgba90(const uint8_t *in, uint8_t *out, size_t width, size_t height, RotateDirection dir) {
	if (!in || width == 0 || height == 0) return NULL;

	size_t bytes = width * height * 4;
	
	if (!out) {
		out = (uint8_t *)vb_alloc(bytes);
		if (!out) return NULL;
	}

	if (dir == ROTATE_CW) {
		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				size_t in_idx = (y * width + x) * 4;
				size_t out_idx = (x * height + (height - 1 - y)) * 4;
				memcpy(&out[out_idx], &in[in_idx], 4);
			}
		}
	} else {
		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				size_t in_idx = (y * width + x) * 4;
				size_t out_idx = ((width - 1 - x) * height + y) * 4;
				memcpy(&out[out_idx], &in[in_idx], 4);
			}
		}
	}

	return out;
}