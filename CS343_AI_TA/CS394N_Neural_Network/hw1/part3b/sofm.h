/**********************  definitions  *************************************/
   
#include <stdio.h>
#include <math.h>

#define SNAPSHOTEND 999999999
#define MAXDISTANCE 999999999.9

#define MAXNX 20              /* maximum network size is nx*ny units */
#define MAXNY 20
#define MAXSNAPS 30           /* maximum number of snapshots */
#define MAXPHASE 10           /* maximum number of simulation phases */
#define MAXVECTORS 100        /* maximum number of input vectors */
#define MAXLABELL 30          /* maximum length of lables */
#define MAXDIM 100            /* maximum dimension of input vectors */

/* in t_, nc_, and alpha_: [-1] = initial value */
int nx, ny, t, t_array[MAXPHASE+1], *t_, tend, seed,
  nc_array[MAXPHASE+1], *nc_, besti, bestj, secondi, secondj, 
  nextsnapshot, snapshots[MAXSNAPS], disp_rate, continuing, 
  testing, labeling, startt, phase, nvectors, dim, misclassified, 
  bestmisclassified, secondmisclassified, step, oldt, oldtesting;
int shuffletable[MAXVECTORS];
float alpha_array[MAXPHASE+1], *alpha_, best, second;
char simufile[100], vectorfile[100], labelfile[100];
FILE *fp, *fp2;

typedef struct inputvectors {
  char label[MAXLABELL+1];
  float comp[MAXDIM];
} inputvectors;

typedef struct unitdata {
  int   labelcount;
  int   labels[MAXVECTORS+1];
  float bestvalue;
  float value;
  float comp[MAXDIM];
} unitdata;

inputvectors vectors[MAXVECTORS], labelvectors[MAXVECTORS];
unitdata units[MAXNX][MAXNY];

/* random number functions */
double drand48();
void srand48();
long lrand48();
int finish();
int display_best();
int iterate_snapshots();
int read_snapshot();
int setup_labels();
int present_inputs(int t);
int compute_responses(int inpv);
int compute_labels(int inpv);
int modify_weights(float alpha, int inpv);
int init_params(char *arg);
int read_params(FILE *fp);
int read_inputs();
int init_weights();
int init_units();
int save_current(int t);
int iterate_weights(int (*dofun)());
int shuffle();
int readfun(float *place);
int writefun(float *place);
int randfun(float *place);
float distance(float *v1, float *v2, int ncomp);
float alpha(int t);
int nc(int t);
float f01rnd();
