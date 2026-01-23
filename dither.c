// dither - v0.10.0 - by vb :) 

#include "libs/stb_image.h"
#include "libs/stb_image_write.h"
#include "libs/qoi.h"
#include "libs/stb_image_resize2.h"
#include "libs/stb_sprintf.h"

#include "libs/vb_mm.h"
#include "libs/vb_flag_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define STR_ENDING(S, E) (strcmp(S + strlen(S) - (sizeof(E)-1),E) == 0)
#define BITVAL(X, N) (((X) >> (N)) & 1)
#define BITFLIP(X, N) (X^(1 << ((N) - 1)))

#define NODITHER 0
#define FLOYDDITHER 1
#define SIMPLEDITHER 2

#include "funcs/dither.h"
#include "funcs/rotate90.h"
#include "funcs/usage.h"

/*
stbir_resize( data, w, h, 0,
              outData, diffw, diffh, 0,
              STBIR_RGBA, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP,
              STBIR_FILTER_DEFAULT ); // STBIR_1CHANNEL
*/



int main(int argc, char **argv)
{
	if (argc < 2) {
		dith_usage(argv);
		return 1;
	}

	vb_setVbArgc(argc);
	vb_setVbArgv(argv);

	if (vb_checkArg("-h") || vb_checkArg("--help") || vb_checkArg("-help")) {
		dith_usage(argv);
		return 1;
	}

	vb_da_ptr_init();

	uint8_t dithType = FLOYDDITHER;
	uint8_t lumaOn = 0;
	uint8_t lumaType = 1;
	uint8_t lumaBits = 1;

	uint8_t redBits =   26;
	uint8_t greenBits = 27;
	uint8_t blueBits =  26;
	uint8_t alphaBits = 3;

	uint8_t forceOverwrite = 0;
	int8_t encoded = 0;
	int32_t p = 0;
	int8_t verboseOutput = 0;
	uint8_t autoOutputFormat = 'p';
	uint8_t shouldBeResized = 0;
	uint8_t shouldBeRotated = 0;

	char tmp[256] = "\0";

	int32_t w, h;
	int32_t new_w, new_h;
	RotateDirection rotation = ROT_CW;
	char *outName = NULL;
	uint8_t *data = NULL;
	uint8_t *yChannel = NULL;


	if (vb_checkArg("-v")) {
		printf("Toggled verbose output\n");
		verboseOutput = 1;
	}
	
	if (vb_checkArg("-y")) {
		if (verboseOutput)
			printf("Luma on\n");
		lumaOn = 1;
	}
	
	if (vb_checkArg("--force")) {
		if (verboseOutput)
			printf("Forced overwrite\n");
		forceOverwrite=1;
	}
	
	if ((p = vb_checkArgWithParams("-d", 1))) {
		if (verboseOutput)
			printf("Dither type: %s\n", argv[p+1]);
		if (!strcmp("no", argv[p+1])) dithType = 0; 
		if (!strcmp("floyd", argv[p+1])) dithType = 1; 
		if (!strcmp("2d", argv[p+1])) dithType = 2; 
	}
	
	if ((p = vb_checkArgWithParams("-f", 1))) {
		if (verboseOutput)
			printf("Luma type: %s\n", argv[p+1]);
		lumaType = atoi(argv[p+1]);
	}
	
	if ((p = vb_checkArgWithParams("-s", 2))) {
		if (verboseOutput)
			printf("Size: %s x %s\n", argv[p+1], argv[p+2]);

		new_w = atoi(argv[p+1]);
		new_h = atoi(argv[p+2]);
		shouldBeResized = 1;
	}

	if ((p = vb_checkArgWithParams("-r", 1))) {
		if (verboseOutput)
			printf("Rotate %s\n", argv[p+1]);

		shouldBeRotated = 1;
		if (!strcasecmp("ccw", argv[p+1])) {
			rotation = ROT_CCW;
		}
}
	
	if ((p = vb_checkArgWithParams("-R", 1))) {
		if (verboseOutput)
			printf("R: %s\n", argv[p+1]);
		redBits = (int)(atof(argv[p+1])*10)%10 == 0 ? atoi(argv[p+1]) : atof(argv[p+1])*10;
	}
	
	if ((p = vb_checkArgWithParams("-G", 1))) {
		if (verboseOutput)
			printf("G: %s\n", argv[p+1]);
		greenBits = (int)(atof(argv[p+1])*10)%10 == 0 ? atoi(argv[p+1]) : atof(argv[p+1])*10;
	}
	
	if ((p = vb_checkArgWithParams("-B", 1))) {
		if (verboseOutput)
			printf("B: %s\n", argv[p+1]);
		blueBits = (int)(atof(argv[p+1])*10)%10 == 0 ? atoi(argv[p+1]) : atof(argv[p+1])*10;
	}
	
	if ((p = vb_checkArgWithParams("-A", 1))) {
		if (verboseOutput)
			printf("A: %s\n", argv[p+1]);
		alphaBits = (int)(atof(argv[p+1])*10)%10 == 0 ? atoi(argv[p+1]) : atof(argv[p+1])*10;
	}
	
	if ((p = vb_checkArgWithParams("-Y", 1))) {
		if (verboseOutput)
			printf("Y: %s\n", argv[p+1]);
		lumaBits = (int)(atof(argv[p+1])*10)%10 == 0 ? atoi(argv[p+1]) : atof(argv[p+1])*10;
	}
	
	if ((p = vb_checkArgWithParams("-O", 1))) {
		if (verboseOutput)
			printf("Output extention: %s\n", argv[p+1]);
		if (!strcmp(argv[p+1], "qoi")) autoOutputFormat = 'q';
	}
	
	if ((p = vb_checkArgWithParams("-o", 1))) {
		if (verboseOutput)
			printf("Output filename: %s\n", argv[p+1]);
		outName = argv[p+1];
	}


	// giving filename and checking if file exists
	if (outName == NULL) {
		char* last_dot = strrchr(argv[1], '.');
		if (!last_dot) {
			printf("Error: Input file has no extension.\n");
			vb_da_ptr_destroy();
			return 1;
		}
		
		size_t base_len = last_dot - argv[1];
		
		size_t needed_len = base_len + 10;
		outName = vb_alloc(needed_len);
		if (!outName) {
			printf("Error: Memory allocation failed.\n");
			vb_da_ptr_destroy();
			return 1;
		}
		
		int written = snprintf(outName, needed_len, "%.*s%s", 
				(int)base_len, argv[1], autoOutputFormat == 'q' ? "-d.qoi" : "-d.png");
		
		if (written < 0 || (size_t)written >= needed_len) {
			printf("Error: Failed to construct output filename.\n");
			vb_da_ptr_destroy();
			return 1;
		}
	}

	if (!forceOverwrite) {
		FILE *fileExist;
		if ((fileExist = fopen(outName, "r"))) {
			fclose(fileExist);
			printf("File exist. Overwrite? (y/n): ");
			fgets(tmp, 256, stdin);
			tmp[strlen(tmp)-1] = '\0';

			if (strcmp(tmp, "y")) {
				vb_da_ptr_destroy();
				return 2;
			};
		}
	}


	if (STR_ENDING(argv[1],".png") || STR_ENDING(argv[1],".jpg")) {
		data = stbi_load(argv[1], &w, &h, NULL, 4);
		if (!data) {
			printf("Failed to load %s\n", argv[1]);
			return 1;
		}
	} else if (STR_ENDING(argv[1], ".qoi")) {
		qoi_desc desc;
		data = qoi_read(argv[1], &desc, 4);
		if (!data) {
			printf("Failed to load %s\n", argv[1]);
			return 1;
		}
		w = desc.width;
		h = desc.height;
	}

	if (!data) {
		printf("Wrong input format. Use png, jpg or qoi\n");
		return 2;
	}
	
// -------- resizing and rotating here --------

	if (shouldBeRotated) {
		uint8_t *rotatedata = 0;
		rotatedata = rotate_rgba90(data, rotatedata, w, h, rotation);
		if (!rotatedata) {
			printf("Fuckd up to alloc memory for u\n");
			vb_da_ptr_destroy();
			return 1;
		}
		
		// evil xor swap trick
		w ^= h;
		h ^= w;
		w ^= h;

		vb_free(data);
		data = rotatedata;
	}

	if (shouldBeResized) {
		if (!new_w) {
			printf("Incorrect width, used original instead\n");
			new_w = w;
		}

		if (!new_h) {
			printf("Incorrect height, used original instead\n");
			new_h = h;
		}

		if (new_w < 0 && new_h < 0) {
			printf("Dont know what resolution to use? oklol, original used instead\n");
			new_w = w;
			new_h = h;
		}

		if (new_w < 0 && new_h > 0) {
			printf("calc w\n");
			new_w = (new_h * w) / h;
		}

		if (new_h < 0 && new_w > 0) {
			printf("calc h\n");
			new_h = (new_w * h) / w;
		}

		uint8_t *resizedata =  0;
		resizedata = stbir_resize_uint8_srgb(data, w, h, 0, resizedata, new_w, new_h, 0, 4);
		if (!resizedata) {
			printf("Fuckd up to alloc memory for u\n");
			vb_da_ptr_destroy();
			return 1;
		}

		w = new_w;
		h = new_h;

		vb_free(data);
		data = resizedata;
	}

// ---------------------------------------------

	if (lumaOn) {
		yChannel = vb_alloc(w * h);
		for (int i = 0; i < w*h; i++) {
			switch (lumaType) {
				case 1:
					yChannel[i] = (19595*data[i*4] + 38470*data[i*4+1] + 7471*data[i*4+2] + 32768) >> 16;
					break;
				case 2: default:
					yChannel[i] = (13933*data[i*4] + 46871*data[i*4+1] + 4732*data[i*4+2] + 32768) >> 16;
					break;
				case 3:
					yChannel[i] = (17216*data[i*4] + 44434*data[i*4+1] + 3886*data[i*4+2] + 32768) >> 16;
			}
		}
	}



	if (lumaOn) {
		if (!dither(yChannel, w, h, dithType, lumaBits, 1, 0) ||
			!dither(data, w, h, dithType, alphaBits, 4, 3)
		){ vb_da_ptr_destroy(); return 1; }
	} else {
		if (!dither(data, w, h, dithType,   redBits, 4, 0) ||
			!dither(data, w, h, dithType, greenBits, 4, 1) ||
			!dither(data, w, h, dithType,  blueBits, 4, 2) ||
			!dither(data, w, h, dithType, alphaBits, 4, 3)
		){ vb_da_ptr_destroy(); return 1; }
	}

	if (lumaOn) {
		for (int64_t i = 0; i < w*h; i++) {
			data[i*4 + 0] = yChannel[i];
			data[i*4 + 1] = yChannel[i];
			data[i*4 + 2] = yChannel[i];
		}
	}

	for (int i = 0; i < w*h; i++) {
		if (!data[i*4 + 3]) {
			data[i * 4 + 0] = 0;
			data[i * 4 + 1] = 0;
			data[i * 4 + 2] = 0;
			data[i * 4 + 3] = 0;
		}
	}



	if (STR_ENDING(outName, ".png")) {
		encoded = stbi_write_png(outName, w, h, 4, data, w * 4);
	} else if (STR_ENDING(outName, ".qoi")) {
		encoded = qoi_write(outName, data, &(qoi_desc){ w, h, 4, QOI_SRGB });
	}

	if (!encoded) {
		printf("Failed to write %s\n", outName);
		vb_da_ptr_destroy();
		return 2;
	}

	vb_da_ptr_destroy();

	return 0;
}
