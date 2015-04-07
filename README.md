# libgermany

libgermany is a Win64 coroutine library. It's almost entirely written in assembly besides an include files with function signatures and some defines, and it doesn't depend on anything from libc like SJLJ coroutine implementations.

libgermany has one interesting feature that allows one to call a function from inside a coroutine but using the caller's (the scheduler's) stack instead of it's own. This allows smaller stacks for the coroutines since called functions that consumes too much stack can be run on the large stack of the caller.

See the source files and the provided examples to learn how to use libgermany.
