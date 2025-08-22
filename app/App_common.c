#include "App_common.h"

char* my_strdup( const char* s ) {
    if (s == NULL) return NULL;

    size_t len = strlen( s ) + 1;        // 字符串长度 + '\0'
    char* dup = (char*)malloc( len );   // 分配内存
    if (dup == NULL) return NULL;      // 检查分配是否成功

    memcpy( dup , s , len );               // 拷贝字符串（包括末尾的 '\0'）
    return dup;
}
