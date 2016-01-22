#include "device.h"


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
	OS_VersionBin nver_versionbin;
	OS_VersionBin cver_versionbin;
	memset(&nver_versionbin, 0, sizeof(OS_VersionBin));
	memset(&cver_versionbin, 0, sizeof(OS_VersionBin));
	ret = osGetSystemVersionData(&nver_versionbin, &cver_versionbin);
	
	device.firmwareversion.major = cver_versionbin.mainver;
	device.firmwareversion.minor = cver_versionbin.minor;
	device.firmwareversion.revision = cver_versionbin.build;
	device.firmwareversion.nver = nver_versionbin.mainver;
	return ret;
}


int setKernelVersion()
{
	u32 kernelValue = osGetKernelVersion();
	device.kernelversion = *(kernelVersion*)&kernelValue;
	return 0;
}