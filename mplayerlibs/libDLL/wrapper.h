#ifndef _WRAPPER_H
#define _WRAPPER_H

#include "cygwin_inttypes.h"
#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct {
  u_int32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
} reg386_t;

typedef int (*wrapper_func_t)(void *stack_base, int stack_size, reg386_t *reg,  u_int32_t *flags);

extern wrapper_func_t report_entry, report_ret;

extern void (*wrapper_target)(void);

extern int wrapper(void);
extern int null_call(void);
#ifdef __cplusplus
}
#endif

#endif /* _WRAPPER_H */

