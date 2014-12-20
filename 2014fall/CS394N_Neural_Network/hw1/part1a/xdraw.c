/* ----------------------------- File :xdraw.c ----------------------------- *
 * XWindow drawing routines for visualizing the network.
 * Author: Joseph Sirosh. Sept 19, 1993.
 * 
 * ------------------------------------------------------------------------- */

#include "sofmdefs.h"      /* Includes Xlib.h, Xutil.h, Xm.h, 
                               * stdio.h and math.h                         */

#define MAXLINES   1024       /* To draw upto 1024 lines in a single call.  */
#define MAXPOINTS  1024       /* To draw upto 1024 points in a single call. */

extern double distr_weight(); /* main.c: Used to calculate ideal gridpts  */
extern double scale01()     ; /* geninp.c: Used to scale to 0-1 range       */
extern int Activity_Color_Range; /* Set by display_init()                   */
extern XColor colors[255]   ;

void display_input()        ;
void display_net()          ;
void draw_border()          ;
void draw_grid()            ;
void display_activity()     ;
/* ------------------------------------------------------------------------ */
void display_input(d, input_list)          
/* This procedure takes the first two elements of the raw input vectors
 * in the input_list as x,y co-ordinates in a 2d space and plots them.
 *
 * The foreground color has already been set in display_init.
 */
My_Display *d;
Input_history *input_list;
{ 
  extern Pixel newinput_color; /* Set in display_init()  */
  int x, y, count            ;
  Input_history *ptr         ;
  XPoint Points[MAXPOINTS]   ;
  Dimension width=0, height=0;
  extern Widget inp_drawingA ;

  if (input_list != NULL){
   
    XClearWindow(d->input.d, d->input.w);
    XtVaGetValues(inp_drawingA,XmNwidth,&width,
		  XmNheight,&height,NULL);

    x = (int)ROUND((double)width  * input_list->input_vector[0]);
    y = (int)ROUND((double)height * (1-input_list->input_vector[1]));

    /* The latest point (at the head of the list )should appear bigger -
     *  so we draw a filled rectangle centered around the actual x,y point.
     */
    XSetForeground(d->input.d, d->input.gc, newinput_color)            ;
    XFillRectangle(d->input.d, d->input.w, d->input.gc, x, y, 4, 4);
    XSetForeground(d->input.d, d->input.gc, d->input.fg)               ;
    
    for(count=0, ptr=input_list->next; ptr!=NULL; ptr=ptr->next)
      {
	Points[count].x   = (int)ROUND((double)width  * 
				       ptr->input_vector[0])   ;
	Points[count++].y = (int)ROUND((double)height * 
				       ptr->input_vector[1]) ;
	
	if (count==MAXPOINTS){
	  XDrawPoints(d->input.d, d->input.w, d->input.gc, Points,
		      count, CoordModeOrigin)                          ;
	  count = 0                                                    ;
	}
}
    if (count>0){
      XDrawPoints(d->input.d, d->input.w, d->input.gc, Points,
		  count, CoordModeOrigin)                              ;
      count = 0                                                        ;
    }
    draw_border(d->input.d, d->input.w, d->input.gc,
	      width, height, d->input.fg)            ;
    XFlush(d->input.d)                                                 ;
  }
}
/* ---------------------------------------------------------------------- */
void display_net(d, net, nx,ny)
/* The drawing color is the foreground color of the display.
 * 
 */
My_Display *d;
struct unitdata net[maxnx][maxny];
int nx,ny;
{
  int i,j,count=0    ;
  double x0, y0                   ;
  XSegment lines[MAXLINES]        ;
  XPoint gridpoints[maxnx][maxny]   ; /* The points of the 2d grid */
  Dimension width=0, height=0;
  extern Widget net_drawingA ;

  XClearWindow(d->net.d, d->net.w);
  XtVaGetValues(net_drawingA,XmNwidth,&width,
		    XmNheight,&height,NULL);
  for(i=0; i < nx; i++)
    for(j=0; j< ny; j++)
      {
	x0 = (double)height * net[i][j].comp[0];
	y0 = (double)width  * net[i][j].comp[1];

	gridpoints[i][j].x = (int)ROUND(x0);
	gridpoints[i][j].y = (int)ROUND(y0);
      }

  draw_grid(d->net.d,d->net.w,d->net.gc,gridpoints,nx,ny,lines);

  /* Draw a square to show the border of the network */
  draw_border(d->net.d, d->net.w, d->net.gc,
	      width, height, d->net.fg);

  XFlush(d->net.d);
}
	
/* ------------------------------------------------------------------------ */

void draw_grid(d, w, gc, gridpoints, nx,ny, lines) 
Display *d; Window w; GC gc;XPoint gridpoints[maxnx][maxny];
int nx,ny ; XSegment *lines;
{
  int i, j, count=0;
  
  for(i=0; i < nx; i++)
    for(j=0; j < ny; j++)
      {
	if (i < nx-1) /* Draw horizontal lines upto gridsize-1 */
	  {
	    lines[count].x1   = gridpoints[i][j].x  ;
	    lines[count].y1   = gridpoints[i][j].y  ;
	    lines[count].x2   = gridpoints[i+1][j].x;
	    lines[count++].y2 = gridpoints[i+1][j].y;

	    if (count == MAXLINES-1){
		XDrawSegments(d, w, gc, lines, MAXLINES);
		count = 0;
	      }
	  }
	if (j < ny-1)  /* Draw vertical lines upto gridsize-1  */
	  {
	    lines[count].x1   = gridpoints[i][j].x  ;
	    lines[count].y1   = gridpoints[i][j].y  ;
	    lines[count].x2   = gridpoints[i][j+1].x;
	    lines[count++].y2 = gridpoints[i][j+1].y;

	    if (count == MAXLINES-1){
		XDrawSegments(d, w, gc, lines, MAXLINES);
		count = 0;
	      }
	  }
      }
  if (count > 0){                              /* Draw the remaining */
    XDrawSegments(d, w, gc, lines, count);
    count = 0;
  }
}
	
/* ---------------------------------------------------------------------- */

void display_activity(d, map, nx, ny)
My_Display *d         ;
struct unitdata map[maxnx][maxny];
int nx,ny;
{ 
  extern Pixel newinput_color;          /* Set in display_init()  */
  int color_index, i, j;
  int x0,y0,x1,y1     ;                 /* The corners of each small square  */
  double maxsqx,maxsqy;
  int sqx, sqy        ;                 /* The size of the small rectangles  */
  double accumulate_x, accumulate_y;    /* To add up the x and y             */
  int count=0         ;
  int markersize      ;                 /* Marker for max.act neuron         */
  extern Widget net_act_drawingA ;
  Dimension width=0, height=0;

  XtVaGetValues(net_act_drawingA,XmNwidth,&width,
		    XmNheight,&height,NULL);
  
  /* Leave two pixels on each side for drawing the border lines */
  maxsqx = ((double)width-2.0)/(double)nx ; 
  maxsqy = ((double)height-2.0)/(double)ny;
  sqx    = (int)ROUND(maxsqx); sqy = (int)ROUND(maxsqy);
  markersize = (int)ROUND(maxsqx/4.0+1.0);

  XClearWindow(d->net_act.d, d->net_act.w);

  x0 = 1; accumulate_x = 1.0; /* Starting pixel : pixel 1 (0 for border)  */
  for( i=0; i < nx; i++)
    {
      y0 = 1;accumulate_y=1.0      ;
      accumulate_x += maxsqx       ;
      x1 = (int) ceil(accumulate_x);
      for(j= 0; j < ny; j++)
	{
	  color_index = (int)ROUND( (map[i][j].value *
				     (double)(Activity_Color_Range)) );
	  accumulate_y += maxsqy       ;
	  y1 = (int) ceil(accumulate_y);

	  XSetForeground(d->net_act.d, d->net_act.gc, 
			 colors[color_index].pixel) ;
	  XFillRectangle(d->net_act.d, d->net_act.w, d->net_act.gc, 
			 x0, (height-y1), sqx, sqy) ;

	  y0 = y1;
	}
      x0 = x1;
    }

  draw_border(d->net_act.d, d->net_act.w, d->net_act.gc,
	      width,height,d->net_act.fg);
  XFlush(d->net_act.d);
}	    

/* ---------------------------------------------------------------------- */

void draw_border(d, w, gc, width, height, fg)
Display *d; Window w;GC gc;
int width,height; Pixel fg;
{
  XSegment lines[4]; int count = 0;

  XSetForeground(d, gc, fg);
  lines[count].x1 = 0      ; lines[count].y1   = 0       ;
  lines[count].x2 = width-1; lines[count++].y2 = 0       ;
  lines[count].x1 = width-1; lines[count].y1   = 0       ;
  lines[count].x2 = width-1; lines[count++].y2 = height-1;
  lines[count].x1 = width-1; lines[count].y1   = height-1;
  lines[count].x2 = 0      ; lines[count++].y2 = height-1;
  lines[count].x1 = 0      ; lines[count].y1   = height-1;
  lines[count].x2 = 0      ; lines[count++].y2 = 0       ;
  XDrawSegments(d, w, gc, lines, 4) ;
}
/* ---------------------------------------------------------------------- */
