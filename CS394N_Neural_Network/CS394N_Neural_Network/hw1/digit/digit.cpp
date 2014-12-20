/*
   Compile with:
     g++ -I/usr/X11R6/include/ -L/usr/X11R6/lib digit.cpp -o digit -lX11 

   110916 Leif Johnson
     Fixed a few things to compile under Ubuntu.

   010925 Marshall R. Mayberry
     Added "-fpermissive" option to get digit to compile on Suns.  Generates
     a lot of warnings, though.

   010128 Marshall R. Mayberry
     Added Lens   input/output format

   980403 James A. Bednar
     Converted to c++-compatible source using protoize(1)
     Added ability to handle multiple output and input formats switched
       by the value of variables <readtype> and <writetype>
     Added SNNS   input/output format
     Added Matlab input/output format
     Added ASCII  output format, printed to stdout if not saving when quitting

   980427 James A. Bednar
     Changed argument processing to use separate input and output filenames.
     Added PBM input/output format -- allows use of PBM and PNM tools to 
     process images; see "apropos pbm pnm" for details.  For a file
     patterns.pbm, it looks for an associated labels file
     patterns.label with a whitespace-separated list of digit labels.

   980510 James A. Bednar
     Added DISPLAY-less format translation mode -- when called using e.g.
     "env DISPLAY= digit in.pat out.pat" translates without opening X window.

     
   Usage: digit <filename> [<outputfilename> [<input_format> [<output_format>]]]

     Digit tries to guess the input and output formats based on the
     file extension, but if it guesses incorrectly, the format can be
     specified explicity using the codes listed in the #defines.
*/

/* include files */

#include <fstream>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

//extern "C" { int atoi(char*); }

/* declarations */

#define MAXIMAGE   5000
#define XRESOL     8
#define YRESOL     8
#define XSTART     200
#define YSTART     300
#define XGAP       20
#define YGAP       30
#define GAP        10
#define XSG        2
#define YSG        3
#define SGAP       2
#define OUTPUTCHARS 10

/* File format types */
#define PLANET 0
#define SNNS   1  // default
#define MATLAB 2
#define ASCII  3
//#define PDP    4
//#define PDPPP  5
#define PBM    6
#define LENS   7

using namespace std;

/* 980403 James A. Bednar  Defaults; overridden at command line if specified */
unsigned int readtype;
unsigned int writetype;

Display *mydisplay;
Window  outwindow,tilewindow,quitwindow,showindow;
Window  numberwindow[10],imagewindow[5];
Window  previouswindow,nextwindow,killwindow;
Window  savewindow,beginwindow,endwindow,gowindow;
GC blackgc, flipgc, smallgc, whitegc;
XFontStruct *the_fontstruct;

int images[MAXIMAGE+1][XRESOL][YRESOL] = {0};
int digit[MAXIMAGE+1];
unsigned int width = XRESOL * XGAP + 1;
unsigned int height = YRESOL * YGAP + 1;
unsigned int swidth = XRESOL * XSG + 2 * SGAP;
unsigned int sheight = YRESOL * YSG + 2 * SGAP;
int image_id[5] = {0,-1,-1,-1,-1};     /* pids for images shown in imagewindow[5] */
int image_frozen[5] = {0};             /* whether images of imagewindow[5] are frozen */
int image_x_coord[5] = {0};

void draw_image(void), fill_tile(int x, int y), unfill_tile(int x, int y), shift_image_array(int cid, int limit);
void draw_pid(int id, int previous), draw_save_windows(void), draw_small_windows(int redraw);
void shift_images(void);
void flip_window(Window window, unsigned int width, unsigned int height);
void draw_file_name(char *name, int modified),draw_number(int num);
int read_images_planet(char *fname);
int read_images_pbm(char *fname);
int read_images_snns(char *fname);
int read_images_matlab(char *fname);
int read_images_lens(char *fname);
void write_images_planet(char *fname, int pid);
void write_images_pbm(char *fname, int pid);
void write_images_snns(char *fname, int pid);
void write_images_lens(char *fname, int pid);
void write_images_matlab(char *fname, int pid);
void write_images_ascii(ostream& outstream, char * fname, int pid);
int small_window_p(Window window);
void swallow_comments(ifstream& file, char commentchar );

int guess_file_type(string filename)
{
  if      (filename.length() > 2 &&
	   filename.substr(filename.length()-2,filename.length()) == ".m")
    return MATLAB;

  else if (filename.length() > 4 &&
	   filename.substr(filename.length()-4,filename.length()) == ".pbm")
    return PBM;

  else if (filename.length() > 4 &&
	   filename.substr(filename.length()-4,filename.length()) == ".pat")
    return SNNS;

  else if (filename.length() > 4 &&
	   filename.substr(filename.length()-4,filename.length()) == ".txt")
    return ASCII;

  else if (filename.length() > 3 &&
	   filename.substr(filename.length()-3,filename.length()) == ".ex")
    return LENS;

  else
    return SNNS;
}


main(int argc, char **argv)
{
  /* declarations */
  char *fname, *outfname;
  XEvent myevent;
  KeySym mykey;
  XSizeHints myhint;
  int myscreen;
  unsigned long myforeground, mybackground;
  int i,j,x,y,k;
  int pid = 0;
  int new_pid = 0;
  int button = 0;
  int done;
  int modified = 0;
  int file_ext = 0;
  int save = 0;
  int gonum=0;
  int translate_only=0;
  char text[10];
  char temp_file[20];
  unsigned long mask;

  /* Process arguments */
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " <filename> [<outputfilename> [<input_format> [<output_format>]]]" << endl;
    exit(1);
  }
  else
    fname = *(argv + 1);
  
  if (argc > 2)
    outfname = *(argv + 2);
  else
    outfname = fname;
    
  if (argc > 3) 
    readtype = atoi(argv[3]);
  else
    readtype = guess_file_type(fname);
  
  if (argc > 4) 
    writetype = atoi(argv[4]);
  else
    writetype = guess_file_type(outfname);


  /* initialization */
  if ((mydisplay = XOpenDisplay("")) == NULL) {
    printf("Can't open X display; running in format translation mode only.\n");
    translate_only = True;
    /* exit(-1);*/
  }
  
  if (!translate_only) {

    myscreen = DefaultScreen(mydisplay);
    
    /* default pixel values */
    mybackground = WhitePixel(mydisplay,myscreen);
    myforeground = BlackPixel(mydisplay,myscreen);
    
    /* ************** Creating Windows ************ */
    
    /* Parent Window */
    
    myhint.x = XSTART; myhint.y = YSTART;
    myhint.width= width + (5 * GAP) + (3 * XGAP);
    myhint.height= height + (4 * GAP) + sheight;
    myhint.flags=PPosition | PSize;
    
    outwindow = XCreateSimpleWindow(mydisplay,DefaultRootWindow(mydisplay),
				    XSTART,YSTART,myhint.width,myhint.height,
				    5,myforeground,mybackground);
    XSetStandardProperties(mydisplay,outwindow,"Digit","Digit", None, NULL, 0,
			   &myhint);
    
    /* input event selection for the outer window */
    XSelectInput(mydisplay,outwindow, ExposureMask | KeyPressMask );
    
    /* Tile Window */
    
    tilewindow =
      XCreateSimpleWindow(mydisplay,outwindow,GAP,(3 * GAP + sheight),
			  width,height,2,myforeground,mybackground);
    
    /* Input selection for the tile window */
    XSelectInput(mydisplay,tilewindow, ButtonPressMask | ButtonMotionMask);
    
    /* Previous, Next, Kill windows */
    
    previouswindow = 
      XCreateSimpleWindow(mydisplay,outwindow,((GAP * 2) + width),
			  (3 * GAP + sheight),XGAP,XGAP,2,
			  myforeground,mybackground);
    XSelectInput(mydisplay,previouswindow, ButtonPressMask | ButtonReleaseMask);
    
    nextwindow =
      XCreateSimpleWindow(mydisplay,outwindow,((GAP * 3) + XGAP + width),
			  (3 * GAP + sheight),XGAP,XGAP,2,
			  myforeground,mybackground);
    XSelectInput(mydisplay,nextwindow, ButtonPressMask | ButtonReleaseMask);  
    
    killwindow =
      XCreateSimpleWindow(mydisplay,outwindow,((GAP * 4) + (2 * XGAP) + width),
			  (3 * GAP + sheight),XGAP,XGAP,2,
			  myforeground,mybackground);
    XSelectInput(mydisplay,killwindow, ButtonPressMask | ButtonReleaseMask);  
    
    /* Begin, End, Go windows */
    
    beginwindow =
      XCreateSimpleWindow(mydisplay,outwindow,((GAP * 2) + width),
			  (4 * GAP + sheight + XGAP),XGAP,XGAP,2,
			  myforeground,mybackground);
    XSelectInput(mydisplay,beginwindow, ButtonPressMask | ButtonReleaseMask);
    
    endwindow =
      XCreateSimpleWindow(mydisplay,outwindow,((GAP * 3) + XGAP + width),
			  (4 * GAP + sheight + XGAP),XGAP,XGAP,2,
			  myforeground,mybackground);
    XSelectInput(mydisplay,endwindow, ButtonPressMask | ButtonReleaseMask);  
    
    gowindow =
      XCreateSimpleWindow(mydisplay,outwindow,((GAP * 4) + (2 * XGAP) + width),
			  (4 * GAP + sheight + XGAP),XGAP,XGAP,2,
			  myforeground,mybackground);
    XSelectInput(mydisplay,gowindow, ButtonPressMask | ButtonReleaseMask);  
    
    
    /* Save Windows */
    
    for (i = 2; i < 5; i++)
      {
	for (j = 0; j < 3; j++)
	  {
	    k = 13 - (3 * i) + j;
	    numberwindow[k] =
	      XCreateSimpleWindow(mydisplay,outwindow,
				  ((GAP * 2) + (j * (XGAP + GAP)) + width),
				  (3 * GAP + sheight  + (i * (XGAP + GAP))),
				  XGAP,XGAP,2,myforeground,mybackground);
	    
	    /* input event selection */
	    XSelectInput(mydisplay,numberwindow[k],
			 ButtonPressMask | ButtonReleaseMask);
	  } /* for j */
      } /* for i */
    
    numberwindow[0] =
      XCreateSimpleWindow (mydisplay,outwindow,
			   ((GAP * 2) + width),
			   (3 * GAP + sheight  + (5 * (XGAP + GAP))),
			   XGAP,XGAP,2,myforeground,mybackground);
    
    /* input event selection */
    XSelectInput(mydisplay,numberwindow[0],
		 ButtonPressMask | ButtonReleaseMask);
    
    
    /* Quit window */
    
    quitwindow = 
      XCreateSimpleWindow (mydisplay,outwindow,
			   ((GAP * 3) + width + XGAP),
			   (3 * GAP + sheight + (5 * (XGAP + GAP))),
			   XGAP,XGAP,2,myforeground,mybackground);
    
    XSelectInput(mydisplay,quitwindow, ButtonPressMask | ButtonReleaseMask);
    
    savewindow = 
      XCreateSimpleWindow (mydisplay,outwindow,
			   ((GAP * 4) + width + (2 * XGAP)),
			   (3 * GAP + sheight + (5 * (XGAP + GAP))),
			   XGAP,XGAP,2,myforeground,mybackground);
    
    XSelectInput(mydisplay,savewindow, ButtonPressMask | ButtonReleaseMask);
    
    
    /* Show Window */
    
    showindow =
      XCreateSimpleWindow(mydisplay,outwindow,
			  ((GAP * 3) + width + XGAP),
			  (myhint.height - GAP - XGAP),
			  XGAP,XGAP,4,myforeground,mybackground);
    
    
    /* Image Windows */
    
    for (i = 0; i < 4; i++)
      {
	imagewindow[(4-i)] = 
	  XCreateSimpleWindow(mydisplay,outwindow,
			      (image_x_coord[(4-i)] =
			       (myhint.width - GAP - ((i + 1) * (swidth + 1)))),
			      (2 * GAP),swidth,sheight,1,myforeground,mybackground);
	XSelectInput(mydisplay,imagewindow[(4-i)], ButtonPressMask);
      }
    
    imagewindow[0] =
      XCreateSimpleWindow(mydisplay,outwindow,
			  (myhint.width - GAP - (5 * (swidth + 1)) - GAP),
			  (2 * GAP),swidth,sheight,2,myforeground,mybackground);
    
    
    
    
    /* GC Creation and Initializtion */
    
    blackgc = XCreateGC(mydisplay,outwindow,0,0); /* white foreground */
    flipgc = XCreateGC(mydisplay,outwindow,0,0);  /* highligting save windows */
    smallgc = XCreateGC(mydisplay,outwindow,0,0);  /* small characters */
    whitegc = XCreateGC(mydisplay,outwindow,0,0); /* erasure */
    
    XSetBackground(mydisplay,blackgc,mybackground);
    XSetForeground(mydisplay,blackgc,myforeground);
    XSetBackground(mydisplay,smallgc,mybackground);
    XSetForeground(mydisplay,smallgc,myforeground);
    XSetBackground(mydisplay,flipgc,mybackground);
    XSetForeground(mydisplay,flipgc,myforeground);
    XSetBackground(mydisplay,whitegc,myforeground);
    XSetForeground(mydisplay,whitegc,mybackground);
    
    
    /* Setting Up Fonts */
    
    the_fontstruct = 
      XLoadQueryFont(mydisplay,
		     /*		 "-adobe-times-bold-r-normal--14-140-75-75-p-77-iso8859-1");*/
		     "7x13bold");
    if (the_fontstruct != NULL) {
        XSetFont(mydisplay,blackgc,the_fontstruct->fid);
        XSetFont(mydisplay,flipgc,the_fontstruct->fid);
    } else {
        fprintf(stderr, "cannot load font 6x10");
    }
    
    the_fontstruct = 
      XLoadQueryFont(mydisplay,
		     /*		 "-adobe-times-bold-r-normal--10-100-75-75-p-57-iso8859-1");*/
		     "6x10");
    if (the_fontstruct != NULL) {
        XSetFont(mydisplay,smallgc,the_fontstruct->fid);
    } else {
        fprintf(stderr, "cannot load font 6x10");
    }
    
    
    /* Setting Up Flip GC */
    XSetFunction(mydisplay,flipgc,GXinvert);
    mask = myforeground ^ mybackground;
    XSetPlaneMask(mydisplay,flipgc,mask);
    
    /* window mapping */
    XMapRaised(mydisplay,outwindow);
    XMapSubwindows(mydisplay,outwindow);
  }
    

  for (i = 0; i < MAXIMAGE; i++)
    digit[i] = -1;

  /* Reading file Content if there is any */
  /* 980403 James A. Bednar  Generalized */
  switch(readtype)
    {
    case PLANET:
      new_pid = read_images_planet(fname);
      break;
    
    case SNNS:
      new_pid = read_images_snns(fname);
      break;
    
    case LENS:
      new_pid = read_images_lens(fname);
      break;
    
    case MATLAB:
      new_pid = read_images_matlab(fname);
      break;
    
    case PBM:
      new_pid = read_images_pbm(fname);
      break;
    
    default:
      printf("Unknown read type of file: %d",readtype);
      exit(1);
    } 

  /* main event-reading loop */
  done = 0;
  
  if (translate_only) {
    done = 1;

    /* Save if formats and/or filenames differ */
    if ((fname != outfname) || (readtype != writetype)) 
      save = 1;
  }

  while (done == 0)
    {
      /* read the next event */
      XNextEvent(mydisplay,&myevent);
      switch(myevent.type)
	{
	  /* repaint window on expose events */
	case Expose:
	  if (myevent.xexpose.count == 0)
	    {
	      draw_file_name(fname,0); 
	      draw_image();
	      draw_pid(pid,-1); 
	      draw_save_windows(); 
	      draw_small_windows(1);
	      XFlush(mydisplay);
	    }
	  break;
	  /* process keyboard mapping changes */
	    
	case MappingNotify:
	  XRefreshKeyboardMapping((XMappingEvent*)&myevent);
	  break;
	    
	    /* process mouse-button presses */
	    
	case ButtonPress:
	  if (myevent.xbutton.window == tilewindow)
	    {
	      button = myevent.xbutton.button;
	      if (button == 1)
		fill_tile(myevent.xbutton.x,myevent.xbutton.y);
	      else
		unfill_tile(myevent.xbutton.x,myevent.xbutton.y);
	      if (modified == 0)
		{
		  modified = 1;
		  draw_file_name(fname,1);
		}
	    }
	  else if (k = small_window_p(myevent.xbutton.window))
	    {
	      flip_window(myevent.xbutton.window,swidth,sheight);
	      XFlush(mydisplay);
	      if (image_frozen[k] == 0)
		image_frozen[k] = 1;
	      else image_frozen[k] = 0;
	    }
	  else 
	    flip_window(myevent.xbutton.window,XGAP,XGAP);
	  XFlush(mydisplay);
	  break;
	    
	    /* process mouse buton release */
	case ButtonRelease:
	  if (myevent.xbutton.window == savewindow)
	    {
	      save = 1;
	      done = 1;
	    }
	  else if (myevent.xbutton.window == quitwindow)
	    done = 1;
	  else if (myevent.xbutton.window == previouswindow)
	    {
	      flip_window(previouswindow,XGAP,XGAP);
	      if (pid > 0)
		{
		  XFlush(mydisplay);
		  pid--;
		  shift_images();
		  image_id[0] = pid;
		  draw_pid(pid,(pid+1));
		  draw_small_windows(0);
		  draw_image();
		}
	    }
	  else if (myevent.xbutton.window == nextwindow)
	    {
	      flip_window(nextwindow,XGAP,XGAP);
	      if (pid < new_pid)
		{
		  XFlush(mydisplay);
		  shift_images();
		  pid++;
		  draw_pid(pid,(pid-1));
		  draw_small_windows(0);
		  draw_image();
		}
	    }
	  else if (myevent.xbutton.window == killwindow)
	    {
	      flip_window(killwindow,XGAP,XGAP);
	      XFlush(mydisplay);
	      if (pid == new_pid)
		new_pid++;
	      shift_image_array(pid,new_pid);
	      new_pid--;
	      if (modified == 0)
		{
		  modified = 1;
		  draw_file_name(fname,1);
		}
	      draw_small_windows(0);
	      draw_image();
	    }
	  else if (myevent.xbutton.window == beginwindow)
	    {
	      flip_window(beginwindow,XGAP,XGAP);
	      XFlush(mydisplay);
	      shift_images();
	      image_id[0] = 0;
	      draw_pid(0,pid);
	      pid = 0;
	      draw_small_windows(0);
	      draw_image();
	    }
	  else if (myevent.xbutton.window == endwindow)
	    {
	      flip_window(endwindow,XGAP,XGAP);
	      XFlush(mydisplay);
	      shift_images();
	      image_id[0] = new_pid;
	      draw_pid(new_pid,pid);
	      pid = new_pid;
	      draw_small_windows(0);
	      draw_image();
	    }
	  else if (myevent.xbutton.window == gowindow)
	    {
	      flip_window(gowindow,XGAP,XGAP);
	      XFlush(mydisplay);
	      if (gonum >= 0 && gonum <= new_pid)
		{
		  shift_images();
		  image_id[0] = gonum;
		  draw_pid(gonum,pid);
		  pid = gonum;
		  draw_small_windows(0);
		  draw_image();
		}
	      gonum = 0;
	    }
	  else 
	    {
	      flip_window(myevent.xbutton.window,XGAP,XGAP);
	      XFlush(mydisplay);
	      for (k = 0; k < 10; k++)
		if (numberwindow[k] == myevent.xbutton.window)
		  {
		    draw_number(k);
		    digit[pid] = k;
		    if ((pid == new_pid) && (new_pid < MAXIMAGE))
		      new_pid++;
		    if (modified == 0)
		      {
			modified = 1;
			draw_file_name(fname,1);
		      }
		  } /* for k */
	    } /* else */
	  XFlush(mydisplay);
	  break;
	    
	  /* process mouse-button moves */
	    
	case MotionNotify:
	  if (button == 1)
	    fill_tile(myevent.xmotion.x, myevent.xmotion.y);
	  else
	    unfill_tile(myevent.xmotion.x, myevent.xmotion.y);
	  break;
	    
	    
	case KeyPress:
	  i = XLookupString((XKeyEvent*)&myevent,text,10,&mykey,0);
	  if ((i == 1) && (text[0] >= '0' && text[0] <= '9'))
	    gonum = gonum * 10 + (text[0] - '0');
	  else
	    gonum = 0;
	  break;
	    
	} /* switch (myevent.type) */
    } /* while (done == 0) */

  /* 980403 James A. Bednar   Generalized */
  if (save == 1) {
    switch(writetype)
      {
      case PLANET:
	write_images_planet(outfname,new_pid);
	break;
    
      case SNNS:
	write_images_snns(outfname,new_pid);
	break;
    
      case LENS:
	write_images_lens(outfname,new_pid);
	break;
    
      case MATLAB:
	write_images_matlab(outfname,new_pid);
	break;
    
      case ASCII: {
	ofstream file(outfname);
	if (!file) {
	  cerr << "Couldn't create file";
	  exit(1);
	}
	write_images_ascii(file,outfname,new_pid);
	break;
      }    
      case PBM:
	write_images_pbm(outfname,new_pid);
	break;
    
      default:
	printf("Unknown write type of file: %d",writetype);
	exit(1);
      } 
  }
  else if (translate_only)
    /* At least copy to standard output if not saving */
    write_images_ascii(cout,fname,new_pid);

  if (!translate_only) {
    /* termination */
    XFreeGC(mydisplay,blackgc);
    XFreeGC(mydisplay,flipgc);
    XFreeGC(mydisplay,whitegc);
    XFreeGC(mydisplay,smallgc);
    XDestroyWindow(mydisplay,outwindow);
    XCloseDisplay(mydisplay);
  }
  
  exit(0);
}
			 
/* draw main images */


void draw_image(void)
{
  int i,j,x,y;
  int k = image_id[0];
  void draw_number(int num);

  XClearWindow(mydisplay,tilewindow);
  for (i = 1; i < XRESOL; i++)   /* vertical lines */
    {
      x = (i * XGAP);
      XDrawLine(mydisplay,tilewindow,blackgc,x,0,x,height);
    }
  for (j = 1; j < YRESOL; j++)   /* horizontal lines */
    {
      y = (j * YGAP);
      XDrawLine(mydisplay,tilewindow,blackgc,0,y,width,y);
    }
  for (i = 0; i < YRESOL; i++)
    {
      y = (i * YGAP);
      for (j = 0; j < XRESOL; j++)
	{
	  x = (j * XGAP);
	  if (images[k][i][j] == 1)
	    XFillRectangle(mydisplay,tilewindow,flipgc,x,y,XGAP,YGAP);
	} /* for j */
    } /* for i */
  draw_number(digit[k]);
}

/* draw number */

void draw_number(int num)
{
  char string[2];

  XClearWindow(mydisplay,showindow);
  if (num >= 0)
    {
      sprintf(string,"%d",num);
      XDrawString(mydisplay,showindow,blackgc,7,(XGAP-6),string,
		  strlen(string));
    }
}


/* draw small windows */

void draw_small_windows(int redraw)
{
  int i,j,k,x,y,id;
  char string[4];

  for (k = 0; k < 5; k++)
    {
      if ((image_frozen[k] == 0) && ((id = image_id[k]) >= 0))
	{
	  if (k > 0)
	    {
	      XFillRectangle(mydisplay,outwindow,whitegc,image_x_coord[k],
			     (GAP - 1), swidth,GAP);
	      sprintf(string,"%d",image_id[k]);
	      XDrawString(mydisplay,outwindow,smallgc,(image_x_coord[k] + 5),
			  (2 * GAP -3), string, strlen(string));
	    }
	  XClearWindow(mydisplay,imagewindow[k]);
	  for (i = 0; i < YRESOL; i++)
	    {
	      y = (SGAP + (i * YSG));
	      for (j = 0; j < XRESOL; j++)
		{
		  x = (SGAP + (j * XSG));
		  if (images[id][i][j] == 1)
		    XFillRectangle(mydisplay,imagewindow[k],flipgc,x,y,XSG,YSG);
		} /* for j */
	    } /* for i */
	} /* if image_frozen[k] */
      else if ((redraw == 1) && ((id = image_id[k]) >= 0))
	{
	  XFillRectangle(mydisplay,outwindow,whitegc,image_x_coord[k],(GAP - 1),
			 swidth,GAP);
	  sprintf(string,"%d",image_id[k]);
	  XDrawString(mydisplay,outwindow,smallgc,(image_x_coord[k] + 5),
		      (2 * GAP -3), string, strlen(string));
	  XClearWindow(mydisplay,imagewindow[k]);
	  flip_window(imagewindow[k],swidth,sheight);
	  for (i = 0; i < YRESOL; i++)
	    {
	      y = (SGAP + (i * YSG));
	      for (j = 0; j < XRESOL; j++)
		{
		  x = (SGAP + (j * XSG));
		  if (images[id][i][j] == 1)
		    XFillRectangle(mydisplay,imagewindow[k],flipgc,x,y,XSG,YSG);
		} /* for j */
	    } /* for i */
	} /* else if */
    } /* for k */
}


/* draw save windows */

void draw_save_windows(void)
{
  char pstring[2];
  int i;

  XDrawString(mydisplay,previouswindow,blackgc,6,(XGAP - 5),
	      "P",strlen("P"));
  XDrawString(mydisplay,nextwindow,blackgc,5,(XGAP - 5),
	      "N",strlen("N"));
  XDrawString(mydisplay,killwindow,blackgc,5,(XGAP - 5),
	      "K",strlen("K"));
  XDrawString(mydisplay,beginwindow,blackgc,6,(XGAP - 5),
	      "B",strlen("B"));
  XDrawString(mydisplay,endwindow,blackgc,5,(XGAP - 5),
	      "E",strlen("E"));
  XDrawString(mydisplay,gowindow,blackgc,5,(XGAP - 5),
	      "G",strlen("G"));
  
  for (i = 0; i < OUTPUTCHARS; i++)
    {
      sprintf(pstring,"%d",i);
      XDrawString(mydisplay,numberwindow[i],blackgc,7,(XGAP - 5),
		  pstring,strlen(pstring));
    }
  XDrawString(mydisplay,quitwindow,blackgc,5,(XGAP - 5),
	      "Q",strlen("Q"));
  XDrawString(mydisplay,savewindow,blackgc,5,(XGAP - 5),
	      "S",strlen("S"));
}

/* flip a window */

void flip_window(Window window, unsigned int width, unsigned int height)
{
  XFillRectangle(mydisplay,window,flipgc,0,0,width,height);
  XFlush(mydisplay);
}


/* drawing a pid */

void draw_pid (int id, int previous)
{
  char pstring[256];

  if (previous >= 0)
    {
      sprintf(pstring, "Pattern: %d", previous);
      XDrawString(mydisplay,outwindow,flipgc,GAP,(sheight + GAP),
		  pstring, strlen(pstring));      
    }
  sprintf(pstring,"Pattern: %d",id);
  XDrawString(mydisplay,outwindow,blackgc,GAP,(sheight + GAP),
	      pstring, strlen(pstring));
}

/* shifting images */

void shift_images(void)
{
  int i,j,k,l;
  k = 4;
  while (k > 0)
    {
      if (image_frozen[k] == 1)
	k--;
      else
	{
	  l = k - 1;
	  while (l >= 0)
	    {
	      if (image_frozen[l] == 0)
		{
		  image_id[k] = image_id[l];
		  k = l;
		  l = -1;}
	      else
		l--;
	    } /* while l */
	} /* else */
    } /* while k */
  image_id[0] = image_id[0] + 1;
}


/*  procedures for flipping a tile */

void fill_tile(int x, int y)
{
  int j = (int) (x / XGAP);
  int i = (int) (y / YGAP);
  int k = image_id[0];

  if (i >= 0 && i < YRESOL && j >= 0 && j < XRESOL)
    if (images[k][i][j] == 0)
      {
	images[k][i][j] = 1;
	XFillRectangle(mydisplay,tilewindow,flipgc,(j * XGAP),(i * YGAP),
		       XGAP,YGAP);
	XFillRectangle(mydisplay,imagewindow[0],blackgc,(SGAP + (j * XSG)),
		       (SGAP + (i * YSG)), XSG,YSG);
      }
}

void unfill_tile(int x, int y)
{
  int j = (int) (x / XGAP);
  int i = (int) (y / YGAP);
  int k = image_id[0];

  if (i >= 0 && i < YRESOL && j >= 0 && j < XRESOL)
    if (images[k][i][j] == 1)
      {
	images[k][i][j] = 0;
	XFillRectangle(mydisplay,tilewindow,flipgc,(j * XGAP),(i * YGAP),
		       XGAP,YGAP);
	XFillRectangle(mydisplay,imagewindow[0],flipgc,(SGAP + (j * XSG)),
		       (SGAP + (i * YSG)), XSG,YSG);
      }
}



int small_window_p(Window window)
{
  int i;

  for (i = 1; i < 5; i++)
    if (window == imagewindow[i])
      return(i);
  return(0);
}

void shift_image_array(int cid, int limit)
{
  int i,j,k,l;
  char string[3];

  limit--;
  for (k = cid; k < limit; k++)
    {
      l = k+1;
      digit[k] = digit[l];
      for (i = 0; i < YRESOL; i++)
	for (j = 0; j < XRESOL; j++)
	  images[k][i][j] = images[l][i][j];
    }

  for (i = 0; i < YRESOL; i++)
    for (j = 0; j < XRESOL; j++)
      images[limit][i][j] = 0;
  digit[limit] = -1;
  for (i = 1; i < 5; i++)
    if (image_id[i] > cid)
      {
	image_id[i]--;
	XFillRectangle(mydisplay,outwindow,whitegc,image_x_coord[i],
		       (GAP - 1), swidth,GAP);
	sprintf(string,"%d",image_id[i]);
	XDrawString(mydisplay,outwindow,smallgc,(image_x_coord[i] + 5),
		    (2 * GAP -3), string, strlen(string));
      }
}
  

void draw_file_name(char *name, int modified)
{
  char print_name[30];

  if (modified == 0)
    sprintf(print_name,"%s",name);
  else
    sprintf(print_name,"%s*",name);
  XDrawString(mydisplay,outwindow,blackgc,GAP,((int)(sheight / 2) + GAP), 
	      print_name, strlen(print_name)); 
}     



/*****************************************************************************/
/*			       File I/O                                      */
/*****************************************************************************/

/* 980403 James A. Bednar */
void swallow_line(ifstream& file)
{
  char c;
  while (file.get(c) && (c != '\n'));

  //  while (file.get(c) && (c != '\n'))
  //  cout << c;
  // cout << endl;
}


/* 980403 James A. Bednar */
void swallow_trash(ifstream& file)
{
  while (file.good() && !(file.eof()) && (!isdigit(file.peek()))) {
    // cout << "Swallowing: ";
    swallow_line(file);
    // cout << endl;
  }
}

/* 010128 Marshall R. Mayberry */
void swallow_nonbits(ifstream& file)
{
  char c;
  while (file.good() && !(file.eof()) && (!isdigit(file.peek()))) {
    file.get(c);
    // cout << "Swallowing: " << c << endl;
  }
}


/* 980427 James A. Bednar   Assumes called at eol*/
void swallow_comments(ifstream& file, char commentchar )
{
  while ( file.good() && !file.eof() && file.peek()==commentchar ) {
    //cout << "Swallowing: ";
    swallow_line(file);
    //cout << endl;
  }
}


/* 980403 James A. Bednar   Generalized */
void write_images_planet(char *fname, int pid)
{
  int i,j,k;

  FILE *file = fopen(fname,"w");
  for (k = 0; k < pid; k++)
    if (digit[k] >= 0) { /* Only output valid patterns */
      for (i = 0; i < YRESOL; i++)
	{
	  for (j = 0; j < XRESOL; j++)
	    if (images[k][i][j] == 1)
	      fprintf(file, "a");
	    else
	      fprintf(file,"%d",images[k][i][j]);
	}
      fprintf(file," ");
      for (i = 0; i < OUTPUTCHARS; i++)
	{
	  if (i == digit[k])
	    fprintf(file, "a");
	  else 
	    fprintf(file, "0");
	}
      fprintf(file, "\n");
    }
  (void) fclose(file);
}


/* 980403 James A. Bednar   Made safer using C++ primitives */
/* Assumes integer format with only binary values */
int read_images_planet(char *fname)
{
  int id = 0;
  int bit = 0;
  int i,j;
  unsigned char c;
  ifstream file(fname);

  if (!file) return(id);
  
  while (!file.eof() && (id < MAXIMAGE)) {
      
    //cout << "Reading " << id << "..." << endl;

    for (i = 0; i < YRESOL; i++)
      for (j = 0; j < XRESOL; j++) {
	file >> c;
	images[id][i][j] = (c == '0' ? 0 : 1);
      }

    for (i = 0; i < OUTPUTCHARS; i++)
      {
	file >> c; /* Automatically skips preceding whitespace */
	if (!(c == '0')) digit[id] = i;
      }
    
    file >> ws;
    
    //cout << "Done with " << id << endl;

    id++;
  } 

  return(id);
}


string determine_label_filename(string filename, int type);
string determine_label_filename(string filename, int type)
{
  string basename;
  if (filename.length() > 4 &&
      filename.substr(filename.length()-4,filename.length()) == ".pbm")
    basename = filename.substr(0,filename.length()-4);
  else
    basename = filename;

  return basename+".label";
}


void write_images_pbm(char *fname, int pid)
{
  int i,j,k;

  ofstream file(fname);

  if (!file) {
    cerr << "Couldn't create file";
    exit(1);
  }

  ofstream labelfile(determine_label_filename(fname,PBM).c_str());


  /* Write header */
  file << "P1\n" << XRESOL << " " << YRESOL*pid <<endl;
  
  /* Write bitmap */
  for (k = 0; k < pid; k++)
    if (digit[k] >= 0) { /* Only output valid patterns */
      for (i = 0; i < YRESOL; i++) {
	for (j = 0; j < XRESOL; j++)
	  file << images[k][i][j];
	file << endl;
      }
      labelfile << digit[k] << endl;
    }
}


/* 980403 James A. Bednar   Read PBM image with bitmaps and
   associated label file (assumed filename - ".pbm" + ".label") */
int read_images_pbm(char *fname)
{
  int id = 0;
  int bit = 0;
  int i,j;
  unsigned char c;
  ifstream file(fname);
  string type;
  int width, height;
  
  if (!file) return(id);

  /* Swallow header */
  file >> type;
  if (type != "P1"){
    cerr << "Type of file is not PBM ASCII; aborting.\n";
    exit(1);
  }
  swallow_line(file);
  swallow_comments(file,'#');
  file >> width;
  swallow_comments(file,'#');
  file >> height;
  swallow_line(file);

  /* Open labels file */
  ifstream labels(determine_label_filename(fname,PBM).c_str());
  if (!labels) {
    cerr << "No labels file " << determine_label_filename(fname,PBM) << " found." << endl;
  }

  /* Read bitmaps */
  while (!file.eof() && (id < MAXIMAGE)) {
	
    //cout << "Reading " << id << "..." << endl;
 
    for (i = 0; i < YRESOL; i++)
      for (j = 0; j < XRESOL; j++) {
	file >> c;
	images[id][i][j] = (c == '0' ? 0 : 1);
      }

    if (labels) 
      labels >> digit[id];
    else
      digit[id] = 0;
    
    file >> ws;
    
    //cout << "Done with " << id << endl;
 
    id++;
  } 

  return(id);
}


/* 980403 James A. Bednar */
/* Should also work with many other formats fitting these
   assumptions:
   1. Every line starting with a digit contains pattern data, and only
      pattern data.
   2. Individual pattern values are represented as
      whitespace-separated integers.  
   3. Input data for a pattern is followed by the target outputs.
   4. Input and target sections contain only values, not other data
      interspersed.  
*/
int read_images_snns(char * fname)
{
  int id = 0;
  int bit = 0;
  int i,j;
  ifstream file(fname);

  if (!file) return(id);
  
  swallow_trash(file);
  
  while (file.good() && !(file.eof()) && (id < MAXIMAGE)) {

    //cout << "Reading " << id << "..." << endl;

    for (i = 0; i < YRESOL; i++)
      for (j = 0; j < XRESOL; j++) 
	file >> images[id][i][j];
    
    swallow_trash(file);

    for (i = 0; i < OUTPUTCHARS; i++)
      {
	file >> bit;
	if (bit == 1) digit[id] = i;
      }

    //cout << "Done with " << id << endl;

    swallow_trash(file);
    file >> ws;

    id++;
  } 

  return(id);
}


/* 980403 James A. Bednar */
void write_images_snns(char *fname, int pid)
{
  ofstream file(fname);

  if (!file) {
    cerr << "Couldn't create file";
    exit(1);
  }

  /* Print the header */
  time_t t;
  time(&t);
  file << "SNNS pattern definition file V1.4"        << endl;
  file << "generated at "          <<  ctime(&t)     << endl << endl;
  file << "No. of patterns     : " <<  pid           << endl;
  file << "No. of input units  : " <<  XRESOL*YRESOL << endl;
  file << "No. of output units : " <<  OUTPUTCHARS   << endl << endl;

  /* Print the patterns */
  for(int k=0;k<pid;k++) {

    if (digit[k] >= 0) { /* Only output valid patterns */
      file << "#Input Number "<< k+1 << endl; 
      for(int i=0; i<XRESOL; i++) {
	for(int j=0; j<YRESOL; j++)
	  file << images[k][i][j] << " ";
	file << endl;
      }
      
      file << "# Output Number "<< k+1 << " below " << endl; 
      for(int i=0; i<OUTPUTCHARS; i++) 
	file << (i == digit[k] ? 1 : 0)  << " ";
      file << endl;
    }
  }
}

/* 010128 Marshall R. Mayberry */
int read_images_lens(char * fname)
{
  int id = 0;
  int bit = 0;
  int i,j;
  ifstream file(fname);

  if (!file) return(id);
  
  swallow_nonbits(file);
  
  while (file.good() && !(file.eof()) && (id < MAXIMAGE)) {

    //    cout << "Reading " << id << "..." << endl;

    for (i = 0; i < YRESOL; i++)
      for (j = 0; j < XRESOL; j++) 
	file >> images[id][i][j];
    
    swallow_nonbits(file);

    for (i = 0; i < OUTPUTCHARS; i++)
      {
	file >> bit;
	if (bit == 1) digit[id] = i;
      }

    //    cout << "Done with " << id << endl;

    swallow_nonbits(file);
    file >> ws;

    id++;
  } 

  return(id);
}

/* 010128 Marshall R. Mayberry */
void write_images_lens(char *fname, int pid)
{
  ofstream file(fname);

  if (!file) {
    cerr << "Couldn't create file";
    exit(1);
  }

  /* Print a header
  time_t t;
  time(&t);
  file << "LENS pattern definition file"             << endl;
  file << "generated at "          <<  ctime(&t)     << endl << endl;
  file << "No. of patterns     : " <<  pid           << endl;
  file << "No. of input units  : " <<  XRESOL*YRESOL << endl;
  file << "No. of output units : " <<  OUTPUTCHARS   << endl << endl;
  */

  /* Print the patterns */
  for(int k=0;k<pid;k++) {

    if (digit[k] >= 0) { /* Only output valid patterns */
      file << "I:" << endl;
      for(int i=0; i<XRESOL; i++) {
	for(int j=0; j<YRESOL; j++)
	  file << images[k][i][j] << " ";
	file << endl;
      }
      
      file << "T:";
      for(int i=0; i<OUTPUTCHARS; i++) 
	file << " " << (i == digit[k] ? 1 : 0);
      file << ";" << endl << endl;
    }
  }
}

/* 980403 James A. Bednar */
/* Discards input until it sees a bracket followed by a number */
void swallow_until_bracket(ifstream& file)
{
  char c;
  while (file.get(c) && !(c == '[' && isdigit(file.peek())));

  //while (file.get(c) && !(c == '[' && isdigit(file.peek())))
  //  cout << c;
  //cout << endl;
}


/* 980404 James A. Bednar */
int read_images_matlab(char * fname)
{
  int id = 0;
  int bit = 0;
  int i,j;
  ifstream file(fname);

  if (!file) return(id);
  
  swallow_until_bracket(file);
  
  while (file.good() && !(file.eof()) && (id < MAXIMAGE)) {

    //cout << "Reading " << id << "..." << endl;

    for (i = 0; i < YRESOL; i++) {
      for (j = 0; j < XRESOL; j++) 
	file >> images[id][i][j];
      swallow_line(file);  
    }
    
    swallow_until_bracket(file);

    for (i = 0; i < OUTPUTCHARS; i++)
      {
	file >> bit;
	if (bit == 1) digit[id] = i;
      }

    //cout << "Done with " << id << endl;

    swallow_until_bracket(file);

    id++;
  } 

  return(id);
}


/* 980403 James A. Bednar */
void write_images_matlab(char *fname, int pid)
{
  ofstream file(fname);

  if (!file) {
    cerr << "Couldn't create file";
    exit(1);
  }

  /* Print the header */
  time_t t;
  time(&t);
  file << "function [patterns,targets] = patterns()"     << endl;
  file << "% Pattern definitions for template.m"       << endl;
  file << "% Generated at "          <<  ctime(&t);
  file << "% " << endl;
  file << "% No. of patterns     : " <<  pid           << endl;
  file << "% No. of input units  : " <<  XRESOL*YRESOL << endl;
  file << "% No. of output units : " <<  OUTPUTCHARS   << endl << endl;

  /* Print the patterns */
  for(int k=0;k<pid;k++) {

    if (digit[k] >= 0) { /* Only output valid patterns */
      file << "pattern" << k+1 << " =  "; 
      if (k < 9) file << " ";
      file << "[";
      
      for(int i=0; i<XRESOL; i++) {
	for(int j=0; j<YRESOL; j++)
	  file << images[k][i][j] << " ";
	if (i == XRESOL-1)
	  file << "]';" << endl << endl;
	else
	  file << "..." << endl << "              ";
      }

      file << "target" << k+1 << " =  ["; 
      for(int i=0; i<OUTPUTCHARS; i++) 
	file << (i == digit[k] ? 1 : 0)  << " ";
      file << "]';" << endl << endl;
    }
  }

  /* Print the footer */
  file << "patterns = [";
  for(int k=0;k<pid;k++) {
    file << "pattern" << k+1;
    if (k == pid-1)
      file << "];" << endl << endl;
    else 
      file <<",";      
  }
  
  file << "targets = [";
  for(int k=0;k<pid;k++) {
    file << "target" << k+1;
    if (k == pid-1)
      file << "];" << endl << endl;
    else 
      file <<",";      
  }
}


/* 980403 James A. Bednar */
void write_images_ascii(ostream& outstream, char * fname, int pid)
{
  /* Print the header */
  time_t t;
  time(&t);
  outstream << "-------------------------------------------------------------------------------" << endl;
  outstream << "Contents of file " << fname << " on " <<  ctime(&t) << endl << endl;

  /* Print the patterns */
  for(int k=0;k<pid;k++) 
    if (digit[k] >= 0) { /* Only output valid patterns */
      
      outstream << "Digit " << digit[k] << ", Pattern "<< k+1 << endl; 
      
      for(int i=0; i<XRESOL; i++) {
	for(int j=0; j<YRESOL; j++)
	  outstream << (images[k][i][j] ? "#" : ".");
	outstream << endl;
      }
      outstream << endl;
    }
}


/* Unneeded */
/* int file_copy(char *dest, char *source)
{
  FILE *sfp,*dfp;
  char c;
  
 if ((sfp = fopen(source,"r")) == NULL)
   printf("Couldn't open %s for reading\n", source);
 else
   {
     if ((dfp = fopen(dest,"w")) == NULL)
       printf("Couldn't open %s for writing\n",dest);
     else
       {
	 while ((c=getc(sfp)) != EOF) 
	   putc(c,dfp);
	 (void) fclose(dfp);
       }
     (void) fclose(sfp);
   }
  return(sfp&&dfp);
}
*/
