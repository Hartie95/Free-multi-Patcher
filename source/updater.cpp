#include "updater.h"
#include "constants.h"
#include <stdlib.h>


int checkVersion()
{
    int onlineVersion = version;
    httpcContext context;
    Result ret = 0;
    u32 statuscode = 0;
    char * url = (char* )versionCheckUrl.c_str();

    

    ret = httpcOpenContext(&context, url, 0);
    if (ret == 0)
    {
        ret = httpcBeginRequest(&context);
        if (ret == 0)
        {
            ret = httpcGetResponseStatusCode(&context, &statuscode, 0);
            if (ret == 0 && statuscode == 200)
            {
                u32 contentsize = 0;
                ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
                if (ret == 0)
                {
                    u8* buf = (u8*)malloc(contentsize);
                    memset(buf, 0, contentsize);
                    ret = httpcDownloadData(&context, buf, contentsize, NULL);
                    if (ret == 0)
                    {
                        char versionString[contentsize + 1];
                        for (u32 i = 0; i<contentsize; i++)
                        {
                            versionString[i] = (char)buf[i];
                        }
                        versionString[contentsize] = '\0';
                        onlineVersion = (int)strtol(versionString, NULL, 16);
                        httpcCloseContext(&context);
                    }
                    free(buf);
                }
            }
        }
    }
    return onlineVersion;
}

int checkForUpdate()
{
    u32 onlineVersion = checkVersion();
    if (version<onlineVersion)
    {
        const char title8[] = { "A New Version is available" };
        const char message8[] = { "A new version is available, please download it from http://fmp.hartie95.de/releases" };
        u16 title16[sizeof(title8)];
        u16 message16[sizeof(message8)];
        for (u32 i = 0; i<sizeof(title8); i++)
        {
            title16[i] = (u16)title8[i];
        }
        for (u32 i = 0; i<sizeof(message8); i++)
        {
            message16[i] = (u16)message8[i];
        }
        
        NEWSU_AddNotification(title16, sizeof(title8), message16, sizeof(message8), nullptr, 0, false);
    }
    return onlineVersion;
}
