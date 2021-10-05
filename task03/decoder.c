#include <sys/stat.h>
#include <stdio.h>

int decode(FILE *in, FILE *out)
{
	char c;
	unsigned char count;
	int ret = 0;
	while (fread(&count, 1, 1, in) == 1) {
		if (fread(&c, 1, 1, in) != 1) {
			ret = 1;
			break;
		}
		while (count--) {
			fwrite(&c, 1, 1, out);
		}
	}
	return ret;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("decoder - decodes provided file\nUsage: decoder ENCODED DECODED\n");
		return 1;
	}
	FILE *in = NULL;
	FILE *out = NULL;
	int ret = 1;
	do {
		in = fopen(argv[1], "rb");
		if (!in) {
			printf("Failed to open encoded file %s\n", argv[1]);
			break;
		}
		struct stat st;
		if (stat(argv[1], &st) || st.st_size < 2) {
			printf("Not enough data in encoded file %s\n", argv[1]);
			break;
		}

		out = fopen(argv[2], "wb");
		if (!out) {
			printf("Failed to open file for decoding %s\n",
			       argv[2]);
			break;
		}
		ret = decode(in, out);
	} while (0);
	if (in) {
		fclose(in);
	}
	if (out) {
		fclose(out);
	}

	return ret;
}