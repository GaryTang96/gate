#include "ota_version.h"


#define OTA_VERSION_INFO_URL "http://192.168.56.56:8000/server/gateway_version_info.json"

#define OTA_SW_URL ""
#define OTA_SW_TMP_PATH ""


ota_version_st* ota_version_parse_version_info( char* json ) {
    log_error( "json=%s" , json );
    cJSON* root = cJSON_Parse( json );
    if (root == NULL) {
        log_error( "cjson parse failed." );
        return NULL;
    }
    ota_version_st* version = (ota_version_st*)malloc( sizeof( ota_version_st ) );

    version->major = cJSON_GetObjectItem( root , "major" )->valueint;
    version->minor = cJSON_GetObjectItem( root , "minor" )->valueint;
    version->patch = cJSON_GetObjectItem( root , "patch" )->valueint;

    if (cJSON_GetObjectItem( root , "hash" )) {
        version->hash = my_strdup( cJSON_GetObjectItem( root , "hash" )->valuestring );
    }

    cJSON_Delete( root );

    return version;
}



gate_status_t ota_version_check( void ) {
    uint8_t* version_info_json = ota_http_getText( OTA_VERSION_INFO_URL );
    log_debug( "version_info_json%s" , version_info_json );
    ota_version_st* new_version = ota_version_parse_version_info( version_info_json );
    printf( "new version: %d.%d.%d\n" , new_version->major , new_version->minor , new_version->patch );

    ota_version_st* curr_version = ota_version_parse_version_info( CURR_VERSION_INFO );
    printf( "curr version: %d.%d.%d\n" , curr_version->major , curr_version->minor , curr_version->patch );

    //  by Gary: 不需要更新的情况
    if (new_version->major < curr_version->major ||
        (new_version->major == curr_version->major && new_version->minor <= curr_version->minor) ||
        (new_version->major == curr_version->major && new_version->minor == curr_version->minor && new_version->patch <= curr_version->patch)
        ) {
        log_info( "已是最新版本,不需要更新" );
        return ERROR;
    } else {
        log_info( "开始更新" );
        gate_status_t sta = ota_http_download( OTA_SW_URL , OTA_SW_TMP_PATH );
        if (sta != OK) {
            log_error( "下载失败" );
            return sta;
        }

    }

    return OK;
}