#pragma once

#include <3ds.h>

#include <string>
#include <stdio.h>
#include <string.h>

//System Contands
typedef struct kernelVersionStruct
{
    u8 unknown;
    u8 revision;
    u8 minor;
    u8 major;
}kernelVersion;

typedef struct firmwareVersionStruct
{
    u8 major;
    u8 minor;
    u8 revision;
    u8 nver;
}firmwareVersion;

typedef struct devicesStruct
{
    u32 old3DS : 1;
    u32 old3DSXL : 1;
    u32 old2DS : 1;
    u32 new3DS : 1;
    u32 new3DSXL : 1;
    u32 placeholder : 3;
} devices;

extern u32 curr_kproc_addr;
extern u32 kproc_start;
extern u32 kproc_size;
extern u32 kproc_num;
extern u32 kproc_codeset_offset;
extern u32 kproc_pid_offset;

void SaveVersionConstants();

//Application Constands 
static const std::string applicationFolder = "sdmc:/fmp/";

static const std::string patchesFolder = applicationFolder+"patches/";
static const std::string patchExtension = ".patch";
static const std::string patchCollectionExtension = ".collection";

static const std::string settingsFolder=applicationFolder+"settings/";
static const std::string settingsFileName="settings.cfg";

static const std::string versionCheckUrl = "http://fmp.hartie95.de/version.txt";
static const u32 version=0x006025;

//Device properties
extern u8 modelID;
extern kernelVersion kernelversion;
