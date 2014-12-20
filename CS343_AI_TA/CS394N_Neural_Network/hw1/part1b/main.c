/* ----------------------------- File : main.c ----------------------------- *
 * XWindow & batch code for Self-organizing feature maps.
 * Authors: Risto Miikkulainen, Joseph Sirosh. Sept 19, 1993.
 * 
 * ------------------------------------------------------------------------- */

#include "sofmdefs.h"

extern void display_message();
extern void wait_for_buttonpress();
void add2history();
void setup_blackboard();
void print_params();
char* alloce();

Blackboard blackboard       ;    /* The actual blackboard structure          */

#ifdef XWINDOW
My_Display disp          ;       /* The current display                      */
XColor colors[255]       ;       /* Colors list of the application           */
Widget top_level         ;       /* Top_level widget of the application      */
Input_history *input_list=NULL;  /* The list of input vectors                */

/* Flags set by X Application to start or continue training. The continue
 * button only sets the flag true - it does not initialize the starting
 * iteration value startt. The restart action initializes startt to 0, and
 * also reinitializes the simulation file. The stop action sets Training_Start
 * to zero so that training is stopped. Training_Done becomes true only at the
 * end of training.
 */
int Training_Start=False ,       /* Default values                        */
    Training_Done =True  ;
int init_done=False;
#endif

/* in t_, nc_, and alpha_: [-1] = initial value */
int nx, ny, t, t_array[maxphase+1], *t_, tend, seed,
  nc_array[maxphase+1], *nc_,besti, bestj, nextsnapshot=0, snapshots[maxsnaps],
  continuing, cmapsize, startt=0, phase=0, dim, disp_rate=1;
int neighborhood=0; float alpha_val=0.0;

float alpha_array[maxphase+1], *alpha_, low, left, high, right; 
char simufile[100], cmapfile[100], sb_outdev[100], sb_outdriver[100];
FILE *fp,*fp2;

/*********************  main control ************************/

main(argc,argv)
int argc; char *argv[];
{
  setup_blackboard(&blackboard); /* Just a parameter interface structure  */

#ifdef XWINDOW
  display_init(1, argv);    /* Set up all the widgets and menus    */
  XtMainLoop ()        ;    /* Start the main event-handling loop. */
  exit(0)              ;    /* Exit at the end of the application.
			     * Control never goes beyond this line if 
			     * displaying.
			     */
#endif
/* The following functions are for doing the computation without a display.
 * The calls to the display inside these functions are included only if XWINDOW
 * is defined at compilation time.
 */ 
  init_params(argv[1]);
  if (continuing) iterate_snapshots();
  else init_weights();
  training();
}
/* ---------------------------------------------------------------------- */
int iterate_snapshots()
/* go through the saved snapshots, displaying them on the screen */
{
  register int i;
  int oldt, readfun(), randfun();
  float alpha();

  if ( (fp=fopen(simufile,"r"))==NULL) return(0);
  read_params(fp);
  oldt=0;
  iterate_weights(randfun); /* just to update rand */
  while (fscanf(fp,"%d",&t)!=EOF) /* read the t */
    {
      /* update rand and parameters */
      startt=t;
      for(i=0; i<startt-oldt; i++)  /* Generate t-oldt-1 input points */
	{ 
	  generate_input();
#ifdef XWINDOW
	  add2history(oldvectors);   /* Add points to the ptlist to be displayed */
#endif
	}
      oldt=startt;
      
      /* read the current weights */
      iterate_weights(readfun);
      generate_input();    /* Create the latest input pt (to be displayed large)*/

#ifdef XWINDOW
      add2history(oldvectors); /* Add last point to history list  */
#endif

      present_inputs();     /* we'll have to modify weights during last snap*/

#ifdef XWINDOW
      display_message("Press a mouse button in any window for next snapshot");
      XmUpdateDisplay(top_level);
      XFlush(XtDisplay(top_level));
      wait_for_buttonpress();
#endif
    }
  fclose(fp);
  for (nextsnapshot=0; t >= snapshots[nextsnapshot]; nextsnapshot++){}
  return(1);
}

/* ---------------------------------------------------------------------- */     
training()
{
  for(t=startt; t<=tend; t++)
    {
      generate_input();
      if (t >= snapshots[nextsnapshot]) save_current();
      present_inputs();
    }
}
/* ---------------------------------------------------------------------- */
#ifdef XWINDOW
int Xtraining(c_data)  
XtPointer c_data;
/* This work proc does the computation if there are no events to be processed.
 * For a work procedure to be called repeatedly, you must return false all the
 * time. 
 */
{
  char msg[100];
  int i;

  XmUpdateDisplay(top_level);
  if (Training_Start && !Training_Done)
    {
      if (t > tend){
	Training_Done=True                ;  /* End of training */

	XtSetSensitive(disp.stop_button    , False);
	XtSetSensitive(disp.continue_button, False);
	XtSetSensitive(disp.restart_button , True );
	XtSetSensitive(disp.command_w      , True );
	XtSetSensitive(disp.list_w         , True );
	XFlush(XtDisplay(top_level)               );

	printf("Training complete. Last Iteration = %d\n", t-1);
      }
      for (i=0; (i<disp_rate) && (t<=tend); i++, t++){
	generate_input()                    ;
	add2history(oldvectors)              ;
	if (t >= snapshots[nextsnapshot]) save_current();
	present_inputs()                    ;
      }
    }
  return(False);
}
#endif
/* ---------------------------------------------------------------------- */
present_inputs()
{
  float alpha();
  while (t>t_[phase]) phase++;



#ifdef XWINDOW
  if (t%disp_rate ==0){  /* Display only this often */
    display_parameters(&disp)         ;
    display_net(&disp, units, nx,ny)    ;
    display_input(&disp, input_list)  ; 
  } 
#endif

  compute_responses();

#ifdef XWINDOW
  if (t%disp_rate ==0)
    display_activity(&disp, units, nx,ny);
#endif

  neighborhood = nc(t); alpha_val = alpha(t);
  modify_weights( neighborhood, alpha_val);
}

/*********************  feature map ******************************/

compute_responses()
{
  register int i,j;
  float distance();
  float best=maxdistance;
  for(i=0; i<nx; i++)
    for(j=0; j<ny; j++)
      {
	units[i][j].value = distance(vectors.comp, units[i][j].comp,dim);
	/* check if this unit's response is best so far encountered */
	if (units[i][j].value < best)
	  {
	    besti=i; bestj=j; best=units[i][j].value;
	  }
      }
}
/* ---------------------------------------------------------------------- */
modify_weights(nc,alpha)
int nc;
float alpha;
{
  register int i, j, k;
  int lowi, highi, lowj, highj;
  /* modify weighs toward the input in a neighborhood of the maximally 
     responding unit */
  lowi=besti-nc;  if (lowi<0) lowi=0;
  highi=besti+nc; if (highi>=nx) highi=nx-1;
  lowj=bestj-nc; if (lowj<0) lowj=0;
  highj=bestj+nc; if (highj>=ny) highj=ny-1;
  for(i=lowi; i<=highi; i++)
    for(j=lowj; j<=highj; j++)
      for(k=0; k<dim; k++)
	units[i][j].comp[k] += alpha*(vectors.comp[k]-units[i][j].comp[k]);
}

/*********************  initializations ******************************/

int init_params(argv)
char *argv;
{
  register int i;
  int c;

  /* we want to use index -1 to refer to initial values */
  t_=t_array+1; nc_=nc_array+1; alpha_=alpha_array+1;
   
  sprintf(simufile, "%s", argv);
  if ((fp=fopen(simufile,"r"))==NULL) {
    printf("File %s not loaded. \n",argv);
    return(0);
  }
  read_params(fp);
  if((c=getc(fp))==EOF) continuing=0;
  else continuing=1;
  fclose(fp);
  srand48(seed);		        /* start random number sequence */
  t=(-1);
  startt=phase=t_[-1]=0;
  dim=maxdim;
  for(i=0; i<dim; i++)
    vectors.comp[i]=maxdistance;	/* previous input outside the screen */
  print_params(stderr, &blackboard);    /* Echo parameters to stderr */

#ifdef XWINDOW
  init_done = True;
#endif
  return(1);
}
/* ---------------------------------------------------------------------- */
read_params(fp)
FILE *fp;
{
  char s[100];
  register int i, phase;
  int nphase;
  /* simulation parameters */
  fscanf(fp,"%f %f %f %f", &left,&low,&right,&high); fgets(s,99,fp);
  fscanf(fp,"%d %d", &nx, &ny); fgets(s,99,fp);
  fscanf(fp,"%d %d", &disp_rate,&seed); fgets(s,99,fp);
  fscanf(fp,"%d %d %d %f", &tend, &nphase, &nc_[-1], &alpha_[-1]);
  fgets(s,99,fp);

  for(phase=0; phase<nphase; phase++)
    {
      fscanf(fp,"%d %d %f", &t_[phase], &nc_[phase],&alpha_[phase]);
      fgets(s,99,fp);
    }
  neighborhood=nc_[-1]; 
  alpha_val=alpha_[-1]; /* Initializations for proper param display */

  /* saving info */
  fscanf(fp,"%d", &snapshots[0]);
  for(i=0; i<maxsnaps && snapshots[i]<snapshotend; i++)
    fscanf(fp,"%d", &snapshots[i+1]);
  fgets(s,99,fp);
  nextsnapshot=0;
}
/* ---------------------------------------------------------------------- */
init_weights()
{
  int randfun();
  iterate_weights(randfun);
}


/*********************** I/O etc functions *****************/ 

save_current()
{
  int writefun();
  fp=fopen(simufile,"a");
  fprintf(fp,"%d\n", t);
  iterate_weights(writefun);
  fclose(fp);
  nextsnapshot++;
}
/* ---------------------------------------------------------------------- */
iterate_weights(dofun)
int (*dofun)();
{
  register int i,j,k;

  for(i=0; i<nx; i++)
    for(j=0; j<ny; j++)
      for(k=0; k<dim; k++)
        (*dofun)(&units[i][j].comp[k]);

}  
/* ---------------------------------------------------------------------- */
readfun(place)
float *place;
{
  fscanf(fp,"%f", place);
} 
/* ---------------------------------------------------------------------- */
writefun(place)
float *place;
{
  fprintf(fp,"%f\n", *place);
} 
/* ---------------------------------------------------------------------- */
randfun(place)
float *place;
{
  float f01rnd();
  *place = f01rnd();
} 

      
/***********************  math stuff **************************/

float distance(v1, v2, ncomp)
int ncomp;
float v1[],v2[];
{
  float sum=0.0;
  register int i;
  /* unit response is equal to the euclidian distance of input and weight */
  for(i=0; i<ncomp; i++)
    sum=sum+(v1[i]-v2[i])*(v1[i]-v2[i]);
  return(sqrt(sum));
}
/* ---------------------------------------------------------------------- */
float alpha(t)
int t;
{
  /* decrease gain linearly within the phase */
  return(alpha_[phase-1] +
	 (alpha_[phase]-alpha_[phase-1])*
	 (t-t_[phase-1])/(t_[phase]-t_[phase-1])
	 );
}
/* ---------------------------------------------------------------------- */
nc(t)
int t;
{
  /* decrease neighborhood size linearly within the phase */
  return(floor( nc_[phase-1] + 0.999999 +
	       ((float) nc_[phase]-nc_[phase-1]) *
	       (t-t_[phase-1])/(t_[phase]-t_[phase-1])
	       )
	 );
}
/* ---------------------------------------------------------------------- */ 
float f01rnd()
{
  /* random float between 0 and 1 */
  return (drand48());
}

/* ---------------------------------------------------------------------- *
 *                        Housekeeping Routines                           *
 * ---------------------------------------------------------------------- */

#ifdef XWINDOW
void add2history(raw_input) /* The latest raw_input is at head of list */
struct inputvectors raw_input;
{
  Input_history *new; int i;

  new = (Input_history*)alloce(sizeof(Input_history));
  new->input_vector = (double*)alloce(sizeof(double)*(2));
  new->next = input_list; /* Attach the head of the input list */
  input_list = new      ;
  
  for(i=0; i<2; i++)
    input_list->input_vector[i] = raw_input.comp[i];
}
/* ------------------------------------------------------------------------ */
void free_inptlist() /* Frees all the input data stored */
{
 Input_history *prev,*current;

 for(prev=current=input_list; current!=NULL; )
   {
     free( (char*)(prev->input_vector));
     current = prev->next;
     free((char*)prev);
     prev=current;
   }
 input_list = NULL;
}
#endif

/* ------------------------------------------------------------------------ */
/*                          Parameter IO routines                           */
/* ------------------------------------------------------------------------ */
void store_in_bboard(blackboard,index,name,type,flag,pointer)
Blackboard *blackboard;char *name; int type,flag; char *pointer;
{
  strcpy(blackboard->list[index].param_name, name);
  blackboard->list[index].data_type  = type       ;
  blackboard->list[index].update_flag    = flag   ;
  
  switch(type){
  case CHAR  : blackboard->list[index].data_ptr.c = pointer        ; break;
  case INT   : blackboard->list[index].data_ptr.i = (int*)pointer  ; break;
  case FLOAT : blackboard->list[index].data_ptr.f = (float*)pointer; break;
  }
  blackboard->no_of_entries++;
}
/* ------------------------------------------------------------------------ */
void setup_blackboard(blackboard)
Blackboard *blackboard;
/* Routine that sets up the interface of SOM with other output routines.
 * The blackboard is used to provide a standard interface to interactive
 * displays, which may vary from one implementation to another. The interface
 * can directly change the variables that are changeable by aliasing through
 * the pointer provided.
 */
{
  blackboard->no_of_entries=0;
  store_in_bboard(blackboard,0,"iterations", INT  ,False,(char*)&t)          ;
  store_in_bboard(blackboard,1,"phase",     INT,  False,(char*)&phase)        ;
  store_in_bboard(blackboard,2,"nc",    INT, False,(char*)&neighborhood)     ;
  store_in_bboard(blackboard,3,"alpha", FLOAT,False,(char*)&alpha_val)       ;
  store_in_bboard(blackboard,4,"tend",       INT  ,True,(char*)&tend)        ;
  store_in_bboard(blackboard,5,"nx",         INT,  True,(char*)&nx)          ;
  store_in_bboard(blackboard,6,"ny",         INT,  True,(char*)&ny)          ;
  store_in_bboard(blackboard,7,"display rate",INT,  True,(char*)&disp_rate)  ;
  store_in_bboard(blackboard,8,"left",       FLOAT,True,(char*)&left)        ;
  store_in_bboard(blackboard,9,"low",        FLOAT,True,(char*)&low)         ;
  store_in_bboard(blackboard,10,"right",      FLOAT,True,(char*)&right)      ;
  store_in_bboard(blackboard,11,"high",       FLOAT,True,(char*)&high)       ;
}
/* ------------------------------------------------------------------------ */
void print_params(fp,blackboard)
FILE *fp; Blackboard *blackboard;
{
/* Prints parameter values to a file from the parameter blackboard.
 */
  int i;

  fprintf(fp,"**** Current Parameters ****\n");
  for(i=0; i< blackboard->no_of_entries; i++){

    fprintf(fp,"%s:",blackboard->list[i].param_name);
    switch(blackboard->list[i].data_type){
    case CHAR :fprintf(fp,"%c\n",*(blackboard->list[i].data_ptr.c)); break;
    case INT  :fprintf(fp,"%d\n",*(blackboard->list[i].data_ptr.i)); break;
    case FLOAT:fprintf(fp,"%f\n",*(blackboard->list[i].data_ptr.f)); break;
    }
  }
  fprintf(fp,"\n");
}
/* ------------------------------------------------------------------------ */
char* alloce(allocsize)/* Allocator with error check for mem unavailability */
int allocsize;
{
  char *new;

  if (( new = malloc(allocsize))==NULL){
    fprintf(stderr,"Not enough memory. Exiting\n");
    exit(0);
  }
  return(new);
}
/* ------------------------------------------------------------------------ */
