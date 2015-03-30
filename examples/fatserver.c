#include <stdio.h>
#include "coro.h"

#ifdef CORO_THREADED
static __declspec( thread ) coro_t* tls;

coro_t** coro_get_tls( void )
{
  return &tls;
}
#endif

static void fat( int d )
{
  double res = 0.0;
  
  for ( ;; )
  {
    if ( d >= 172 )
    {
      coro_suicide_f64( -3.1415927 );
    }
    
    res = 1.0;
    
    while ( d > 1 )
    {
      res *= d;
      d--;
    }
    
    d = coro_yield_f64_i32( res );
  }
}

int main()
{
  static const char* states[] = { "dead", "prep", "psed" };
  coro_t coro;
  char stack[ 1024 ];
  int i;
  
  coro_prepare( &coro, stack, sizeof( stack ), (uintptr_t)fat );
  
  for ( i = 0; i <= 175; i++ )
  {
    printf( "%016llx %s %3d! = %.16e\n", coro.rsp, states[ coro.state ], i, coro_resume_i32_f64( &coro, i ) );
  }
  
  return 0;
}
