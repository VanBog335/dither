void dith_usage(char **argv)
{
	printf("Usage: %s input [flags]\n\nInput formats: png, jpg, qoi\nOutput formats: png, qoi\n\n-v\n\tverbose output\n\t[default: false]\n\n-y\n\tluminance only\n\t[default: false]\n\n-d <no/floyd/2d>\n\tdither type\n\t[default: floyd]\n\n--force\n\tforce overwrite\n\t[default: false]\n\n-<R/G/B/A/Y> <0/1/2/2.6/2.7/3-8>\n\tbits per channel\n\t[default: 2.6 2.7 2.6 3 1]\n\n-f <1-3>\n\tluma conversion formula\n\n-o <filename>\n\toutput file\n\t[default: path/to/input-d.png]\n\n-s <num> <num> (WIP)\n\tsize of image\n\n-h/--help/-help\n\tthis help\n", *argv);

}