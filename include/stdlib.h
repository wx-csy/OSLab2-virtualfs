#ifndef _stdlib_h
#define _stdlib_h

#define EXIT_SUCCESS  0
#define EXIT_FAILURE  1

void _Exit(int exit_code);
void abort(void);

/* These functions generate low-quality pseudorandom
 * numbers, so they shouldn't be used for cryptographical 
 * purpose.
 */

#define RAND_MAX    0x7FFFFFFF

void srand(unsigned int seed);
int rand();

/*
typedef struct div_t    {int quot; int rem;} div_t;
typedef struct ldiv_t   {long quot; long rem;} ldiv_t;
typedef struct lldiv_t  {long long quot; long long rem;} lldiv_t;

div_t   div   (int x, int y);
ldiv_t  ldiv  (long x, long y);
lldiv_t lldiv (long long x, long long y);
*/

#endif

