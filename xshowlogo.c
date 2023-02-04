/*
 * Show apollo_logo, using X
 *
 * Written by Jim Rees, University of Michigan, Sept 1992
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define W 512
#define H 160
#define IBSZ (W * H / 8)

static char progname[] = "xshowlogo";
static char *dfltgeom = "512x160+8+8";
int iflag = 1;

extern char *getenv(), *malloc();

main(ac, av)
int ac;
char *av[];
{
	char *displayname = NULL;
	char *geom = dfltgeom;
	int i, an;
	Display *display;
	Window window;
	GC gc;
	XEvent ev;
	char buf[80];
	XComposeStatus compstatus;

	while (ac > 1 && av[1][0] == '-') {
		switch (av[1][1]) {
		case 'd':
			displayname = av[2];
			ac--;
			av++;
			break;
		case 'g':
			geom = av[2];
			ac--;
			av++;
			break;
		}
		ac--;
		av++;
	}

	openX(progname, displayname, dfltgeom, geom, NULL, NULL, &gc, &display, &window);

	XSelectInput(display, window, (ExposureMask | KeyPressMask | ButtonPressMask));
	XMapWindow(display, window);

	an = 1;

	while(1) {
		XNextEvent(display, &ev);
		switch (ev.type) {
		case Expose:
			if (ev.xexpose.count == 0) {
				while (XCheckTypedEvent(display, Expose, &ev))
					;
			}
			break;
		case KeyPress:
			i = XLookupString((XKeyEvent *) &ev, buf, sizeof buf, NULL, &compstatus);
			buf[i] = '\0';
			if (buf[0] == 'q')
				goto getout;
			an++;
			if (an >= ac)
				goto getout;
			break;
		}
		dofile(display, window, gc, av[an], an);
	}

getout:
	XCloseDisplay(display);

	exit(0);
}

static
dofile(display, window, gc, fname, an)
Display *display;
Window window;
GC gc;
char *fname;
int an;
{
	static int pan = -1;
	static XImage *image;

	if (an != pan) {
		if (getimage(display, fname, &image) < 0)
			return -1;
		pan = an;
		XClearWindow(display, window);
	}
	XPutImage(display, window, gc, image, 0, 0, 0, 0, W, H);

	return 0;
}

static
getimage(display, fname, ip)
Display *display;
char *fname;
XImage **ip;
{
	int fd;
	static char *buf;
	static XImage *image;

	if (buf == NULL)
		buf = malloc(IBSZ);

	if ((fd = open(fname, 0)) < 0)
		return -1;
	if (read(fd, buf, IBSZ) != IBSZ) {
		close(fd);
		return -1;
	}
	close(fd);

	if (image != NULL) {
		XDestroyImage(image);
		image = NULL;
	}
	image = XCreateImage(display, DefaultVisual(display, DefaultScreen(display)), 1, XYBitmap, 0, buf, W, H, 32, 64);
	*ip = image;

	return 0;
}
