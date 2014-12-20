/* ---------------------------- File :xdisplay.c ---------------------------*
 *
 * XWindow display for sofm. Author: Joseph Sirosh. Sept 19, 1993.
 * 
 * 09 Oct 1997 James A. Bednar  Updated to work with Motif 2.0 and Linux:
 * 
 * Replaced all calls to XmStringCreateSimple with calls to
 * XmStringCreateLocalized.  The previous routine has been considered deprecated 
 * since Motif version 1.2, and when compiled under Motif 2.0 the call to it
 * in display_message() was crashing.  The corresponding calls to XtFree
 * were also replaced with calls to XmStringFree to match requirements for 
 * XmStringCreateLocalized.
 *
 * Added explicit typecasts in the remaining calls to XtFree to supress
 * compiler warnings.
 *
 * Fixed missing comment character on prototype for display_init(); it had
 * previously been commenting out the prototype for setup_disp_struct()
 * accidentally.
 *
 * Added better color_range handling to prevent crashes on 24-bit displays.
 *
 *
 *
 * ------------------------------------------------------------------------ */

#include "sofmdefs.h"         /* Includes Xlib.h, Xutil.h, Xm.h, 
                               * stdio.h and math.h                         */
#define MAXARGS    30
/* -------------------------  Global Variables  --------------------------- */

extern My_Display disp        ;
extern Widget top_level       ;    
extern char simufile[100]     ;
extern int continuing         ;
extern int Training_Start     ;
extern int Training_Done      ;
extern int t, startt,nx,ny    ;
extern int init_done          ;
extern Blackboard blackboard  ;
extern Input_history *input_list;
extern XColor colors[255]     ; /* Max 256 colors                           */

/* ------------------------ Function Prototypes --------------------------- */

extern int Xtraining()        ; /* The work procedure                       */
extern int init_params()      ; /* For use in loading. In main.c            */
extern void free_inptlist()   ;
extern int iterate_snapshots();
extern void init_weights()    ;
extern void display_net()     ;
extern void display_activity();
extern void display_input()   ;

/* ------------------------------------------------------------------------ */
void setup_disp_struct()      ;
void quit_action()            ;
void load_file_action()       ;
void read_filename()          ;/*Reads text from command widget to load_file */
void stop_action()            ;
void continue_action()        ;
void restart_action()         ;
void command_action()         ;
void command_complete()       ; /* Completes the command entered            */
void list_action()            ;
void handle_expose()          ;
XmString Create_Compound_Str();
void create_colormap()        ;
void display_init()           ; /* Sets up the display */
void setup_disp_struct()      ; /* Initializes the display struct disp      */
XmString *xmget_list()        ; /* Returns list of parameter names & values */ 
void Clear_All_Windows()      ; /* Clear and generate expose events         */
void display_message()        ; /* Displays a message in command window     */

int Actual_Color_Range        ; /* Actually allocated no. of colors         */
int Activity_Color_Range      ; /* Colors allocated for activity display    */
int Input=INPUT, Net = NET, Net_Act = NET_ACT;
Pixel newinput_color          ; /* color for new inputs used in xdraw.c     */
XmString command_prompt       ; /* Prompt for command widget in main window */

Widget main_window,inp_drawingA, net_drawingA,net_act_drawingA;
/* ------------------------------------------------------------------------ */
void display_init(argc, argv)
int argc;
char **argv;
{
  Widget menu_bar, shell1, shell2, shell4,form1,
  filebutton,quitbutton,button1, button2, button3;

  Widget list,command ;

  XtWorkProcId work_id;
  Arg args[MAXARGS]   ;  int i;
  XmStringTable     str_list  ;
  
/* Create the top level shell widget and initialize the toolkit 
 */
  top_level = XtInitialize(argv[0],"XFmrevised", NULL, 0, &argc, argv);
			
/* Initialize the main window widget. 
 */

  main_window = XmCreateMainWindow(top_level, "Main", args, 0);
  XtManageChild(main_window);

/* ---------------------Children of Main Window------------------------- */
 /* Create a form widget for the parameter list and text widget.
 */
  form1 = XmCreateForm(main_window, "Form1",args,0);
  XtManageChild(form1);

/* Create the menubar 
 */
  XtSetArg (args[0], XmNorientation, XmHORIZONTAL          );
  menu_bar = XmCreateRowColumn(main_window,"Menubar",args,1);
  XtManageChild(menu_bar);

/* -------- Create List & command widget for parameter interface ------- */
  
  str_list = xmget_list(&blackboard)                         ; i=0;
  XtSetArg (args[i], XmNitemCount, blackboard.no_of_entries ); i++; 
  XtSetArg (args[i], XmNitems, str_list                     ); i++;
  XtSetArg (args[i], XmNselectionPolicy, XmSINGLE_SELECT    ); i++;
  XtSetArg (args[i], XmNleftAttachment, XmATTACH_WIDGET     ); i++; 
  XtSetArg (args[i], XmNleftOffset, 10                      ); i++; 
  XtSetArg (args[i], XmNrightAttachment, XmATTACH_WIDGET    ); i++;
  XtSetArg (args[i], XmNtopAttachment, XmATTACH_WIDGET      ); i++;
  XtSetArg (args[i], XmNbottomAttachment, XmATTACH_WIDGET   ); i++;
  disp.list_w = list = XmCreateScrolledList(form1, "List", args, i );
  XtAddCallback(list, XmNdefaultActionCallback, list_action, NULL);
  XtManageChild(list); XtFree((char *)str_list);

  command_prompt = XmStringCreateLocalized(" "); i=0 ;
  XtSetArg (args[i], XmNpromptString, command_prompt       ); i++ ; 
  disp.command_w = command = XmCreateCommand(main_window,"Command",args,i);
  XtAddCallback(command, XmNcommandEnteredCallback,command_action, NULL)  ;
  XtAddCallback(command, XmNcommandChangedCallback,command_complete, NULL);
  XtManageChild(command);

/* ---------------RESTART, CONTINUE & STOP buttons---------------------- */

/* Create buttons for the quit, load, stop, continue and restart actions
 */
  i =0;
  XtSetArg (args[i], XmNleftAttachment, XmATTACH_WIDGET     ); i++; 
  XtSetArg (args[i], XmNrightAttachment, XmATTACH_WIDGET    ); i++;
  XtSetArg (args[i], XmNtopAttachment, XmATTACH_WIDGET      ); i++;
  XtSetArg (args[i], XmNbottomAttachment, XmATTACH_WIDGET   ); i++;
  
  quitbutton = XmCreatePushButton(menu_bar,"QUIT", args, i  );
  filebutton = XmCreatePushButton(menu_bar,"FILE", args, i  );

  XtSetArg (args[i], XmNsensitive, False                      ); i++;
  button1    = XmCreatePushButton(menu_bar,"RESTART", args, i );
  button2    = XmCreatePushButton(menu_bar,"CONTINUE", args, i);
  button3    = XmCreatePushButton(menu_bar,"STOP" , args, i   );

  XtAddCallback(quitbutton, XmNactivateCallback,quit_action, NULL)     ;
  XtAddCallback(filebutton, XmNactivateCallback,load_file_action, NULL);
  XtAddCallback(button1,    XmNactivateCallback,restart_action, NULL)  ;
  XtAddCallback(button2,    XmNactivateCallback,continue_action, NULL) ;
  XtAddCallback(button3,    XmNactivateCallback,stop_action, NULL   )  ;

  disp.file_button     = filebutton;  disp.restart_button  = button1;
  disp.continue_button = button2   ;  disp.stop_button     = button3;
 
  XtManageChild(quitbutton);  XtManageChild(filebutton);
  XtManageChild(button1); XtManageChild(button2); XtManageChild(button3);

/* Attach the menubar and other areas to main window. 
 */
  XmMainWindowSetAreas(main_window, menu_bar, command, NULL, NULL, form1);

/* -----------------Other top-level windows------------------------- */
 
  shell1 = XtCreatePopupShell("INPUT",     topLevelShellWidgetClass, 
			      top_level, args, 0);
  shell4 = XtCreatePopupShell("NETWORK",   topLevelShellWidgetClass, 
			      top_level, args, 0);
  shell2 = XtCreatePopupShell("ACTIVITY1", topLevelShellWidgetClass, 
			      top_level, args, 0);

/* Create drawing areas. Use same resize policy for all.
 */
  i=0;
  XtSetArg (args[i], XmNresizePolicy, XmRESIZE_ANY); i++;

  inp_drawingA     = XmCreateDrawingArea (shell1,"INPUTS",args,i);
  net_drawingA     = XmCreateDrawingArea (shell4,"NET",args,i)   ;
  net_act_drawingA = XmCreateDrawingArea (shell2,"NETACT",args,i);

  XtAddCallback(inp_drawingA,     XmNexposeCallback, handle_expose, &Input)  ;
  XtAddCallback(net_drawingA,     XmNexposeCallback, handle_expose, &Net)    ;
  XtAddCallback(net_act_drawingA, XmNexposeCallback, handle_expose, &Net_Act);

  XtManageChild (inp_drawingA)    ; XtManageChild (net_drawingA)    ;
  XtManageChild (net_act_drawingA); 

/* ---------------------------------------------------------------------- */
  XtPopup (shell1, XtGrabNone);  XtPopup (shell2, XtGrabNone);
  XtPopup (shell4, XtGrabNone);
 
/* Realize all widgets 
 */
  XtRealizeWidget(top_level);
  XmUpdateDisplay(top_level);

/* Get the drawing area info and store in display structure.
 */
  create_colormap  (net_drawingA, colors)             ;
  setup_disp_struct(inp_drawingA, &(disp.input))      ;
  setup_disp_struct(net_drawingA, &(disp.net))        ;
  setup_disp_struct(net_act_drawingA, &(disp.net_act));

/* Add a work procedure.
 */
  work_id = XtAddWorkProc((XtWorkProc)Xtraining , NULL);
}
/* ---------------------------------------------------------------------- */
void setup_disp_struct(drawingA, structptr)
Widget drawingA; d_info *structptr;
{
  Arg args[4]; int i     ; Pixel fg, bg  ;  
  Dimension width, height; XGCValues xgcv;

  i=0                                         ;
  XtSetArg(args[i],XmNwidth,  &width ); i++   ;
  XtSetArg(args[i],XmNheight, &height); i++   ;
  XtSetArg(args[i],XmNforeground, &fg); i++   ;
  XtSetArg(args[i],XmNbackground, &bg); i++   ;

  structptr->d     = XtDisplay(drawingA  );
  structptr->w     = XtWindow(drawingA   );
  XtGetValues(drawingA, args, i          );
  structptr->width = width                ;
  structptr->height= height               ;
  structptr->fg = xgcv.foreground  = fg   ;
  structptr->bg = xgcv.background  = bg   ;
  structptr->gc = XtGetGC(drawingA, GCForeground | GCBackground, &xgcv);
}
/* ---------------------------------------------------------------------- */
void create_colormap(w, colors)  
/* Allocates colors in the colormap for the display passed as a parameter.
 * As of now, the default colormap is being used, so the whole application
 * has only one colormap. It does not matter what widget is passed as the
 * parameter.
 */
Widget w          ;
XColor colors[255];
{
 int i, color, argcount=0, rangeby3, multiplier;
 Arg args[1]   ; XColor unused ;
 Colormap colormap             ;
 int planes;
 unsigned int max_colors       ;
 extern Screen *XtScreen();
 int color_range;

 Screen * scr;

 XtSetArg (args[argcount], XmNcolormap, &colormap); argcount++; 
 XtGetValues(w, args, argcount )                  ;

 scr = XtScreen(w);
 planes = scr->root_depth;
 max_colors = 1 << planes;
 switch (max_colors) 
   {
   case   0:
     printf("Error -- couldn't find out the number of bit planes.\n");
     exit(-1);
     break;

   case   2: color_range = 2; break;

     /* The following are divisible by 3 */
   case   4: color_range =   3; break;
   case   8: color_range =   6; break;
   case  16: color_range =  12; break;
   case  32: color_range =  27; break;
   case  64: color_range =  60; break;

     /* This seems like a reasonable color range
	which leaves some for other applications: */
   default:  color_range = 126; break;
   }

 printf("Planes=%d, max_colors=%d, actual_colors=%d\n",planes,max_colors,color_range);
 Actual_Color_Range = color_range-1 ; /* Initialize */ 
 
 if (max_colors==2){
   
   colors[0].pixel = BlackPixelOfScreen(scr);
   colors[1].pixel = WhitePixelOfScreen(scr);
   i = 2;
 }
 else{
   /* Store all the pixel values in the colormap in contiguous locations
    */
   rangeby3   = (int)(color_range/3)  ;
   multiplier = (int)( 65535/rangeby3);
 
   for(i= 0; i< color_range; i++ )   {
     colors[i].green = 0                              ;
     colors[i].blue  = 0                              ;  
     if ( i < rangeby3 )
       colors[i].red = i*multiplier                   ;     
     else{
       colors[i].red = 65535                          ;
       if ( i < 2*rangeby3)
	 colors[i].green = (i-rangeby3) * multiplier  ;
       else{
	 colors[i].green = 65535                      ;
	 colors[i].blue  = (i- 2*rangeby3)*multiplier ;
       }
     }
     colors[i].flags = DoRed | DoGreen | DoBlue       ; 

     if ( XAllocColor(XtDisplay(w), colormap, &(colors[i]) ) == 0 ){
       fprintf(stderr, "Color %d not allocated \n", i);
       Actual_Color_Range--                           ;
     } 
   } /* Now, Actual_Color_Range is the actually allocated color range */
 }
 Activity_Color_Range =  Actual_Color_Range; /* Make more elegant */

 /* Set some color in the middle of the range for the newinput_color */
 newinput_color = WhitePixelOfScreen(scr);
}

/* ---------------------------------------------------------------------- */
void display_parameters(d)
My_Display *d;
{
   XmStringTable     str_list  ;

   /* Replace all items of the list with a new list */
 
  str_list = xmget_list(&blackboard)                ;
  XmListReplaceItemsPos(d->list_w, str_list, 
			blackboard.no_of_entries, 1);
  XtFree((char *)str_list); 
}
/* ---------------------------------------------------------------------- */
XmStringTable xmget_list(blackboard)
Blackboard *blackboard;
{
/* This procedure gets a list of parameter names and parameter values
 * that can be used by the list_widget. It is used to initialize, add
 * and delete.
 */
  int i; char buf[50];
  XmStringTable str_list;

  str_list = (XmStringTable)XtMalloc( blackboard->no_of_entries * 
				     sizeof(XmString*));
  for (i=0; i < blackboard->no_of_entries; i++)
    {
      switch(blackboard->list[i].data_type){
      case CHAR  : sprintf(buf,"%s = %c",blackboard->list[i].param_name,
			   *(blackboard->list[i].data_ptr.c));
	break;
      case INT   : sprintf(buf,"%s = %d",blackboard->list[i].param_name,
			   *(blackboard->list[i].data_ptr.i));
	break;
      case FLOAT : sprintf(buf,"%s = %.6f",blackboard->list[i].param_name,
			   *(blackboard->list[i].data_ptr.f));
	break;
      }
      str_list[i] = XmStringCreateLocalized(buf);
    }
  return(str_list);
}
/* ---------------------------------------------------------------------- */
void quit_action(w, client_data, call_data)
Widget w;
XtPointer client_data;
XmAnyCallbackStruct *call_data;
{
  printf("Exiting: Iteration = %d\n",t);
  XtCloseDisplay(XtDisplay(top_level))  ;
  exit(0);
}
/* ------------------------------------------------------------------------ */
void load_file_action(w, client_data, call_data)
Widget w;
XtPointer client_data;
XmAnyCallbackStruct *call_data;
{
/* Changes the prompt on the command widget. Also, temporarily changes its
 * callback to read_filename(). After reading the file, the previous 
 * callback is reinstated.
 */
  int argc; Arg args[2];

  XmString str = XmStringCreateLocalized("Enter Simulation File Name:");
  XtVaSetValues(disp.command_w,XmNpromptString,str,NULL); XmStringFree(str) ;

  XtSetSensitive(disp.command_w,True);

  XtRemoveCallback(disp.command_w, XmNcommandEnteredCallback,
		   command_action, NULL);
  XtRemoveCallback(disp.command_w, XmNcommandChangedCallback,
		   command_complete, NULL);
  XtAddCallback(disp.command_w,XmNcommandEnteredCallback,read_filename,NULL);

}
/* ------------------------------------------------------------------------ */
void read_filename(w, client_data, cbs) /* Temporary callback for command_w */
Widget w;
XtPointer client_data;
XmCommandCallbackStruct *cbs;
{
  char *value;
  char buf[100]; XmString msg; 

  XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &value);

  fprintf(stderr,"The file is:%s\n", value);

  if (!init_params(value))
    display_message("File Not Found. Load Again!");
  else{
    XtSetSensitive(disp.restart_button , True );
    sprintf(buf,"File %s loaded. Press RESTART to start.",value);
    display_message(buf);
  }

  XtFree(value);

  /* Reset the previous prompt and callbacks of the command widget 
   */
  XtVaSetValues(disp.command_w, XmNpromptString, command_prompt, NULL);
  XtRemoveCallback(disp.command_w, XmNcommandEnteredCallback,
		   read_filename, NULL);
  XtAddCallback(disp.command_w,XmNcommandEnteredCallback,command_action,NULL);
  XtAddCallback(disp.command_w,XmNcommandChangedCallback,command_complete,NULL);
  XtSetSensitive(disp.command_w, True);
}

/* --------------------- Callback for STOP button ------------------------ */
void stop_action(w, client_data, call_data)
Widget w;
XtPointer client_data;
XmAnyCallbackStruct *call_data;
{
  int i;

  Training_Start = False;   /* Start flag for work procedure */
  Training_Done  = False;
  XtSetSensitive(disp.stop_button    , False);
  XtSetSensitive(disp.continue_button, True );
  XtSetSensitive(disp.restart_button , True );
  XtSetSensitive(disp.command_w      , True );
  XtSetSensitive(disp.list_w         , True );
  XmUpdateDisplay(top_level                 );
}
/* ----------------- Callback for CONTINUE button ------------------------ */
void continue_action(w, client_data, call_data)
Widget w;
XtPointer client_data;
XmAnyCallbackStruct *call_data;
{
  Training_Start = True ;   /* Start flag for work procedure */
  Training_Done  = False;

  XtSetSensitive(disp.stop_button    , True );
  XtSetSensitive(disp.continue_button, False);
  XtSetSensitive(disp.restart_button , False);
  XtSetSensitive(disp.command_w      , False);
  XtSetSensitive(disp.list_w         , False);
  XmUpdateDisplay(top_level                 );
}
/* ------------------- Callback for RESTART button ----------------------- */
void restart_action(w, client_data, call_data)
Widget w;
XtPointer client_data;
XmAnyCallbackStruct *call_data;
{
/* This function starts training from the first iteration. It clears all 
 * windows and starts on a clean slate.
 */
  char msg[100];
  extern char simufile[100];

  free_inptlist()    ;
  Clear_All_Windows();
  init_params(simufile);
  if (continuing){ 
    if (!iterate_snapshots()){          /* File problem   */
      sprintf(msg, "File: %s. Loading Problem!", simufile);
      display_message(msg);
      return;
    }
  }
  else init_weights()   ;
  t = startt            ; 
  Training_Start = True ;   /* Start flag for work procedure */
  Training_Done  = False;

  /* Set the stop and continue buttons sensitive and the restart button 
   * insensitive.
   */
  XtSetSensitive(disp.stop_button    , True );
  XtSetSensitive(disp.continue_button, False);
  XtSetSensitive(disp.restart_button , False);
  XtSetSensitive(disp.command_w      , False);
  XtSetSensitive(disp.list_w         , False);
  XmUpdateDisplay(top_level                 );
}
/* ------------------- Callbacks for command widget ---------------------- */
void command_action(w, client_data, call_data)
Widget w;
XtPointer client_data;
XmCommandCallbackStruct *call_data;
{
}
/* ----------------------------------------------------------------------- */
void command_complete(w, client_data, call_data)
Widget w;
XtPointer client_data;
XmCommandCallbackStruct *call_data;
{
}
/* ---------------------- Callback for list widget ----------------------- */
void list_action(w, client_data, call_data)
Widget w;
XtPointer client_data;
XmListCallbackStruct *call_data;
{
}
/* ---------------------- Expose: Clear & redraw.  ----------------------- */

void handle_expose(w, client_data, call_data)
Widget w;
XtPointer client_data;
XmDrawingAreaCallbackStruct *call_data;
{
  extern int maxact_i, maxact_j;
  XEvent *p_event = call_data->event;

  if(init_done)                        /* If initialization has been done */
    if (p_event->xexpose.count == 0)          /* If last expose event */
      if (Training_Start || !Training_Done) 
	/* Refresh only if training was started or is going on.
	 */
	switch( *((int*)client_data)){

	case INPUT  : display_input(&disp,input_list)         ; break; 
 	case NET    : display_net(&disp, units, nx, ny)      ; break;
	case NET_ACT: display_activity(&disp, units, nx, ny)     ; break;
	}
  XmUpdateDisplay(w);  
}
/* ------------------------- Clear all windows  -------------------------- */
void Clear_All_Windows()
{
/* This function uses XClearArea which both clears the area and generates
 * an expose event. The expose event is handles by an expose_action that
 * redraws the display.
 */
  XClearArea(disp.input.d  , disp.input.w  ,0,0,0,0,True);
  XClearArea(disp.net.d    , disp.net.w    ,0,0,0,0,True);
  XClearArea(disp.net_act.d, disp.net_act.w,0,0,0,0,True);
}
/* ---------------------------------------------------------------------- */
void display_message(str)
char *str;
{
  /* Displays The Message In The History Area Of The Command Widget.
   */
  XmString msg;

  msg = XmStringCreateLocalized(str);
  XmCommandError(disp.command_w,msg);    /* Show Error Message */
  XmStringFree(msg); 
}
/* ---------------------------------------------------------------------- */
void wait_for_buttonpress()
{
  int notpressed;
  XEvent thevent;
  
  notpressed = True;

  while (notpressed){
    XNextEvent(XtDisplay(main_window), &thevent);
    if ((thevent.type == ButtonPress)) notpressed = False;
  }
}
/* ---------------------------------------------------------------------- */	  
    
