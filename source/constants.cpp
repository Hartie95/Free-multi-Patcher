#include "constants.h"

#include "kernel11.h"

u32 curr_kproc_addr      = 0;
u32 kproc_start = 0;
u32 kproc_size  = 0;
u32 kproc_num   = 0;
u32 kproc_codeset_offset = 0;
u32 kproc_pid_offset     = 0;


void FindKProcStart()
{
    // Get the vtable* of the current application's KProcess.
    // The vtable* is the first u32 in the KProcess struct, and
    // it's constant between all KProcesses.
    u32 kproc_vtable_ptr = *(u32*)curr_kproc_addr;

    for (u32 itr_kproc_addr = curr_kproc_addr;; itr_kproc_addr -= kproc_size) {
        u32 itr_kproc_vtable_ptr = *(u32*)itr_kproc_addr;
        if (itr_kproc_vtable_ptr != kproc_vtable_ptr) {
            // If the current iteration's vtable* doesn't match, we
            // overran the KProcess list.
            kproc_start = itr_kproc_addr + kproc_size;
            return;
        }
    }
}

int ScanKProcList()
{
    curr_kproc_addr = *(u32*)0xFFFF9004;
    FindKProcStart();
    return 0;
}

void SaveVersionConstants()
{
    u32 kversion = *(vu32*)0x1FF80000; // KERNEL_VERSION register

    u8 is_n3ds = 0;
    APT_CheckNew3DS(NULL, &is_n3ds);

    if (kversion < 0x022C0600) {
        kproc_size = 0x260;
        kproc_num  = 0x2C; // TODO: Verify
        kproc_codeset_offset = 0xA8;
        kproc_pid_offset     = 0xAC;
    } else if (!is_n3ds) {
        kproc_size = 0x268;
        kproc_num  = 0x2C; // TODO: Verify
        kproc_codeset_offset = 0xB0;
        kproc_pid_offset     = 0xB4;
    } else {
        kproc_size = 0x270;
        kproc_num  = 0x2F; // TODO: Verify
        kproc_codeset_offset = 0xB8;
        kproc_pid_offset     = 0xBC;
    }

    KernelBackdoor(ScanKProcList);
}
