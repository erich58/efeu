#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <EFEU/oldpixmap.h>

static void useourowncmap(OldPixMap_t *im);
static void setcolormap(OldPixMap_t *im);
static void makewindow(char *geomstring);

#ifndef	rindex
#define	rindex	strrchr
#endif

#define	CMAPSIZE	256
#define EV_MASK         KeyPressMask|ButtonReleaseMask|ExposureMask|ButtonPressMask

static char		title[BUFSIZ], *myname;
static Display		*display;
static Visual		*visual;
static int		theScreen, depth;
static Colormap		cmap;
static Window		winder, rootw;
static GC		theGC;
static XImage		*ximage;
static unsigned char	carray[CMAPSIZE];
static unsigned int	w, h;
static int		forcecmap;
static int		verbose;


static void (*copyfun)(char *from, char *to, int size);

static void xbcopy(char *from, char *to, int size)
{
	while (size--)
		*to++ = carray[(unsigned char) *from++];
}

static void mybcopy(char *from, char *to, int size)
{
	while (size--)
		*to++ = *from++;
}

/*
 * program to read in a color-mapped image and display it on X11, greyscale
 * is used if the image is rgb.
 */
int main(int ac, char **av)
{
	char		*mem, *mmem, *data;
	register int	i, j;
	unsigned int	w2, bytes_per_line;
	int		x, y, val, upsidedown, wreal, mag = 1;
	unsigned char	*line;
	OldPixMap_t	*im;
	XEvent		event;
	XVisualInfo	visualinfo;
	char		*geomstring, *displaystring;

        if ((myname = rindex(av[0],'/')) == NULL)
                myname = av[0];
       
       	else
                myname++;

	if (ac < 2)
	{
		fprintf(stderr, "%s: usage %s [-m<n>] [-u] [-f] [-geom geometry] [-display displayname] file.\n", myname, myname);
		fprintf(stderr, "	-m<n> means magnify by replicating pixels <n> times.\n");
		fprintf(stderr, "	-u means display upsizedown.\n");
		fprintf(stderr, "	-f means force creation of a new colourmap.\n");
		fprintf(stderr, "	-v means be verbose about some things.\n");
		fprintf(stderr, "	-geom (or -g or -geometry) means specify X geometry.\n");
		fprintf(stderr, "	-display means specify X display.\n");
		exit(EXIT_FAILURE);
	}

	geomstring = displaystring = (char *)NULL;
;
        verbose = forcecmap = upsidedown = 0;
        for (i = 1; i < ac && *av[i] == '-' && av[i][1] != 0; i++)
	{
                if (strncmp(av[i], "-m", 2) == 0)
		{             /* magnify */
                        if (sscanf(av[i] , "-m%d", &mag) != 1)
			{
                                fprintf(stderr, "%s: bad mag factor\n", myname);
                                exit(1);
                        }

			if (verbose)
				fprintf(stderr, "Magnify by %d.\n", mag);

                        if (mag < 1 || mag > 255)
			{
                                fprintf(stderr, "%s: bad mag factor\n", myname);
                                exit(EXIT_FAILURE);
                        }
                }
		else if (strncmp(av[i], "-u", 2) == 0)
		{              /* show
image upside down */
                        upsidedown = 1;
			if (verbose)
				fprintf(stderr, "Display upsidedown.\n");
                }
		else if (strcmp(av[i], "-g") == 0 ||
			   strcmp(av[i], "-geom") == 0 ||
			   strcmp(av[i], "-geometry") == 0)
			   {
				geomstring = av[++i];

			if (verbose)
				fprintf(stderr, "X geometry (from command line): %s.\n", geomstring);

                }
		else if (strcmp(av[i], "-display") == 0)
		{
			displaystring = av[++i];

			if (verbose)
				fprintf(stderr, "Display on: %s.\n", displaystring);
                }
		else if (strcmp(av[i], "-f") == 0)
		{
			forcecmap = 1;
			if (verbose)
				fprintf(stderr, "Force new colourmap.\n");
                }
		else if (strcmp(av[i], "-v") == 0)
		{
			verbose = 1;
		}
		else
		{
			fprintf(stderr, "Unknown option: '%s'.\n", av[i]);
		}
        }

/*	Pixelfile laden
*/
	if	(strcmp(av[i], "-") == 0)
	{
		im = load_OldPixMap(NULL);
	}
	else	im = load_OldPixMap(av[i]);

	if	(im == NULL)
	{
		fprintf(stderr, "disp: can't open file %s.\n", av[i]);
		exit(EXIT_FAILURE);
	}

	strcpy(title, myname);
	strcat(title, ": ");
	strcpy(title, av[i]);

	if (!displaystring)
		displaystring = XDisplayName((char *)NULL);

	if ((display = XOpenDisplay(displaystring)) == NULL)
	{
		fprintf(stderr, "Can't open display %s.\n", displaystring);
		exit(EXIT_FAILURE);
	}

	/*
	 * See if there is anything in .Xdefaults for us...
	 */
	if (!geomstring)
		geomstring = XGetDefault(display, "vort", "Geometry");

	if (geomstring && verbose)
		fprintf(stderr, "X geometry (from .Xdefaults): %s.\n", geomstring);

	theScreen = DefaultScreen(display);
	if ((depth = XDisplayPlanes(display, theScreen)) < 8)
	{
		fprintf(stderr, "You need a 256 (or more) colour display device\n");
		/*exit(1);*/
	}

	i = XMatchVisualInfo(display, theScreen, depth, PseudoColor, &visualinfo);
	if (!i)
	{
		fprintf(stderr, "Need a PseudoColor visual type.\n");
		exit(EXIT_FAILURE);
	}

	visual = visualinfo.visual;
	
	rootw = RootWindow(display, theScreen);
	theGC = DefaultGC(display, theScreen);

        wreal = im->cols;
        w = wreal * mag;
        h = mag * im->rows;

	/*
	 * Make it all 32 bit aligned
	 */
        w2 = w;
        if ((i = w % 4) != 0)
                w2 = (w / 4 + i) * 4;

	bytes_per_line = w2;
	
	setcolormap(im);
	makewindow(geomstring);

	/*
	 * Get mem for data in image
	 */
	if ((mem = data = malloc((unsigned)(w2 * h))) == NULL)
	{
		fprintf(stderr, "mem = NULL\n");
		exit(EXIT_FAILURE);
	}

	
	ximage = XCreateImage(display,
			visual,
			depth,
			ZPixmap,
			0,
			data,
			w2, (unsigned int)h,
			8, 0
		);

	if (ximage == (XImage *)NULL)
	{
		fprintf(stderr, "Unable to create ximage\n");
		exit(EXIT_FAILURE);
	}

	if (!upsidedown)
		mem += (h - 1) * bytes_per_line;

	y = h - 1;
	x = 0;
	val = -1;

	while (y >= 0)
	{
		line = im->pixel + y * im->cols;

		if (mag == 1)
		{
			(*copyfun)((char *) line, mem, wreal);
			if (upsidedown)
				mem += bytes_per_line;
			else
				mem -= bytes_per_line;
			y--;
		}
		else
		{
			for (j = 0; j < mag; j++)
			{
				mmem = mem;
				for (x = 0; x < wreal; x++)
					for (i = 0; i < mag; i++)
						*mmem++ = carray[line[x]];

				if (upsidedown)
					mem += bytes_per_line;
				else
					mem -= bytes_per_line;
			}
			y -= mag;
		}
	}

	XMapWindow(display, winder);

        /*
         * Wait for Exposure event.
         */
        do
	{
                XNextEvent(display, &event);
        } while (event.type != Expose);
 
        /*
         * Copy it to the screen
         */

	XPutImage(display, 
		winder,
		theGC, 
		ximage,
		0, 0, 
                0, 0,
                w, h
        );

	XSetInputFocus(display, winder, RevertToParent, CurrentTime);

	XFlush(display);

        /*
         * Wait for keypress or Button press
         */
        do
	{
                XNextEvent(display, &event);
		if (event.type == Expose)
		{
			/*
			 * Copy it to the screen
			 */

			XPutImage(display, 
				winder,
				theGC, 
				ximage,
				0, 0, 
				0, 0,
				w, h
			);

			XFlush(display);
		}

        } while (event.type != KeyPress && event.type != ButtonPress);

	XUnmapWindow(display, winder);
	XFreeColormap(display, cmap);
	XDestroyImage(ximage);
	XCloseDisplay(display);

	exit(EXIT_SUCCESS);
}


static int trydefaultcmap(OldPixMap_t *im)
{
	int i, num;
	XColor	tmp;

	/*
	 * First try to Allocate out of the default map...
	 */
	cmap = DefaultColormap(display, theScreen);

	num = 0;

	for (i = 0; i < im->colors; i++)
	{
#ifdef DEBUG
		fprintf(stderr, "Trying %d  ", i);
#endif
		tmp.pixel = (unsigned long)i;
		tmp.red = (unsigned short)(im->color[i].red << 8);
		tmp.green = (unsigned short)(im->color[i].green << 8);
		tmp.blue = (unsigned short)(im->color[i].blue << 8);
		tmp.flags = DoRed | DoGreen | DoBlue;
	/* Should use XAllocColorcells... */
		if (XAllocColor(display, cmap, &tmp) == 0)
		{
#ifdef DEBUG
		fprintf(stderr, "XAllocColor failed - default map not used.\n");
#endif
			if (verbose)
			{
				fprintf(stderr, "XAllocColor failed after %d colours allocated.\n", num);
				fprintf(stderr, "Using own colour map.\n");
			}

			return(0);
		}
#ifdef DEBUG
		fprintf(stderr, "Got %d\n", tmp.pixel);
#endif
		carray[i] = tmp.pixel;
		num++;

	}

	if (verbose)
		fprintf(stderr, "%d colours allocated from default colourmap.\n", num);

	copyfun = xbcopy;
	return(1);
}

static void useourowncmap(OldPixMap_t *im)
{
	int		i;
	unsigned int	size;
	XColor		*cols;


	cmap = XCreateColormap(display, rootw, visual, AllocAll);

	size = im->colors;

	if ((cols = (XColor *)malloc(size * sizeof(XColor))) == NULL)
	{
		fprintf(stderr, "Out of mem.\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < im->colors; i++)
	{
		cols[i].pixel = (unsigned long)i;
		cols[i].red = (unsigned short)(im->color[i].red << 8);
		cols[i].green = (unsigned short)(im->color[i].green << 8);
		cols[i].blue = (unsigned short)(im->color[i].blue << 8);
		cols[i].flags = DoRed | DoGreen | DoBlue;
	}

	copyfun = mybcopy;
	XStoreColors(display, cmap, cols, size);
	free(cols);
}

static void setcolormap(OldPixMap_t *im)
{
	if (forcecmap)
		useourowncmap(im);
	else if (!trydefaultcmap(im))
		useourowncmap(im);
}

static void makewindow(char *geomstring)
{
	unsigned int		wattrmask;
	XSetWindowAttributes	wattr;
	XSizeHints		sizehints;
	XWMHints		wmhints;
	unsigned int		bw, mask, wid, hi;
	int			xpos, ypos;

	bw = 3;
	mask = 0;
	sizehints.flags = 0;
	sizehints.x = DisplayWidth(display, theScreen) / 2 - w / 2;
	sizehints.y = DisplayHeight(display, theScreen) / 2 - h / 2;
	sizehints.width = w;
	sizehints.height = h;

	if (geomstring)
	{
		mask = XParseGeometry(geomstring, &xpos, &ypos, &wid, &hi);

		if (mask & XValue)
		{
			sizehints.flags |= USPosition;
			sizehints.x = xpos;
		}

		if (mask & YValue)
		{
			sizehints.flags |= USPosition;
			sizehints.y = ypos;
		}

		if (mask & WidthValue)
		{
			sizehints.flags |= USSize;
			sizehints.width = wid;
		}

		if (mask & HeightValue)
		{
			sizehints.flags |= USSize;
			sizehints.height = hi;
		}

		if (mask & XNegative)
			 sizehints.x = DisplayWidth(display, theScreen) - 2*bw - sizehints.width + xpos;

		if (mask & YNegative)
			sizehints.y = DisplayHeight(display, theScreen) - 2*bw - sizehints.height + ypos;

	}
	else
		sizehints.flags = PPosition | PSize;

	if (sizehints.width < w)
		fprintf(stderr, "Specified window width (%d) is less than image width (%d).\n", sizehints.width, w);

	if (sizehints.height < h)
		fprintf(stderr, "Specified window height (%d) is less than image height (%d).\n", sizehints.height, h);

/*
	fprintf(stderr, "%d %d, %d %d\n", sizehints.x, sizehints.y, sizehints.width, sizehints.height);
*/

        wattr.background_pixel = BlackPixel(display, theScreen);
        wattr.border_pixel = WhitePixel(display, theScreen);
	wattr.colormap = cmap;
	wattrmask = CWBackPixel | CWBorderPixel | CWColormap;

	winder = XCreateWindow(display,
			rootw,
			sizehints.x, sizehints.y,
			sizehints.width, sizehints.height,
			bw,
			depth,
			CopyFromParent,
			visual,
			wattrmask,
			&wattr
		);

	XSetWindowColormap(display, winder, cmap);

	XSetStandardProperties(display,
			winder,
			title, 
			myname,
			None,
			(char **)NULL, 0,
			&sizehints
	);

        wmhints.initial_state = NormalState;
        wmhints.input = True;
        wmhints.flags = StateHint | InputHint;
        XSetWMHints(display, winder, &wmhints);

	XSetForeground(display, theGC, WhitePixel(display, theScreen));
	XSetBackground(display, theGC, BlackPixel(display, theScreen));

        XSelectInput(display, winder, EV_MASK);
}
