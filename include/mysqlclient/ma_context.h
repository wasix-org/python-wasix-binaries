/*
   Copyright 2011 Kristian Nielsen and Monty Program Ab
             2015, 2022 MariaDB Corporation AB

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  Simple API for spawning a co-routine, to be used for async libmysqlclient.

  Idea is that by implementing this interface using whatever facilities are
  available for given platform, we can use the same code for the generic
  libmysqlclient-async code.

  (This particular implementation uses Posix ucontext swapcontext().)
*/


/*
  When running with address sanitizer, the stack switching can cause confusion
  unless the __sanitizer_{start,finish}_switch_fiber() functions are used
  (CONC-618).

  In this case prefer the use of boost::context or ucontext, which should have
  this instrumentation, over our custom assembler variants.
*/
#ifdef __has_feature
   /* Clang */
#  if __has_feature(address_sanitizer)
#    define ASAN_PREFER_NON_ASM 1
#  endif
#else
   /* GCC */
#  ifdef __SANITIZE_ADDRESS__
#    define ASAN_PREFER_NON_ASM 1
#  endif
#endif

#ifdef _WIN32
#define MY_CONTEXT_USE_WIN32_FIBERS 1
#elif defined(ASAN_PREFER_NON_ASM) && defined(HAVE_BOOST_CONTEXT_H)
#define MY_CONTEXT_USE_BOOST_CONTEXT
#elif defined(ASAN_PREFER_NON_ASM) && defined(HAVE_UCONTEXT_H)
#define MY_CONTEXT_USE_UCONTEXT
#elif defined(__GNUC__) && __GNUC__ >= 3 && defined(__x86_64__) && !defined(__ILP32__)
#define MY_CONTEXT_USE_X86_64_GCC_ASM
#elif defined(__GNUC__) && __GNUC__ >= 3 && defined(__i386__)
#define MY_CONTEXT_USE_I386_GCC_ASM
#elif defined(__GNUC__) && __GNUC__ >= 3 && defined(__aarch64__)
#define MY_CONTEXT_USE_AARCH64_GCC_ASM
#elif defined(HAVE_BOOST_CONTEXT_H)
#define MY_CONTEXT_USE_BOOST_CONTEXT
#elif defined(HAVE_UCONTEXT_H)
#define MY_CONTEXT_USE_UCONTEXT
#else
#define MY_CONTEXT_DISABLE
#endif

#ifdef   __cplusplus
extern "C" {
#endif

#ifdef MY_CONTEXT_USE_WIN32_FIBERS
struct my_context {
  void (*user_func)(void *);
  void *user_arg;
  void *app_fiber;
  void *lib_fiber;
  int return_value;
#ifndef DBUG_OFF
  void *dbug_state;
#endif
};
#endif


#ifdef MY_CONTEXT_USE_UCONTEXT
#include <ucontext.h>

struct my_context {
  void (*user_func)(void *);
  void *user_data;
  void *stack;
  size_t stack_size;
  ucontext_t base_context;
  ucontext_t spawned_context;
  int active;
#ifdef HAVE_VALGRIND
  unsigned int valgrind_stack_id;
#endif
#ifndef DBUG_OFF
  void *dbug_state;
#endif
};
#endif


#ifdef MY_CONTEXT_USE_X86_64_GCC_ASM
#include <stdint.h>

struct my_context {
  uint64_t save[9];
  void *stack_top;
  void *stack_bot;
#ifdef HAVE_VALGRIND
  unsigned int valgrind_stack_id;
#endif
#ifndef DBUG_OFF
  void *dbug_state;
#endif
};
#endif


#ifdef MY_CONTEXT_USE_I386_GCC_ASM
#include <stdint.h>

struct my_context {
  uint64_t save[7];
  void *stack_top;
  void *stack_bot;
#ifdef HAVE_VALGRIND
  unsigned int valgrind_stack_id;
#endif
#ifndef DBUG_OFF
  void *dbug_state;
#endif
};
#endif


#ifdef MY_CONTEXT_USE_AARCH64_GCC_ASM
#include <stdint.h>

struct my_context {
  uint64_t save[22];
  void *stack_top;
  void *stack_bot;
#ifdef HAVE_VALGRIND
  unsigned int valgrind_stack_id;
#endif
#ifndef DBUG_OFF
  void *dbug_state;
#endif
};
#endif


#ifdef MY_CONTEXT_USE_BOOST_CONTEXT
/*
  boost::context is a C++-library that provides a portable co-routine fallback
  for architectures that lack a native my_context implementation, and which is
  available on some platforms where ucontext is not (ucontext has been
  deprecated in Posix).

  Since boost::context is in C++, the implementation details must be put into
  a separate source file ma_boost_context.cc and hidden in an opaque void *.
*/
struct my_context {
  /* Pointer to state that uses C++-types and cannot be compiled in C. */
  void *internal_context;
  void *stack;
  size_t stack_size;
#ifndef DBUG_OFF
  void *dbug_state;
#endif
  int active;
#ifdef HAVE_VALGRIND
  unsigned int valgrind_stack_id;
#endif
};
#endif /* MY_CONTEXT_USE_BOOST_CONTEXT */


#ifdef MY_CONTEXT_DISABLE
struct my_context {
  int dummy;
};
#endif

/*
  Initialize an asynchronous context object.
  Returns 0 on success, non-zero on failure.
*/
extern int my_context_init(struct my_context *c, size_t stack_size);

/* Free an asynchronous context object, deallocating any resources used. */
extern void my_context_destroy(struct my_context *c);

/*
  Spawn an asynchronous context. The context will run the supplied user
  function, passing the supplied user data pointer.

  The context must have been initialised with my_context_init() prior to
  this call.

  The user function may call my_context_yield(), which will cause this
  function to return 1. Then later my_context_continue() may be called, which
  will resume the asynchronous context by returning from the previous
  my_context_yield() call.

  When the user function returns, this function returns 0.

  In case of error, -1 is returned.
*/
extern int my_context_spawn(struct my_context *c, void (*f)(void *), void *d);

/*
  Suspend an asynchronous context started with my_context_spawn.

  When my_context_yield() is called, execution immediately returns from the
  last my_context_spawn() or my_context_continue() call. Then when later
  my_context_continue() is called, execution resumes by returning from this
  my_context_yield() call.

  Returns 0 if ok, -1 in case of error.
*/
extern int my_context_yield(struct my_context *c);

/*
  Resume an asynchronous context. The context was spawned by
  my_context_spawn(), and later suspended inside my_context_yield().

  The asynchronous context may be repeatedly suspended with
  my_context_yield() and resumed with my_context_continue().

  Each time it is suspended, this function returns 1. When the originally
  spawned user function returns, this function returns 0.

  In case of error, -1 is returned.
*/
extern int my_context_continue(struct my_context *c);

struct st_ma_pvio;

struct mysql_async_context {
  /*
    This is set to the value that should be returned from foo_start() or
    foo_cont() when a call is suspended.
  */
  unsigned int events_to_wait_for;
  /*
    It is also set to the event(s) that triggered when a suspended call is
    resumed, eg. whether we woke up due to connection completed or timeout
    in mysql_real_connect_cont().
  */
  unsigned int events_occurred;
  /*
    This is set to the result of the whole asynchronous operation when it
    completes. It uses a union, as different calls have different return
    types.
  */
  union {
    void *r_ptr;
    const void *r_const_ptr;
    int r_int;
    my_bool r_my_bool;
  } ret_result;
  /*
    The timeout value (in millisecods), for suspended calls that need to wake
    up on a timeout (eg. mysql_real_connect_start().
  */
  unsigned int timeout_value;
  /*
    This flag is set when we are executing inside some asynchronous call
    foo_start() or foo_cont(). It is used to decide whether to use the
    synchronous or asynchronous version of calls that may block such as
    recv().

    Note that this flag is not set when a call is suspended, eg. after
    returning from foo_start() and before re-entering foo_cont().
  */
  my_bool active;
  /*
    This flag is set when an asynchronous operation is in progress, but
    suspended. Ie. it is set when foo_start() or foo_cont() returns because
    the operation needs to block, suspending the operation.

    It is used to give an error (rather than crash) if the application
    attempts to call some foo_cont() method when no suspended operation foo is
    in progress.
  */
  my_bool suspended;
  /*
    If non-NULL, this is a pointer to a callback hook that will be invoked with
    the user data argument just before the context is suspended, and just after
    it is resumed.
  */
  struct st_ma_pvio *pvio;
  void (*suspend_resume_hook)(my_bool suspend, void *user_data);
  void *suspend_resume_hook_user_data;

  /* If non-NULL,  this is a poitner to the result of getaddrinfo() currently
   * under traversal in pvio_socket_connect(). It gets reset to NULL when a
   * connection has been established to a server. The main objective is to
   * free this memory resource in mysql_close() while an initiated connection
   * has not been established. */
  struct addrinfo* pending_gai_res;

  /*
    This is used to save the execution contexts so that we can suspend an
    operation and switch back to the application context, to resume the
    suspended context later when the application re-invokes us with
    foo_cont().
  */
  struct my_context async_context;
};

#ifdef   __cplusplus
}
#endif
