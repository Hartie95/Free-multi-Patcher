#pragma once

#include <3ds.h>

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

//Australia is not used, instead it uses European region
typedef struct regionsStruct
{
	u32 japan : 1;
	u32 northAmerica : 1;
	u32 europe : 1;
	u32 australia : 1;
	u32 china : 1;
	u32 korea : 1;
	u32 taiwan : 1;
	u32 placeholder : 1;
} regions;

typedef struct deviceInformationsStruct
{
	u8 modelID;
	u8 region;
	kernelVersion kernelversion;
	firmwareVersion firmwareversion;
} deviceInformations;


extern deviceInformations device;

int initDeviceInformations();

int setModelID();
int setDeviceRegion();
int setFirmwareVersion();
int setKernelVersion();
