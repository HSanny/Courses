/* ----------------------------- File : sofmdefs.h ------------------------- *
 * XWindow & batch code for Self-organizing feature maps. Header file.
 * Authors: Risto Miikkulainen, Joseph Sirosh. Sept 19, 1993.
 * ------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifdef XWINDOW                       /* Include these only if X is defined.  */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/Command.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/RowColumn.h>
#endif

#define True            1
#define False           0
#define CHAR            0
#define INT             1
#define FLOAT           2
#define ROUND(a)        (floor( (0.5 + (a))))

#define INPUT    1
#define NET      2
#define NET_ACT  3

#define snapshotend 999999999
#define maxdistance 1.414213562

#define maxnx 1000            /* maximum network size is nx*ny units */
#define maxny 1000
#define maxsnaps 100         /* maximum number of snapshots */
#define maxphase 100
#define maxdim 2             /* maximum dimension of input vectors */

#define iboxs 0.001            /* size of input dots */
#define ciboxs 0.001            /* size of current input dot */
#define vcenter low+(high-low)/2.0
#define hcenter left+(right-left)/2.0
#define frame_color 0          /* 0 = red in color, black in bw */
#define net_color 1            /* 1 = green in color, black in bw */
#define data_color 1           /* 1 = green in color, black in bw */
#define clear_color cmapsize-1 /*   = black in color, white in bw */
#define old_input_color 2+2*cmapsize/3  /* = is red in color, grey in bw */
#define current_input_color 2  /* 2 = white in color, black in bw */

struct inputvectors {
  float comp[maxdim];
} vectors,oldvectors;

struct unitdata {
  float value;
  float comp[maxdim];
} units[maxnx][maxny];

/* random number functions */
extern double drand48();
extern void srand48();

/* Structure for a neat interface with display routines. 
 * Do not bother if you are a novice.
 */
typedef struct board             /* Communication board for User Interface */
{ 
  char param_name[40];           /* Name of parameter.                     */
  int data_type      ;           /* Param data_type:CHAR=0,INT=1,FLOAT=2   */
  int update_flag    ;           /* If set, the data must be updated by UI */
  union t{
    char   *c        ;           /* This union holds the addresses of the..*/
    int    *i        ;           /* associated data item. The locations .. */
    float  *f        ;           /* they point to can be changed by the UI */
  }data_ptr          ;
} ParamInterface     ;

typedef struct b{                /* The parameter blackboard               */
  int no_of_entries      ;
  ParamInterface list[50];
}Blackboard              ;
/* -------------------- Display definitions. ----------------------------- */
#ifdef XWINDOW  

#define XtPointer caddr_t        /* Compatibility's sake                   */
#define XmStringCreateSimple(a) XmStringLtoRCreate((a),XmSTRING_DEFAULT_CHARSET)

typedef struct inpt              /* To store previous input vectors.       */
{
  double *input_vector;
  struct inpt *next   ;
}Input_history        ;

typedef struct info              /* Display window info                    */
{
  Display *d ;
  Window   w ;
  GC      gc ;
  Pixel fg,bg;
  int     width, height; 
}d_info;

typedef struct d                 /* The current display configuration     */ 
{
  d_info  input,                 /* Input window -random dots in 2d space */
          net,                   /* 2d net display                        */
          net_act;               /* net activity pattern display          */

  XColor  colors[255];           /* Colormap of the application. Max 255  */
  Widget  stop_button,           /* Other widgets the application needs...*/
      continue_button,           /* to know about. Mostly to set the...   */
      restart_button ,           /* sensitivity.                          */
      file_button    ,
      command_w      ,           /* For filenames & changing parameters   */
      list_w         ;           /* For displaying parameters & values    */
} My_Display         ;

#endif
/* ----------------------------------------------------------------------- */
