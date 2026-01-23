#define NOB_IMPLEMENTATION
#include "nob.h"
#define VB_FLAG_PARSER_IMPLEMENTATION
#include "libs/vb_flag_parser.h"

int main(int argc, char **argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);
	Nob_Cmd cmd = {0};
	
	vb_setVbArgc(argc);
	vb_setVbArgv(argv);
	
	if (vb_checkArg("-h") || vb_checkArg("--help")) {
		printf("-release        compiles release version in ./build\r\n-clib           compiles libs in ./obj/libs-platform.o\n");
		return 0;
	}
	
	if (vb_checkArg("-clib")) {
		nob_log(NOB_INFO, "Compiling lib");
		nob_cc(&cmd);
		nob_cmd_append(&cmd, "-Wall", "-Wextra", "-O3", "-s", "-c");
		nob_cc_inputs(&cmd, "buildLibs.c");
		#if defined(_WIN32)
		  nob_cc_output(&cmd, "obj/libs-win.o");
		#elif defined(__ANDROID__)
		  nob_cc_output(&cmd, "obj/libs-and.o");
		#else
		  nob_cc_output(&cmd, "obj/libs-cus.o");
		#endif
	} else {
		nob_cc(&cmd);
		nob_cmd_append(&cmd, "-Wall", "-Wextra", "-lm");

		nob_cmd_append(&cmd, "-O2", "-s");
		#ifndef __ANDROID__
		  nob_cmd_append(&cmd, "-static");
		  //nob_cmd_append(&cmd, "-static-libgcc");
		#endif
		
		if (vb_checkArg("-release"))
			nob_cc_output(&cmd, "build/dither");
		else
			nob_cc_output(&cmd, "dither");
		
		nob_cc_inputs(&cmd, "dither.c");
		#if defined(_WIN32)
		  nob_cc_inputs(&cmd, "obj/libs-win.o");
		#elif defined(__ANDROID__)
		  nob_cc_inputs(&cmd, "obj/libs-and.o");
		#else
		  nob_cc_inputs(&cmd, "obj/libs-cus.o");
		#endif
	}

	if (!nob_cmd_run(&cmd)){
		nob_log(NOB_INFO, "Compile lib plz: ./nob -clib");
		return 1;
	} 
}
