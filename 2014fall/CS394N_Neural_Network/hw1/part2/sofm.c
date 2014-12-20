#include "sofm.h"
/* ---------------------------- File: sofm.c      ---------------------------*
 *
 * 5 Oct 2002 Marshall R. Maybery, III - Added SWIG Gui to sofm code.
 * 
 * ------------------------------------------------------------------------ */
//#include "sofmdefs.h"
/* ---------------------------- File: main.c      ---------------------------*
 *
 * 09 Oct 1997 James A. Bednar  Replaced "displaying" param with "disp_rate"
 * 
 * The displaying parameter was not very useful, since it only allowed
 * either all iterations or no iterations to be displayed.  It has
 * been generalized to disp_rate, which has the same behavior as the
 * previous displaying parameter for the former's valid inputs of 0 and
 * 1 (i.e. no display and always display, respectively), while also
 * allowing any other integer n, which specifies display every n iterations.
 *
 *
 * ------------------------------------------------------------------------ */


/*  These routines are rendered unnecessary with the SWIG GUI */
/*************** display routines *************************/
 
/* alphanumeric output routines */
/*
  display_best()
  {
    register int i,j,k;
    int more;
    printf("\nIteration %d\n",t);
    for(j=0; j<ny; j++)
      printf("----");
    printf("\n");
    for(i=0; i<nx; i++)
      {
        more=1;
        for(k=1; more==1; k++)
	  {		    
            more=0;
	    for(j=0; j<ny; j++)
	      {
	        printf("%2s |",
 		     (units[i][j].labelcount >= k &&
	 	      units[i][j].labels[k]<nvectors)?
		      vectors[units[i][j].labels[k]].label:
		      ((k==1)?"  ":""));
 	      if(units[i][j].labelcount>k) more=1;
	     }
	   printf("\n");
 	}
       for(j=0; j<ny; j++)
 	printf("----");
       printf("\n");
     }
 }

  main(argc,argv)
  int argc; char *argv[];
  { 
    init_params(argv);
    read_inputs();
    if (continuing) iterate_snapshots();
    else init_weights();
    training();
   exit(0);
  } 
*/

finish() {
  save_current(tend);
}

iterate_snapshots()
/* go through the saved snapshots, displaying the map */
{
  register int i;
  int randfun();

  fp=fopen(simufile,"r");
  read_params(fp);
  oldtesting=testing;
  testing=1;		/* we don't want to change weights */
  oldt=0;
  iterate_weights(randfun); /* just to update rand */
}

read_snapshot()
/* go through the snapshot, displaying the map */
{
  register int i;
  int readfun();

  if (fp == NULL) return;
  if (fscanf(fp,"%d",&t)!=EOF) /* read the t */
    {
      /* update rand and parameters */
      startt=t+1;
      for(i=0; i<startt-oldt; i++) shuffle();
      oldt=startt;
      
      /* read the current weights */
      iterate_weights(readfun);
      /* Display only if there is some valid disp_rate */
      if (disp_rate != 0) present_inputs(t);
    }
  else {
    fclose(fp);
    for (nextsnapshot=0; t >= snapshots[nextsnapshot]; nextsnapshot++){}
    testing=oldtesting;
  }
}

present_inputs(int t)
{
  register int i;
  float alpha();
  while (t>t_[phase]) phase++;
  init_units();
  for(i=0; i<nvectors; i++)
    {
      compute_responses(shuffletable[i]);
      if (!testing)
	modify_weights( nc(t), alpha(t), shuffletable[i]);
    }
  if( !testing ) {
    if (t >= snapshots[nextsnapshot]) save_current(t);
    shuffle();
  }
  /* Display only this often, and only if nonzero */
  if ( (disp_rate != 0) && (t%disp_rate == 0)) return 1;
  return 0;
}

/*********************  feature map ******************************/

compute_responses(int inpv)
{
  register int i,j;
  float distance();
  float best=MAXDISTANCE;
  for(i=0; i<nx; i++)
    for(j=0; j<ny; j++)
      {
	units[i][j].value = distance(vectors[inpv].comp, units[i][j].comp,dim);
	/* check if this unit's response is best so far encountered */
	if (units[i][j].value < best)
	  {
	    besti=i; bestj=j; best=units[i][j].value;
	  }
	if (units[i][j].value < units[i][j].bestvalue){
	  units[i][j].bestvalue = units[i][j].value;
	  units[i][j].labels[0] = inpv;
	}
      }
/*  printf("%s : %f\n", vectors[inpv].label, best);*/
  units[besti][bestj].labelcount=units[besti][bestj].labelcount+1;
  units[besti][bestj].labels[ units[besti][bestj].labelcount ] = inpv;
}

modify_weights(int nc, float alpha, int inpv)
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
	units[i][j].comp[k] +=
	  alpha*(vectors[inpv].comp[k]-units[i][j].comp[k]);
}

/*********************  initializations ******************************/

init_params(char *arg)
{
  int c;

  /* we want to use index -1 to refer to initial values */
  t_=t_array+1; nc_=nc_array+1; alpha_=alpha_array+1;

  sprintf(simufile, "%s", arg);
  fp=fopen(simufile,"r");
  read_params(fp);
  if((c=getc(fp))==EOF) continuing=0;
  else continuing=1;
  fclose(fp);

  srand48(seed);		        /* start random number sequence */
  t=(-1);
  startt=phase=t_[-1]=0;
}

read_params(FILE *fp)
{
  char s[100];
  register int i,phase;
  int nphase;
  /* simulation parameters */
  fscanf(fp,"%s", vectorfile); fgets(s,99,fp);
  fscanf(fp,"%d %d", &nx, &ny); fgets(s,99,fp);
  fscanf(fp,"%d %d", &disp_rate,&seed); fgets(s,99,fp);
  fscanf(fp,"%d %d %d %f", &tend, &nphase, &nc_[-1], &alpha_[-1]);
  fgets(s,99,fp);

  for(phase=0; phase<nphase; phase++)
    {
      fscanf(fp,"%d %d %f", &t_[phase], &nc_[phase],&alpha_[phase]);
      fgets(s,99,fp);
    }
  
  /* saving info */
  fscanf(fp,"%d", &snapshots[0]);
  for(i=0; i<MAXSNAPS && snapshots[i]<SNAPSHOTEND; i++)
    fscanf(fp,"%d", &snapshots[i+1]);
  fgets(s,99,fp);
  nextsnapshot=0;
}

read_inputs()
{
  register int i,j;
  char s[100];

  fp=fopen(vectorfile,"r");

  /* read labelfilename and vector dimension */
  fscanf(fp,"%s", labelfile); fgets(s,99,fp);
  fscanf(fp,"%d", &dim); fgets(s,99,fp);

  /* read the labels */
  fp2=fopen(labelfile,"r");
  for(i=0; i<MAXVECTORS && fscanf(fp2,"%s", vectors[i].label)!=EOF; i++);
  nvectors=i;
  fclose(fp2);

  /* read the vectors */
  for(i=0; i<nvectors; i++)
    for(j=0; j<dim; j++)
      fscanf(fp,"%f",&vectors[i].comp[j]);

  for(i=0; i<nvectors; i++)
    shuffletable[i]=i;
  shuffle();
}

init_weights()
{
  int randfun();
  iterate_weights(randfun);
  if (snapshots[0]==(-1))
    save_current(t);			/* save the initial state */
}

init_units()
{
  register int i,j;
  for(i=0; i<nx; i++)
    for(j=0; j<ny; j++) 
      {
	units[i][j].bestvalue = 999999999.9;
	units[i][j].labelcount= 0;
      }
}

/*********************** I/O etc functions *****************/ 

save_current(int t)
{
  int writefun();
  fp=fopen(simufile,"a");
  fprintf(fp,"%d\n", t);
  iterate_weights(writefun);
  fclose(fp);
  nextsnapshot++;
}

iterate_weights(int (*dofun)())
{
  register int i,j,k;

  for(i=0; i<nx; i++)
    for(j=0; j<ny; j++)
      for(k=0; k<dim; k++)
        (*dofun)(&units[i][j].comp[k]);

}  

shuffle()
{
  register int i;
  int temp1,temp2,temp3;
  for(i=0; i<nvectors; i++)
    {
      temp1=lrand48()%nvectors;
      temp2=lrand48()%nvectors;
      temp3=shuffletable[temp1];
      shuffletable[temp1]=shuffletable[temp2];
      shuffletable[temp2]=temp3;
    }
}

readfun(float *place)
{
  fscanf(fp,"%f", place);
} 

writefun(float *place)
{
  fprintf(fp,"%f\n", *place);
} 

randfun(float *place)
{
  float f01rnd();
  *place = f01rnd();
} 

      
/***********************  math stuff **************************/

float distance(float *v1, float *v2, int ncomp)
{
  float sum=0.0;
  register int i;
  /* unit response is equal to the euclidian distance of input and weight */
  for(i=0; i<ncomp; i++)
    sum=sum+(v1[i]-v2[i])*(v1[i]-v2[i]);
  return(sqrt(sum));
}

float alpha(int t)
{
  /* decrease gain linearly within the phase */
  return(alpha_[phase-1] + (alpha_[phase]-alpha_[phase-1])*
	 (t-t_[phase-1])/(t_[phase]-t_[phase-1]));
}

nc(int t)
{
  /* decrease neighborhood size linearly within the phase */
  return(floor( nc_[phase-1] + 0.999999 + ((float) nc_[phase]-nc_[phase-1]) *
		(t-t_[phase-1])/(t_[phase]-t_[phase-1]))
	 );
}
 
float f01rnd()
{
  /* random float between 0 and 1 */
  return (drand48());
}
