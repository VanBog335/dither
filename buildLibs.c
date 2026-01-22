#define VB_MM_IMPLEMENTATION
#include "libs/vb_mm.h"
#define VB_FLAG_PARSER_IMPLEMENTATION
#include "libs/vb_flag_parser.h"

#define STBI_MALLOC(sz)           vb_alloc(sz)
#define STBI_REALLOC(p,newsz)     vb_realloc(p,newsz)
#define STBI_FREE(p)              vb_free(p)

#define STBIR_MALLOC(size,user_data) ((void)(user_data), vb_alloc(size))
#define STBIR_FREE(ptr,user_data)    ((void)(user_data), vb_free(ptr))

#define QOI_MALLOC(sz) vb_alloc(sz)
#define QOI_FREE(p)    vb_free(p)

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"
#define STB_SPRINTF_IMPLEMENTATION
#include "libs/stb_sprintf.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "libs/stb_image_resize2.h"
#define QOI_IMPLEMENTATION
#include "libs/qoi.h"