#include <stdio.h>
#include <sys/stat.h>

static int encode(FILE *in, FILE *out)
{
	static const unsigned char MAX_COUNT = 0xFF;
	unsigned char count = 1;
	char p;
	char c;
	fread(&p, 1, 1, in);
	while (fread(&c, 1, 1, in) == 1) {
		if (c == p && count < MAX_COUNT) {
			++count;
			continue;
		}
		fwrite(&count, 1, 1, out);
		fwrite(&p, 1, 1, out);
		p = c;
		count = 1;
	}
	fwrite(&count, 1, 1, out);
	fwrite(&p, 1, 1, out);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("encoder - encodes provided file with RLE\nUsage: encoder FILE ENCODED\n");
		return 1;
	}
	FILE *in = NULL;
	FILE *out = NULL;
	int ret = 1;
	do {
		in = fopen(argv[1], "rb");
		if (!in) {
			printf("Failed to open file for encoding %s\n",
			       argv[1]);
			break;
		}
		struct stat st;
		if (stat(argv[1], &st) || !st.st_size) {
			printf("Cannot encode empty file %s\n", argv[1]);
			break;
		}
		out = fopen(argv[2], "wb");
		if (!out) {
			printf("Failed to open output file %s\n", argv[2]);
			break;
		}

		ret = encode(in, out);
	} while (0);
	if (in) {
		fclose(in);
	}
	if (out) {
		fclose(out);
	}

	return ret;
}