///////////////////////////////////VPAND.COM////////////////////////////////////
//                                                                             *
// A64Dbg PLUGIN HEADER FILE                                                   *
//                                                                             *
// Copyright(C) 2024 VPAND.COM TEAM, ALL RIGHTS RESERVED.                      *
//                                                                             *
// Internet: yunyoo.cn vpand.com                                               *
//                                                                             *
// This code is distributed "as is", part of A64Dbg and without warranty of    *
// any kind, expressed or implied, including, but not limited to warranty of   *
// fitness for any particular purpose. In no event will A64Dbg be liable to    *
// you for any special, incidental, indirect, consequential or any other       *
// damages caused by the use, misuse, or the inability to use of this code,    *
// including anylost profits or lost savings, even if A64Dbg has been advised  *
// of the possibility of such damages.                                         *
//                                                                             *
///////////////////////////////////////*////////////////////////////////////////
#ifndef __ADCPP_H__
#define __ADCPP_H__

#define __ADCPP_VERSION__ "1.0.5"
#define __ADCPP_CDECL__ extern "C"

// function with this prefix is the real user api
#define __ADCPP_API__ static inline
#define __ADCPP_EXPORT__ __attribute__((visibility("default")))

/*
 * standard headers
 */
// pre-include standard c headers
#include <stddef.h>
#include <ctype.h>   // Functions to determine the type contained in character data
#include <errno.h>	 // Macros reporting error conditions
#include <float.h>	 // Limits of float types
#include <inttypes.h>// Format conversion of integer types
#include <limits.h>	 // Sizes of basic types
#include <locale.h>	 // Localization utilities
#include <math.h>	 // Common mathematics functions
#include <signal.h>	 // Signal handling
#include <stdarg.h>	 // Variable arguments
#include <stdbool.h> // Macros for boolean type
#include <stddef.h>	 // Common macro definitions
#include <stdint.h>  // Fixed-width integer types
#include <stdio.h>	 // Input/output
#include <stdlib.h>	 // General utilities: memory management, program utilities, string conversions, random numbers, algorithms
#include <string.h>	 // String handling
#include <time.h>	 // Time/date utilities

// pre-include unix c headers
#include <pthread.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>

// pre-include c++ stl headers
#include <string>
#include <vector>
#include <set>
#include <map>

// pre-include for different platforms

#if __APPLE__
// pre-include basic objc framework
#include <TargetConditionals.h>

#if __OBJC__
#import <Foundation/Foundation.h>
#if TARGET_OS_OSX
#else
#import <UIKit/UIKit.h>
#endif
#endif

#else
// pre-include basic jni headers
#include <jni.h>
#include <android/log.h>

#endif

#if __x86__ || __x86_64__
#define __YOOPHONE_HYPER__ 1
#endif

#if __YOOPHONE_HYPER__
#define __aarch64__ 1
#define __arm64__ 1
#endif

#if __arm__
// arm common register like r0 r1 ... lr sp pc
typedef union cmmreg_t {
  unsigned int w;
  int sw;
  const void *p;
  const char *s;
} cmmreg_t;

// arm neon register like s0 d0 ...
typedef union neonreg_t {
  unsigned int i[2];
  unsigned long long l;
  int si[2];
  long long sl;
  float f[2];
  double d;
} neonreg_t;

// arm execution context
typedef struct regctx_t {
  cmmreg_t r[16];  // 0-12, 13-sp, 14-lr, 15-reserved
  neonreg_t v[32];
  cmmreg_t pc;
} regctx_t;
#else
// arm64 common register like x0 x1 ... lr sp pc
typedef union cmmreg_t {
  unsigned int w;
  unsigned long long x;
  int sw;
  long long sx;
  const void *p;
  const char *s;
  unsigned int ws[2];
  int sws[2];
} cmmreg_t;

// arm64 neon register like s0 d0 q0 ...
typedef union neonreg_t {
  unsigned int i[4];
  unsigned long long l[2];
  int si[4];
  long long sl[2];
} neonreg_t;

// arm64 execution context
typedef struct regctx_t {
  cmmreg_t r[32];  // 0-28,29-fp,30-lr,31-sp
  neonreg_t v[32];
  cmmreg_t pc;
} regctx_t;
#endif

// yoophone hyper hook return type
typedef enum hyper_hook_return_t {
  /*
  * return type for inline and syscall hooks
  */
  // continue executing by hyper engine
  hyper_continue, 
  // hooker has modified the vm context
  // must write them back to hyper engine before continue executing
  hyper_modified, 

  /*
  * return type only for inline hook
  */
  // already processed by hooker
  // break calling and return to vm caller
  hyper_break, 
} hyper_hook_return_t;

/*
usrdata is passed in by hook_inline_hyper;
vm is the current virtual mchaine's register context;
*/
typedef hyper_hook_return_t (*hyper_hook_inline_t)(void *usrdata, regctx_t *vm);

/*
usrdata is passed in by hook_svc_hyper;
syscall is the current calling svc number;
vm is the current virtual mchaine's register context;

should return hyper_continue or hyper_modified.
*/
typedef hyper_hook_return_t (*hyper_hook_svc_t)(void *usrdata, int syscall, regctx_t *vm);

struct adcpp_api_t {
  // add log to a64dbg
  void (*send2ad)(const char *format, ...);

  // send string to python3 adp, pyfn is the python3 recever function name
  void (*sendstr2py)(const char *pyfn, const char *format, ...);

  // send buffer to python3 adp, pyfn is the python3 recever function name
  void (*sendbuf2py)(const char *pyfn, const void *buff, long size);
  
  // get the debugee thread register context, return null if there's no debugee thread
  const regctx_t *(*current_regs)();
  
  // hookers
  void (*hook_inline)(const void *srcfn, const void *hooker, void **orig);
  void (*hook_got)(const char *imagename, const char *funcname, const void *hooker);
  
  // uvm hookers
  // trap srcfn to UraniumVM Execution Environment without code modification
  void (*hook_inline_uvm)(const void *srcfn, const void *hooker, void **orig);
  void (*unhook_inline_uvm)(const void *srcfn);

  // yoophone hyper hookers
  // set hookers for yoophone hyper virtualization engine
  // srclib is the hookee library name, like "libc.so"
  // srcfn is the hookee function name in srclib, like "open"
  void (*hook_symbol_hyper)(const char *srclib, const char *srcfn, 
                            hyper_hook_inline_t hooker, void *usrdata);
  // srcfn is the hookee runtime address, you can copy it from a64dbg's module symbol list
  void (*hook_inline_hyper)(uint64_t srcfn, hyper_hook_inline_t hooker, 
                            void *usrdata);
  // set hooker for every syscall
  void (*hook_svc_hyper)(hyper_hook_svc_t hooker, void *usrdata);
  // hyper unhook, hooker is the address passed into hook_*_hyper
  void (*unhook_hyper)(const void *hooker);

#if __APPLE__ // macOS/iOS
  // void MSHookMessageEx(Class _class, SEL message, IMP hook, IMP *old);
  void (*hook_objc)(const void *cls, const void *sel, const void *hooker, void **orig);
#else // android
  // get the android jvm instance
  JavaVM *(*current_jvm)();

  // get the current JNIEnv attached by current thread
  JNIEnv *(*current_jenv)();
#endif

#if __YOOPHONE_HYPER__ // yoophone
  // load an arm64 so from yoophone app
  void *(*load_so_hyper)(const char *path);
#endif
};

// get the adcpp api instance from uvmdbg runtime
const adcpp_api_t *uvmdbg_adcapi();

#if !ADCPP_IMPL
/*
 * api wrappers
 */
__ADCPP_API__ const adcpp_api_t *adcapi() {
  // cache it to improve performance
  static const adcpp_api_t *api = nullptr;
  if (!api) api = uvmdbg_adcapi();
  return api;
}

// dump buffer to python
__ADCPP_API__ void buf2py(const char *pyfn, const void *buff, long size) {
  adcapi()->sendbuf2py(pyfn, buff, size);
}

// hookers
__ADCPP_API__ void hook_inline(const void *srcfn, const void *hooker, void **orig) {
  adcapi()->hook_inline(srcfn, hooker, orig);
}

__ADCPP_API__ void hook_got(const char *imagename, const char *funcname, const void *hooker) {
  adcapi()->hook_got(imagename, funcname, hooker);
}

// uvm hookers
__ADCPP_API__ void hook_inline_uvm(const void *srcfn, const void *hooker, void **orig) {
  adcapi()->hook_inline_uvm(srcfn, hooker, orig);
}

// uvm unhookers
__ADCPP_API__ void unhook_inline_uvm(const void *srcfn) {
  adcapi()->unhook_inline_uvm(srcfn);
}

// hyper symbol hooker
__ADCPP_API__ void hook_symbol_hyper(const char *srclib, const char *srcfn, 
                                     hyper_hook_inline_t hooker, void *usrdata) {
  adcapi()->hook_symbol_hyper(srclib, srcfn, hooker, usrdata);
}

// hyper address hooker
__ADCPP_API__ void hook_inline_hyper(uint64_t srcfn, hyper_hook_inline_t hooker, 
                                     void *usrdata) {
  adcapi()->hook_inline_hyper(srcfn, hooker, usrdata);
}

// hyper syscall hooker
__ADCPP_API__ void hook_svc_hyper(hyper_hook_svc_t hooker, void *usrdata) {
  adcapi()->hook_svc_hyper(hooker, usrdata);
}

// hyper unhook
__ADCPP_API__ void unhook_hyper(const void *hooker) {
  adcapi()->unhook_hyper(hooker);
}

// dump string to python
#define str2py(pyfn, format, ...) adcapi()->sendstr2py(pyfn, format, __VA_ARGS__)

#if __APPLE__ // macOS/iOS
__ADCPP_API__ void hook_objc(const void *cls, const void *sel, const void *hooker, void **orig) {
  adcapi()->hook_objc(cls, sel, hooker, orig);
}
#else // android
__ADCPP_API__ JavaVM *current_jvm() {
  return adcapi()->current_jvm();
}

__ADCPP_API__ JNIEnv *current_jenv() {
  return adcapi()->current_jenv();
}
#endif

#if __YOOPHONE_HYPER__ // yoophone
// load an arm64 so from yoophone app
__ADCPP_API__ void *load_so_hyper(const char *path) {
  return adcapi()->load_so_hyper(path);
}
#endif

#endif // end of !ADCPP_IMPL

// some register wrapper
#if __arm__
#define r0  adcapi()->current_regs()->r[0].w
#define r1  adcapi()->current_regs()->r[1].w
#define r2  adcapi()->current_regs()->r[2].w
#define r3  adcapi()->current_regs()->r[3].w
#define r4  adcapi()->current_regs()->r[4].w
#define r5  adcapi()->current_regs()->r[5].w
#define r6  adcapi()->current_regs()->r[6].w
#define r7  adcapi()->current_regs()->r[7].w
#define r8  adcapi()->current_regs()->r[8].w
#define r9  adcapi()->current_regs()->r[9].w
#define r10 adcapi()->current_regs()->r[10].w
#define r11 adcapi()->current_regs()->r[11].w
#define r12 adcapi()->current_regs()->r[12].w
#define r13 adcapi()->current_regs()->r[13].w
#define r14 adcapi()->current_regs()->r[14].w
#define r15 adcapi()->current_regs()->pc.w
#define fp  adcapi()->current_regs()->r[12].w
#define sp  adcapi()->current_regs()->r[13].w
#define lr  adcapi()->current_regs()->r[14].w
#define pc  adcapi()->current_regs()->pc.w
#else
#define x0  adcapi()->current_regs()->r[0].x
#define x1  adcapi()->current_regs()->r[1].x
#define x2  adcapi()->current_regs()->r[2].x
#define x3  adcapi()->current_regs()->r[3].x
#define x4  adcapi()->current_regs()->r[4].x
#define x5  adcapi()->current_regs()->r[5].x
#define x6  adcapi()->current_regs()->r[6].x
#define x7  adcapi()->current_regs()->r[7].x
#define x29 adcapi()->current_regs()->r[29].x
#define x30 adcapi()->current_regs()->r[30].x
#define x31 adcapi()->current_regs()->r[31].x
#define fp  adcapi()->current_regs()->r[29].x
#define lr  adcapi()->current_regs()->r[30].x
#define sp  adcapi()->current_regs()->r[31].x
#define pc  adcapi()->current_regs()->pc.x
#endif

// redirect printf/puts to a64dbg's log
#define printf adcapi()->send2ad
#define puts adcapi()->send2ad

// asm function macro helper
#define __ASM__ __asm__ __volatile__
#define __NAKED__ __attribute__((naked))

/*
 * a valid adcpp module loaded by UraniumVM/YooPhone must implement one of these functions
*/
// the start entry of adcpp module.
__ADCPP_CDECL__ __ADCPP_EXPORT__ void adc_main(void);

// the start entry of adcpp module which will interperte it in a new thread.
__ADCPP_CDECL__ __ADCPP_EXPORT__ void adc_main_thread(void);

#endif // end of __ADCPP_H__
