/*
	vanbog_flag_parser -- v0.0.0  -- https://github.com/VanBog335
*/

#ifndef VB_FLAG_PARSER_H_
#define VB_FLAG_PARSER_H_

#include <string.h>

void vb_setVbArgc(int aargc);
void vb_setVbArgv(char **aargv);
int vb_checkArgWithParams(const char *check, int num_params);
int vb_checkArg(const char *check);

#endif /* VB_FLAG_PARSER_H_ */


#ifdef VB_FLAG_PARSER_IMPLEMENTATION

int vb_argc;
char **vb_argv;

void vb_setVbArgc(int aargc) {
	vb_argc = aargc;
}

void vb_setVbArgv(char **aargv) {
	vb_argv = aargv;
}

int vb_checkArgWithParams(const char *check, int num_params)
{
	for (int i = 1; i < vb_argc - num_params; i++) {
		if (!strcmp(check, vb_argv[i]))
			return i;
	}

	return 0;
}

int vb_checkArg(const char *check)
{
	return vb_checkArgWithParams(check, 0);
}

#endif /* VB_FLAG_PARSER_IMPLEMENTATION */