#pragma once

#include <3ds.h>

#include <ctrcommon/input.hpp>
#include <stdio.h>
#include <string.h>

static const std::string applicationFolder = "sdmc:/fmp/";
static const std::string patchesFolder = applicationFolder+"patches/";
static const std::string patchExtension =".patch";
static const std::string settingsFolder=applicationFolder;
static const std::string settingsFileName="settings.cfg";

extern u32 curr_kproc_addr;
extern u32 kproc_start;
extern u32 kproc_size;
extern u32 kproc_num;
extern u32 kproc_codeset_offset;
extern u32 kproc_pid_offset;

void SaveVersionConstants();