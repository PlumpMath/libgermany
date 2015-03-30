#ifndef COROUTINE_H
#define COROUTINE_H

extern "C" {
#include "coro.h"
}

class Coroutine
{
protected:
  coro_t m_Coro;
  
  inline void Prepare( void* stack, unsigned int stksize )
  {
    coro_prepare( &m_Coro, stack, stksize, (uintptr_t)RunMe );
  }
  
  static void RunMe( Coroutine* self );

public:
  inline void Resume()
  {
    coro_resume_p_v( &m_Coro, (void*)this );
  }
  
  inline void Yield()
  {
    coro_yield_v_v();
  }
  
  virtual void Run() = 0;
};

#endif /* COROUTINE_H */
