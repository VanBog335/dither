// dither - v1.0.0-a_02 - by vb :) 

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "qoi.h"
#include "stb/stb_image_resize2.h"
#include "stb/stb_sprintf.h"

#include "vb_mm.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <getopt.h>

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define STR_ENDING(S, E) (strcmp(S + strlen(S) - (sizeof(E)-1),E) == 0)
#define BITVAL(X, N) (((X) >> (N)) & 1)
#define BITFLIP(X, N) (X^(1 << ((N) - 1)))

#define NODITHER 0
#define FLOYDDITHER 1
#define SIMPLEDITHER 2

#include "funcs/dither.h"
#include "funcs/parser-size.h"
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

	if (**(argv+1) == '-') {
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
	int32_t parsedArgGetOpt = 0;

	char tmp[256] = "\0";

	int32_t w, h;
	char *outName = NULL;
	uint8_t *data = NULL;
	uint8_t *yChannel = NULL;

	while ( (parsedArgGetOpt = getopt(argc-1, argv+1, "yFd:f:s:R:G:B:A:Y:o:")) != -1){
		switch (parsedArgGetOpt) {
			case 'y':
				printf("Luma on\n");
				lumaOn=1;
				break;
			case 'F':
				printf("Forced overwrite\n");
				forceOverwrite=1;
				break;
			case 'd':
				printf("Dither type: %s\n", optarg);
				if (!strcmp("no", optarg)) dithType = 0; 
				if (!strcmp("floyd", optarg)) dithType = 1; 
				if (!strcmp("2d", optarg)) dithType = 2; 
				break;
			case 'f':
				printf("Luma type: %s\n", optarg);
				lumaType = atoi(optarg);
				break;
			case 's':
				printf("Size: %s\n", optarg);
				break;
			case 'R':
				printf("R: %s\n", optarg);
				redBits = (int)(atof(optarg)*10)%10 == 0 ? atoi(optarg) : atof(optarg)*10;
				break;
			case 'G':
				printf("G: %s\n", optarg);
				greenBits = (int)(atof(optarg)*10)%10 == 0 ? atoi(optarg) : atof(optarg)*10;
				break;
			case 'B':
				printf("B: %s\n", optarg);
				blueBits = (int)(atof(optarg)*10)%10 == 0 ? atoi(optarg) : atof(optarg)*10;
				break;
			case 'A':
				printf("A: %s\n", optarg);
				alphaBits = (int)(atof(optarg)*10)%10 == 0 ? atoi(optarg) : atof(optarg)*10;
				break;
			case 'Y':
				printf("Y: %s\n", optarg);
				lumaBits = (int)(atof(optarg)*10)%10 == 0 ? atoi(optarg) : atof(optarg)*10;
				break;
			case 'o':
				printf("Output filename: %s\n", optarg);
				outName = optarg;
				break;
			case '?':
				printf("Error found !\n");
				dith_usage(argv);
				vb_da_ptr_destroy();
				return 1;
				break;
		}
	}

	// giving filename and checking if file exists
	if (outName == NULL) { 
		int16_t len = strlen(argv[1]);
		if (len < 5) {
			printf("Error: filename is too short.\n");
			vb_da_ptr_destroy();
			return 1;
		}

		outName = vb_alloc(len + 10);
		if (!outName) {
			printf("vb_alloc() failed.\n");
			vb_da_ptr_destroy();
			return 1;
		}

		strcpy(outName, argv[1]);
		strcpy(outName + len - 4, "-d.png");
		//strcpy(outName + len - 4, "-d.qoi");
	}

	if (!forceOverwrite) {
		FILE *fileExist;
		if ((fileExist = fopen(outName, "r"))) {
			fclose(fileExist);
			printf("File exist. Overwrite? (y/n): ");
			fgets(tmp, 256, stdin);
			tmp[strlen(tmp)-1] = '\0';

			if (strcmp(tmp, "y")) {
				puts("false");
				vb_da_ptr_destroy();
				return 2;
			} else puts("true");
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
	free(data);

	return 0;
}
