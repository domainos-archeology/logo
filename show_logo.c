/*
 * show_logo
 *
 * show apollo_logo on your screen
 *
 * Jim Rees, April 1984
 */

#include "/sys/ins/base.ins.c"
#include "/sys/ins/gpr.ins.c"

#include <stdio.h>

#define LOGOSIZE 10240
#define XSIZ	512
#define YINC	32
#define PperB	8

int iflag;
int xmax, ymax;
int nplotted;

gpr_$pixel_value_t map[YINC][XSIZ];

/* Keyset of [' ' .. '~'] */
gpr_$keyset_t keyset = { 0, 0, 0, 0, 0x7fffffff, 0xffffffff, 0xffffffff, 0 };

extern char *malloc();

main(ac, av)
int ac;
char *av[];
{
	int i;
	gpr_$window_t window;
	gpr_$position_t pos;
	gpr_$event_t type;
	char data;
	short count;
	status_$t st;

	while (ac > 1 && av[1][0] == '-') {
		switch (av[1][1]) {
		case 'i':
			iflag = 1;
			break;
		}
		ac--;
		av++;
	}

	if (ac >= 2)
		for (i = 1; i < ac; i++)
			show(av[i], &window);
	else
		show("/sys/apollo_logo", &window);

	if (nplotted > 0) {
		gpr_$acquire_display(st);
		check("gpr_$acquire_display", st);
		gpr_$event_wait(type, data, pos, st);
		check("gpr_$event_wait", st);
		gpr_$force_release(count, st);
		check("gpr_$force_release", st);
		gpr_$terminate(false, st);
		check("gpr_$terminate", st);
	}
	exit(0);
}

init_display(wp)
gpr_$window_t *wp;
{
	gpr_$offset_t size;
	gpr_$bitmap_desc_t bitmap;
	short plane;
	status_$t st;

	size.x_size = 1280;
	size.y_size = 1024;
	gpr_$init(gpr_$direct, stream_$errout, size, 0, bitmap, st);
	check("gpr_$init", st);

	gpr_$enable_input(gpr_$keystroke, keyset, st);
	check("gpr_$enable_input", st);

	gpr_$set_auto_refresh(true, st);
	check("gpr_$set_auto_refresh", st);

	gpr_$inq_bitmap_dimensions(bitmap, size, plane, st);
	check("gpr_$inq_bitmap_dimensions", st);
	xmax = size.x_size;
	ymax = size.y_size;

	wp->window_base.x_coord = 0;
	wp->window_size.x_size = XSIZ;
	wp->window_base.y_coord = 0;
	wp->window_size.y_size = YINC;
}

show(filename, wp)
char *filename;
gpr_$window_t *wp;
{
	static char *logo;
	short y, ysiz;
	long size_mapped;
	int fd;
	status_$t st;

	if (logo == NULL)
		logo = malloc(LOGOSIZE);

	if ((fd = open(filename, 0)) < 0) {
		perror(filename);
		return;
	}

	if ((size_mapped = read(fd, logo, LOGOSIZE)) <= 0) {
		perror(filename);
		return;
	}
	close(fd);

	ysiz = size_mapped / XSIZ * PperB;

	for (y = 0; y < ysiz; y++) {
		doscan(&logo[(y * XSIZ) / PperB], &map[y % YINC][0]);
		if (y % YINC == YINC - 1) {
			if (!nplotted)
				init_display(wp);
			gpr_$acquire_display(st);
			check("gpr_$acquire_display", st);
			gpr_$write_pixels(map, *wp, st);
			check("gpr_$write_pixels", st);
			nplotted++;
			gpr_$release_display(st);
			wp->window_base.y_coord += YINC;
		}
	}

	if (wp->window_base.y_coord >= ymax - 160) {
		wp->window_base.x_coord += XSIZ;
		wp->window_base.y_coord = 0;
	}
}

doscan(src, dest)
char src[];
long dest[];
{
	register short x, b;

	for (x = 0; x < XSIZ; x++) {
		if ((x & (PperB - 1)) == 0)
			b = 0x80;
		else
			b >>= 1;
		if (iflag)
			dest[x] = (src[x / PperB] & b) ? gpr_$white : gpr_$black;
		else
			dest[x] = (src[x / PperB] & b) ? gpr_$black : gpr_$white;
	}
}

check(s, st)
char *s;
status_$t st;
{
	status_$t xst;

	if (st.all != status_$ok) {
		gpr_$terminate(false, xst);
		ErrPrint(s, st);
		exit(1);
	}
}
