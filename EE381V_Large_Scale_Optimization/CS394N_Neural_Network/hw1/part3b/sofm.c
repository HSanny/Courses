#include "sofm.h"

/*********************  main control ************************/

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
  testing=1;				/* we don't want to change weights */
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
      setup_labels();
      /* Display only if there is some valid disp_rate */
      if (disp_rate != 0) present_inputs(t);
    }
  else {
    fclose(fp);
    for (nextsnapshot=0; t >= snapshots[nextsnapshot]; nextsnapshot++){}
  }
  testing=oldtesting;
}

setup_labels()
{
  register int i;
  init_units();
  labeling=1;
  for(i=0; i<nvectors; i++)
    compute_labels(i);
  labeling=0;
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
      if (!testing && best*second != 0.0)
	{
/*	  if (shuffletable[i]==21) printf("%2d: (%d %d) %f, (%d %d) %f, %f %f\n", i,
		 besti, bestj, best,
		 secondi, secondj, second,
		 (best<second) ? (best/second) : (second/best), ratio);*/
/*	  if (((best<second) ? (best/second) : (second/best)) > ratio)*/
	    modify_weights(alpha(t), shuffletable[i]);
	}
      
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
  best=MAXDISTANCE;
  second=MAXDISTANCE;
  for(i=0; i<nx; i++)
    for(j=0; j<ny; j++)
      {
	units[i][j].value = distance(vectors[inpv].comp, units[i][j].comp,dim);
	/* check if this unit's response is best so far encountered */
	if (units[i][j].value < best)
	  {
	    secondi=besti; secondj=bestj; second=best;
	    besti=i; bestj=j; best=units[i][j].value;
	  }
	else if (units[i][j].value < second)
	  {
	    secondi=i; secondj=j; second=units[i][j].value;
	  }
	if (labeling)
	  if (units[i][j].value < units[i][j].bestvalue){
	    units[i][j].bestvalue = units[i][j].value;
	    units[i][j].labels[0] = inpv;
	}
      }
/*  printf("%s : %f\n", vectors[inpv].label, best);*/
  if (!labeling)
    {
      units[besti][bestj].labelcount=units[besti][bestj].labelcount+1;
      units[besti][bestj].labels[ units[besti][bestj].labelcount ] = inpv;
      if (strcasecmp(vectors[units[besti][bestj].labels[0]].label,
		     vectors[inpv].label))
	{
	  misclassified++;
	  bestmisclassified=1;
	}
      else bestmisclassified=0;
      if (strcasecmp(vectors[units[secondi][secondj].labels[0]].label,
		     vectors[inpv].label))
	secondmisclassified=1;
      else secondmisclassified=0;
    }
}

compute_labels(int inpv)
{
  register int i,j;
  float distance();
  best=MAXDISTANCE;
  second=MAXDISTANCE;
  for(i=0; i<nx; i++)
    for(j=0; j<ny; j++)
      {
	units[i][j].value = distance(vectors[inpv].comp,
				     units[i][j].comp,dim);
	if (units[i][j].value < units[i][j].bestvalue){
	  units[i][j].bestvalue = units[i][j].value;
	  units[i][j].labels[0] = inpv;
	}
      }
/*  printf("%s : %f\n", vectors[inpv].label, best);*/
}
  


modify_weights(float alpha, int inpv)
{
  register int k;
/*  if (inpv==15)
    printf("(%d %d) %f %d,   (%d %d) %f %d:  %f %f\n",
	   besti, bestj, best, bestmisclassified,
	   secondi, secondj, second, secondmisclassified,
	   best/second, second/best);*/

  if (bestmisclassified)
    for(k=0; k<dim; k++)
      units[besti][bestj].comp[k] -=
	alpha*(vectors[inpv].comp[k]-units[besti][bestj].comp[k]);
  else
    for(k=0; k<dim; k++)
      units[besti][bestj].comp[k] +=
	alpha*(vectors[inpv].comp[k]-units[besti][bestj].comp[k]);
/*
  if (secondmisclassified)
    for(k=0; k<dim; k++)
      units[secondi][secondj].comp[k] -=
	alpha*(vectors[inpv].comp[k]-units[secondi][secondj].comp[k]);
  else
    for(k=0; k<dim; k++)
      units[secondi][secondj].comp[k] +=
	alpha*(vectors[inpv].comp[k]-units[secondi][secondj].comp[k]);
*/
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
  fclose(fp);

  for(i=0; i<nvectors; i++)
    shuffletable[i]=i;
  shuffle();
}


init_weights()
{
  int randfun();
  iterate_weights(randfun);
  if (snapshots[0]==(-1))
    save_current(-1);			/* save the initial state */
}


init_units()
{
  register int i,j;
  misclassified=0;
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
  for(i=0; i<ncomp; i++) sum=sum+(v1[i]-v2[i])*(v1[i]-v2[i]);
  return(sqrt(sum));
}

float alpha(int t)
{
  /* decrease gain linearly within the phase */
  return(alpha_[phase-1] + (alpha_[phase]-alpha_[phase-1])*
	 (t-t_[phase-1])/(t_[phase]-t_[phase-1])
	 );
}

float f01rnd()
{
  /* random float between 0 and 1 */
  return (drand48());
}
