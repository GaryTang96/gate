#ifndef __APP_COMMON_H__
#define __APP_COMMON_H__

#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "log.h"
#include "unistd.h"


typedef enum{
    OK=0,
    ERROR,
    TIMEOUT,
    OTHERERROR
} gate_status_t;


char* my_strdup( const char* s );

#endif
