#pragma once

#include <3ds.h>

#include <ctrcommon/input.hpp>
#include <stdio.h>
#include <string.h>
//Application Constands 
static const std::string applicationFolder = "sdmc:/fmp/";
static const std::string patchesFolder = applicationFolder+"patches/";
static const std::string patchExtension =".patch";
static const std::string settingsFolder=applicationFolder+"settings/";
static const std::string settingsFileName="settings.cfg";
static const u32 version=0x006010;

//System Contands
typedef struct kernelVersionStruct
{
    u8 unknown ;
    u8 revision;
    u8 minor;
    u8 major;
}kernelVersion;

extern u32 curr_kproc_addr;
extern u32 kproc_start;
extern u32 kproc_size;
extern u32 kproc_num;
extern u32 kproc_codeset_offset;
extern u32 kproc_pid_offset;

void SaveVersionConstants();