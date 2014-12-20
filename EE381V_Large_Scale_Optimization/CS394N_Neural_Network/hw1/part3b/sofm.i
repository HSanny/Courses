%module sofm
%{
%}

%inline %{

#define MAXVECTORS 100  /* maximum number of input vectors */
#define MAXLABELL 30    /* maximum length of lables */
#define MAXDIM 100      /* maximum dimension of input vectors */
#define MAXNX 20        /* maximum network size is nx*ny units */
#define MAXNY 20

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

  extern inputvectors vectors[MAXVECTORS];
  extern unitdata units[MAXNX][MAXNY];
  extern char vectorfile[100];
  extern int startt, t, continuing, step, nx, ny, disp_rate, nvectors, tend, misclassified;
  extern int phase, seed;
  unitdata get_unit(unitdata (*units)[MAXNY], int i, int j) {
    return units[i][j];
  }
  int get_label(int *lbls, int idx) {
    return lbls[idx];
  }
  inputvectors get_vector(inputvectors *vector, int i) {
    return vector[i];
  }
%}

extern char vectorfile[100];
extern int startt, t, continuing, step, nx, ny, disp_rate, nvectors, tend, misclassified;
extern int phase, seed;
extern int finish();
extern int iterate_snapshots();
extern int read_snapshot();
extern int init_weights();
extern int setup_labels();
extern int init_params(char *arg);
extern int read_inputs();
extern int present_inputs(int t);
