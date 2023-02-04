/*
 * Written by Jim Rees, University of Michigan, Aug 1992
 */

#include <stdio.h>

main(ac, av)
int ac;
char *av[];
{
	char stdoutbuf[BUFSIZ];
	int c;

	setbuf(stdout, stdoutbuf);

	printf("P4\n");
	printf("# produced by logotopbm\n");
	printf("512 160\n");

	while ((c = getchar()) != EOF)
		putchar(c ^ 0xff);

	exit(0);
}
