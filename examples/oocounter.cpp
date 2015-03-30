#include <stdio.h>
#include "Coroutine.h"

extern "C" {
#include "coro.h"
}

#ifdef CORO_THREADED
static __declspec( thread ) coro_t* tls;

extern "C" coro_t** coro_get_tls( void )
{
  return &tls;
}
#endif

typedef struct
{
  int i;
  coro_t* coro;
}
printargs_t;

static void PrintInt( printargs_t* p )
{
  static const char* states[] = { "dead", "prep", "psed" };

  printf( "%016llx %s %d\n", p->coro->rsp, states[ p->coro->state ], p->i );
}

class Counter: public Coroutine
{
private:
  char stack[ 1024 ];
  int m_Count;
  
public:
  inline Counter(): m_Count( 0 )
  {
    Prepare( stack, sizeof( stack ) );
  }
  
  void Run()
  {
    for ( ;; )
    {
      printargs_t a;
      a.coro = &m_Coro;
      a.i = m_Count;
      coro_run_on_caller_p( (uintptr_t)PrintInt, (void*)&a );
      
      m_Count++;
      Yield();
    }
  }
};

int main()
{
  Counter counter;
  
  for ( int i = 0; i < 20; i++ )
  {
    counter.Resume();
  }
  
  return 0;
}
