#ifndef __OTA_VERSION_H__
#define __OTA_VERSION_H__

#include "App_common.h"
#include "ota_http.h"
#include "cJSON.h"
#include <openssl/sha.h>


#define CURR_VERSION_INFO "{\"major\": 1, \"minor\": 1, \"patch\": 0}"


typedef struct {
    int major;
    int minor;
    int patch;
    char* hash;
} ota_version_st;



gate_status_t ota_version_check( void );


#endif
