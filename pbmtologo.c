/*
 * Written by Jim Rees, University of Michigan, Aug 1992
 */

#include <stdio.h>
#include <stdlib.h>

void cpascii();
void cpraw();

int
main(int ac, char** av)
{
	char buf[80], stdoutbuf[BUFSIZ], ptype;
	int width, height;

	setbuf(stdout, stdoutbuf);

	if (fgets(buf, sizeof buf, stdin) == NULL)
		exit(1);
	if (buf[0] != 'P') {
		fprintf(stderr, "input not a pbm file\n");
		exit(1);
	}
	ptype = buf[1];

	buf[0] = '#';
	while (buf[0] == '#')
		if (fgets(buf, sizeof buf, stdin) == NULL)
			exit(1);

	sscanf(buf, "%d %d", &width, &height);
	if (width != 512 || height != 160) {
		fprintf(stderr, "expected 512x160, got %dx%d\n", width, height);
		exit(1);
	}

	if (ptype == '1')
		cpascii();
	else if (ptype == '4')
		cpraw();
	else {
		fprintf(stderr, "unknown pbm file type '%c'\n", ptype);
		exit(1);
	}

	exit(0);
}

void
cpascii()
{
	int px, n = 0, bit;

	while (scanf("%d", &bit) == 1) {
		if (n % 8 == 0) {
			px = 0;
			n = 0;
		}
		if (bit)
			px |= 1 << (7 - n);
		if (n++ % 8 == 7)
			putchar(px ^ 0xff);
	}
}

void
cpraw()
{
	int c;

	while ((c = getchar()) != EOF)
		putchar(c ^ 0xff);
}
