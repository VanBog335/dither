void dith_usage(char **argv)
{
	printf("Usage: %s input [flags]\n\n", *argv);

	printf("Input formats: png, jpg, qoi\n");
	printf("Output formats: png, qoi\n\n");

	printf("-y\n");
	printf("\tluminance only\n");
	printf("\t[default: false]\n\n");

	printf("-d <no/floyd/2d>\n");
	printf("\tdither type\n");
	printf("\t[default: floyd]\n\n");

	printf("-F\n");
	printf("\tforce overwrite\n");
	printf("\t[default: false]\n\n");

	printf("-<R/G/B/A/Y> <1/2/2.6/2.7/3-8>\n");
	printf("\tbits per channel\n");
	printf("\t[default: 2.6 2.7 2.6 3 1]\n\n");

	printf("-f <1-3>\n");
	printf("\tluma conversion formula\n\n");

	printf("-o <filename>\n");
	printf("\toutput file\n");
	printf("\t[default: path/to/input-d.png]\n\n");

	printf("-s <num>x<num>\n");
	printf("\tsize of image\n");
}