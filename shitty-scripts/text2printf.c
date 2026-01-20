#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE *file = fopen(argv[1], "rb");
	if (!file) {
		perror("File opening error");
		return 1;
	}

	printf("printf(\"");

	int ch;
	while ((ch = fgetc(file)) != EOF) {
		unsigned char uc = ch;

		switch (uc) {
			case '\n':
				printf("\\n");
				break;
			case '\t':
				printf("\\t");
				break;
			case '\r':
				printf("\\r");
				break;
			case '\"':
				printf("\\\"");
				break;
			case '\\':
				printf("\\\\");
				break;
			case '%':
				printf("%%%%");
				break;
			default:
				if (uc >= 32 && uc <= 126) {
					putchar(uc);
				} else {
					printf("\\x%02X", uc);
				}
		}
	}

	printf("\\n\");\n");

	fclose(file);
}