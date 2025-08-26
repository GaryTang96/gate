#include "ota_http.h"


/**
 * @brief
 * @param 	 ptr 	传进来的数据指针
 * @param 	 size 	单个数据块大小
 * @param 	 nmemb 	数据块数量
 * @param 	 userdata
 * @return 	 size_t
 */
size_t get_text_cb( char* ptr , size_t size , size_t nmemb , void* userdata ) {

    size_t len = size * nmemb;
    memcpy( userdata , ptr , len );
    ((uint8_t*)userdata)[len] = '\0';
    return len;
}


uint8_t* ota_http_getText( uint8_t* url ) {
    CURL* curl;
    CURLcode res;

    // 设置全局环境(整个app只需要调用一次)
    curl_global_init( CURL_GLOBAL_DEFAULT );

    // 初始化curl
    curl = curl_easy_init();

    if (curl == NULL) {
        log_error( "curl_easy_init() failed" );
        return NULL;
    }

    // 设置要访问的url地址
    curl_easy_setopt( curl , CURLOPT_URL , url );

    // 设置回调函数, 用来接收网站的响应的数据
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , get_text_cb );

    // 设置回调函数的参数
    uint8_t* buffer = malloc( 1024 );
    memset( buffer , 0 , 1024 );
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , buffer );

    res = curl_easy_perform( curl ); // 阻塞式请求

    if (res != CURLE_OK)
        fprintf( stderr , "curl_easy_perform() failed: %s\n" ,
            curl_easy_strerror( res ) );

    // printf("%s\n", buffer);
    // fclose(fp);
    curl_easy_cleanup( curl );

    return buffer;
}
