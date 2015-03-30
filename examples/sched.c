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

static uint8_t stacks[ 3 ][ 1024 ];
static coro_t  coroutines[ 3 ];
static int     current;

static coro_t* next_coro()
{
  coro_t* coro = coroutines + current;
  current = ( current + 1 ) % 3;
  return coro;
}

static void print_u64( uint64_t param )
{
  printf( "fib: %llu\n", param );
}

static void print_double( double param )
{
  printf( "fat: %.16e\n", param );
}

static void fat( void* param )
{
  double res = 1.0;
  double next = 1.0;
  
  for ( ;; )
  {
    coro_run_on_caller_f64( (uintptr_t)print_double, res );
    coro_chain_v( next_coro() );
    
    res *= next;
    next += 1.0;
  }
}

static void fibonacci( void* param )
{
  uint64_t i = 1;
  uint64_t j = 1;
  
  (void)param;

  coro_run_on_caller_u32( (uintptr_t)print_u64, i );
  coro_chain_v( next_coro() );
  
  coro_run_on_caller_u32( (uintptr_t)print_u64, j );
  coro_chain_v( next_coro() );
  
  for ( ;; )
  {
    uint64_t k = i + j;
    i = j;
    j = k;
    
    coro_run_on_caller_u32( (uintptr_t)print_u64, k );
    coro_chain_v( next_coro() );
  }
}

static void yield( void* param )
{
  for ( ;; )
  {
    coro_yield_v_v();
  }
}

int main()
{
  int i;
  
  coro_prepare( coroutines + 0, stacks[ 0 ], sizeof( stacks[ 0 ] ), (uintptr_t)fat );
  coro_prepare( coroutines + 1, stacks[ 1 ], sizeof( stacks[ 1 ] ), (uintptr_t)fibonacci );
  coro_prepare( coroutines + 2, stacks[ 2 ], sizeof( stacks[ 2 ] ), (uintptr_t)yield );
  current = 0;
  
  for ( i = 1; i <= 10; i++ )
  {
    printf( "\niteration %d\n", i );
    coro_resume_v_v( next_coro() );
  }
  
  return 0;
}
