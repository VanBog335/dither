#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);
	Nob_Cmd cmd = {0};

	if (argc < 2) {
		nob_cc(&cmd);
		nob_cmd_append(&cmd, "-Wall", "-Wextra", "-lm");
		
		nob_cmd_append(&cmd, "-O2", "-s");
		
		nob_cc_output(&cmd, "dither");
		nob_cc_inputs(&cmd, "dither.c");
		#if defined(_WIN32)
		  nob_cc_inputs(&cmd, "sq-win.o");
		#elif defined(__ANDROID__)
		  nob_cc_inputs(&cmd, "sq-andro.o");
		#else
		  nob_cc_inputs(&cmd, "sq-custom.o");
		#endif
	} else if (!strcmp(argv[1], "clib")) {
		nob_log(NOB_INFO, "Compiling lib");
		nob_cc(&cmd);
		nob_cmd_append(&cmd, "-Wall", "-Wextra", "-lm", "-O3", "-s", "-c");
		nob_cc_inputs(&cmd, "buildLibs.c");
		#if defined(_WIN32)
		  nob_cc_output(&cmd, "sq-win.o");
		#elif defined(__ANDROID__)
		  nob_cc_output(&cmd, "sq-andro.o");
		#else
		  nob_cc_output(&cmd, "sq-custom.o");
		#endif
	}
	if (!nob_cmd_run(&cmd)){
		nob_log(NOB_INFO, "Compile lib plz: ./nob clib");
		return 1;
	} 
}