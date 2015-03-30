#ifndef CORO_H
#define CORO_H

/*****************************************************************************
BEGIN OF USER SETTINGS
*****************************************************************************/

/*
If you define CORO_DEBUG_STACK:

1. A guard will be written at stkbot in coro_prepare
2. The stack pointer will be checked against stkbot to look for an underflow
   when coro_resume returns
3. The guard will be checked to see if it was overwritten when coro_resume returns
4. You must write the void coro_stack_error( coro_t*, int )
   function which will be called if the checks in items 2 or 3 fail
*/
#undef CORO_DEBUG_STACK

/*
The 32-bit guard to check for stack underflows if CORO_DEBUG_STACK is defined.
*/
#define CORO_STACK_GUARD 0xdeadbeef

/*
If you define CORO_THREADED:

1. You must write the coro_t** coro_get_tls( void ) which will be called to
   get a thread-local space used internally
*/
#undef CORO_THREADED

/*****************************************************************************
END OF USER SETTINGS -- DO NOT CHANGE ANYHTING BELOW THIS LINE
*****************************************************************************/

/*
The possible states of a coroutine.
*/
#define CORO_DEAD   0
#define CORO_ACTIVE 1

/*
Stack errors passed to coro_stack_underflow.
*/
#define CORO_STACK_UNDERFLOW   1
#define CORO_GUARD_OVERWRITTEN 2

/*
The number of bytes used to save the non-volatile registers.

* R12, R13, R14, R15, RDI, RSI, RBX, RBP (8 * 8)
* XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15 (10 * 16)
* 8 bytes for stack alignment (since in this ABI rsp must be adjusted)
*/
#define CORO_NVBYTES ( 8 * 8 + 10 * 16 + 8 )

#ifndef CORO_ASM_COMPILE

#include <stdint.h>

typedef struct
{
  uint64_t rsp;
  uint8_t  state;
  
#ifdef CORO_DEBUG_STACK
  uint64_t stkbot;
#endif
}
coro_t;

#ifndef CORO_DEBUG_STACK
void coro_stack_error( coro_t* coro, int reason );
#endif

void coro_prepare( coro_t* coro, void* stkbot, unsigned int stksize, uintptr_t func );

#define CORO_RESUME_FUNC( argt, argn, rett, retn ) rett coro_resume_ ## argn ## _ ## retn( coro_t* coro, argt yield_ret_val );

#define CORO_RESUME_FUNCS( argt, argn ) \
  CORO_RESUME_FUNC( argt, argn, int8_t, i8 )    \
  CORO_RESUME_FUNC( argt, argn, int16_t, i16 )  \
  CORO_RESUME_FUNC( argt, argn, int32_t, i32 )  \
  CORO_RESUME_FUNC( argt, argn, int64_t, i64 )  \
  CORO_RESUME_FUNC( argt, argn, uint8_t, u8 )   \
  CORO_RESUME_FUNC( argt, argn, uint16_t, u16 ) \
  CORO_RESUME_FUNC( argt, argn, uint32_t, u32 ) \
  CORO_RESUME_FUNC( argt, argn, uint64_t, u64 ) \
  CORO_RESUME_FUNC( argt, argn, float, f32 )    \
  CORO_RESUME_FUNC( argt, argn, double, f64 )   \
  CORO_RESUME_FUNC( argt, argn, void*, p )      \
  argt coro_resume_v_ ## argn( coro_t* coro );  \
  void coro_resume_ ## argn ## _v( coro_t* coro, argt yield_ret_val );

#define CORO_YIELD_FUNC( argt, argn, rett, retn ) rett coro_yield_ ## argn ## _ ## retn( argt resume_ret_val );

#define CORO_YIELD_FUNCS( argt, argn ) \
  CORO_YIELD_FUNC( argt, argn, int8_t, i8 )    \
  CORO_YIELD_FUNC( argt, argn, int16_t, i16 )  \
  CORO_YIELD_FUNC( argt, argn, int32_t, i32 )  \
  CORO_YIELD_FUNC( argt, argn, int64_t, i64 )  \
  CORO_YIELD_FUNC( argt, argn, uint8_t, u8 )   \
  CORO_YIELD_FUNC( argt, argn, uint16_t, u16 ) \
  CORO_YIELD_FUNC( argt, argn, uint32_t, u32 ) \
  CORO_YIELD_FUNC( argt, argn, uint64_t, u64 ) \
  CORO_YIELD_FUNC( argt, argn, float, f32 )    \
  CORO_YIELD_FUNC( argt, argn, double, f64 )   \
  CORO_YIELD_FUNC( argt, argn, void*, p )      \
  argt coro_yield_v_ ## argn( void );          \
  void coro_yield_ ## argn ## _v( argt resume_ret_val );

#define CORO_FUNCS( argt, argn ) \
  CORO_RESUME_FUNCS( argt, argn )                               \
  CORO_YIELD_FUNCS( argt, argn )                                \
  void coro_suicide_ ## argn( argt suicide_ret_val );           \
  void coro_kill_ ## argn( coro_t* coro, argt kill_ret_val );   \
  void coro_run_on_caller_ ## argn( uintptr_t func, argt val ); \
  void coro_chain_ ## argn ( coro_t* coro, argt resume_ret_val );

CORO_FUNCS( int8_t, i8 )
CORO_FUNCS( int16_t, i16 )
CORO_FUNCS( int32_t, i32 )
CORO_FUNCS( int64_t, i64 )
CORO_FUNCS( uint8_t, u8 )
CORO_FUNCS( uint16_t, u16 )
CORO_FUNCS( uint32_t, u32 )
CORO_FUNCS( uint64_t, u64 )
CORO_FUNCS( float, f32 )
CORO_FUNCS( double, f64 )
CORO_FUNCS( void*, p )

void coro_resume_v_v( coro_t* coro );
void coro_yield_v_v( void );
void coro_suicide_v( void );
void coro_kill_v( coro_t* coro );
void coro_run_on_caller_v( uintptr_t func );
void coro_chain_v( coro_t* coro );

#endif /* CORO_ASM_COMPILE */

#endif /* CORO_H */
