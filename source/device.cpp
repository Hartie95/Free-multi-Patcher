#include "device.h"
#include "archive.h"


#include <string>
#include <stdio.h>
#include <string.h>

using namespace std;

deviceInformations device;


int initDeviceInformations()
{
	device.modelID = 255;
	device.region = 255;
	device.kernelversion = { 0, 0, 0, 0 };
	device.firmwareversion = { 0,0,0,0 };

	setModelID();
	setDeviceRegion();
	setFirmwareVersion();
	setKernelVersion();

	return 0;
}

int setModelID()
{
	return CFGU_GetSystemModel(&device.modelID);
	
}

int setDeviceRegion()
{
	return CFGU_SecureInfoGetRegion(&device.region);
}


int setFirmwareVersion()
{
	Result ret = 0;
	u8 nver_versionbin[0x8];
	u8 cver_versionbin[0x8];
	
	ret = readNver(nver_versionbin, sizeof(nver_versionbin));
	ret = readCver(cver_versionbin, sizeof(cver_versionbin));

	device.firmwareversion.major = cver_versionbin[2];
	device.firmwareversion.minor = cver_versionbin[1];
	device.firmwareversion.revision = cver_versionbin[0];
	device.firmwareversion.nver = nver_versionbin[2];
	return ret;
}


int setKernelVersion()
{
	u32 kernelValue = osGetKernelVersion();
	device.kernelversion = *(kernelVersion*)&kernelValue;
	return 0;
}