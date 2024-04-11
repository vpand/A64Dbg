// a64dbg cloud adcpp will be compiled to x86_64
// so we should include aarch64 headers manually for __NR_ syscall definitions
#include <aarch64-linux-android/asm/unistd.h>

#define case_string(n) case n: return #n;

static const char *syscall_name(int syscall) {
  switch (syscall) {
  case_string(__NR_openat);
  case_string(__NR_close);
  case_string(__NR_read);
  case_string(__NR_write);
  default: return "???";
  }
}

static hyper_hook_return_t hyper_hook_printf(void *usrdata, regctx_t *vm) {
  printf("%s is invoked, the format string is : %s\n", __FUNCTION__, vm->r[0].s);                              
  return hyper_continue;                                    
}

static hyper_hook_return_t hyper_hook_syscall(void *usrdata, int syscall, regctx_t *vm) {
  printf("%s is invoked, the syscall number is : %d(%s), p0=%p, p1=%p\n", __FUNCTION__,
    syscall, syscall_name(syscall), vm->r[0].p, vm->r[1].p);                     
  return hyper_continue;                                    
}

void adc_main() {
  hook_symbol_hyper("libc.so", "printf", hyper_hook_printf, nullptr);
  hook_svc_hyper(hyper_hook_syscall, nullptr);
  puts("Fired hyper hook for yoophone debugee's libc.so-printf,syscall.\n");
}
