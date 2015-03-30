#include <coro.h>

#ifdef CORO_DEBUG_STACK
#include <string.h>
#endif /* CORO_DEBUG_STACK */

typedef uint64_t (coro_start_t)( void );
extern coro_start_t coro_start;
typedef uint64_t (coro_return_t)( void );
extern coro_return_t coro_return;

typedef struct
{
  /* Space for the non-volatile registers. */
  uint8_t nvregs[ CORO_NVBYTES ];
  /* The coro_start function that is called the first time the coroutine is resumed. */
  uint64_t start;
  /* The coroutine's entry point. */
  uint64_t entry;
  /* Return address for when the coroutine returns. */
  uint64_t _return;
  /* Home area. */
  uint64_t rcx;
  uint64_t rdx;
  uint64_t r8;
  uint64_t r9;
}
initial_stack_t;

void coro_prepare( coro_t* coro, void* stkbot, unsigned int stksize, uintptr_t func )
{
  initial_stack_t* stk;
  int numargs;
  
  /* Compute the 16 byte-aligned stack pointer. */
  stk = (initial_stack_t*)( ( ( (uintptr_t)stkbot + stksize ) & ~15 ) - sizeof( initial_stack_t ) );

  /* coro_start will be called the first time the coroutine is resumed. */
  stk->start = (uint64_t)coro_start;
  
  /* The coroutine's entry point. */
  stk->entry = (uint64_t)func;
  
  /* If the coroutine exits via return, it exits to coro_return. */
  stk->_return = (uint64_t)coro_return;
  
  /* Initialize structure. */
  coro->rsp = (uint64_t)stk;
  coro->state = CORO_ACTIVE;
  
#ifdef CORO_DEBUG_STACK
  coro->stkbot = (uint64_t)(uintptr_t)stkbot;
  *(uint32_t*)stkbot = CORO_STACK_GUARD;
#endif /* CORO_DEBUG_STACK */
}
