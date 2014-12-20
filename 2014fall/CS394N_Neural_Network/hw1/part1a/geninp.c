#include "sofmdefs.h"

extern int dim;
generate_input()
{
  /* input uniformly distributed in 2-D unit square */
  register int i;
  float f01rnd();
  for(i=0; i<dim; i++)
    {
      oldvectors.comp[i]=vectors.comp[i]; /* save the previous point */
      vectors.comp[i]=f01rnd();
    }
}
