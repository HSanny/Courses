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
  nc_array[MAXPHASE+1], *nc_,
  besti, bestj, secondi, secondj, nextsnapshot, snapshots[MAXSNAPS],
  disp_rate, continuing, testing, labeling, startt, phase,
  nvectors, dim, misclassified, bestmisclassified, secondmisclassified;
int shuffletable[MAXVECTORS];
float alpha_array[MAXPHASE+1], *alpha_, best, second;
char simufile[100], vectorfile[100], labelfile[100];
FILE *fp, *fp2;

struct inputvectors {
  char label[MAXLABELL+1];
  float comp[MAXDIM];
} vectors[MAXVECTORS];

struct unitdata {
  int   labelcount;
  int   labels[MAXVECTORS+1];
  float bestvalue;
  float value;
  float comp[MAXDIM];
} units[MAXNX][MAXNY];

/* random number functions */
double drand48();
void srand48();
long lrand48();
