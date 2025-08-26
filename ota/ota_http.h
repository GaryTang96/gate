#ifndef __OTA_HTTP_H__
#define __OTA_HTTP_H__

#include "App_common.h"
#include "curl/curl.h"



uint8_t* ota_http_getText( uint8_t* url );

gate_status_t ota_http_download( uint8_t* url , uint8_t path_file );


#endif