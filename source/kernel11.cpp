#include <3ds.h>

extern "C" int (*backdoor_callback)(void);
extern "C" int backdoor_ret;

int (*backdoor_callback)(void) = nullptr;
int backdoor_ret = 0;

static s32 __attribute__((naked))
KernelBackdoorTargetWrapper()
{
    __asm__ __volatile__ (
        "cpsid aif                  \t\n"
        "push {r4, lr}              \t\n"

        "ldr r0, =backdoor_callback \t\n"
        "ldr r0, [r0]               \t\n"
        "blx r0                     \t\n"
    
        "ldr r4, =backdoor_ret      \t\n"
        "str r0, [r4]               \t\n"
        "pop {r4, pc}               \t\n"
        );
}

int KernelBackdoor(int (*callback)(void))
{
    backdoor_callback = callback;
    svcBackdoor(KernelBackdoorTargetWrapper);
    return backdoor_ret;
}
