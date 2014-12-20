#include "sofmdefs.h"

float normal(x1,x2)
float x1,x2;
{
   float y,y1,y2;

   y1 = sqrt((-2.0) * log( x1 )) * cos( 2*M_PI * x2);
   y2 = sqrt((-2.0) * log( x1 )) * sin( 2*M_PI * x2);
   y = 0.5 * (y1+y2);

   return(y);
}

extern int dim;
generate_input()
{
  /* input bimodally distributed in 2-D unit square */
  register int i;
  float f01rnd();

  oldvectors.comp[0]=vectors.comp[0]; /* x comp */
  vectors.comp[0]= 0.5 + 0.3 * normal( f01rnd(), f01rnd() );

  oldvectors.comp[1]=vectors.comp[1]; /* y comp */

  if ( f01rnd() < 0.5 )
     vectors.comp[1] = 0.25 + 0.1 * normal( f01rnd(), f01rnd() );
  else
     vectors.comp[1] = 0.75 + 0.1 * normal( f01rnd(), f01rnd() );
}

