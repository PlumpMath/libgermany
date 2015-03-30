#include <stdio.h>
#include <stdint.h>
#include "coro.h"

#ifdef CORO_THREADED
static __declspec( thread ) coro_t* tls;

coro_t** coro_get_tls( void )
{
  return &tls;
}
#endif

static void fibonacci( void )
{
  uint64_t i = 1;
  uint64_t j = 1;
  
  coro_yield_u64_v( 1 );
  coro_yield_u64_v( 1 );
  
  for ( ;; )
  {
    uint64_t k = i + j;
    i = j;
    j = k;
    
    coro_yield_u64_v( k );
  }
}

int main()
{
  static const char* states[] = { "dead", "prep", "psed" };
  coro_t coro;
  char stack[ 1024 ];
  int i;
  
  coro_prepare( &coro, stack, sizeof( stack ), (uintptr_t)fibonacci );
  
  for ( i = 1; i <= 98; i++ )
  {
    if ( i == 95 )
    {
      coro_kill_u64( &coro, 31415927 );
    }
    
    printf( "%016llx %s fib_%02d = %llu\n", coro.rsp, states[ coro.state ], i, coro_resume_v_u64( &coro ) );
  }
  
  return 0;
}
