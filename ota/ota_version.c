#include "ota_version.h"


#define OTA_VERSION_INFO_URL "http://192.168.56.56:8000/gateway_version_info.json"


void ota_version_check( void ) {
    uint8_t* version_info_json = ota_http_getText( OTA_VERSION_INFO_URL );
    printf( "OTA_VERSION_INFO_URL=%s\n" , version_info_json );
}