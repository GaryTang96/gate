#ifndef __APP_MSG_H__
#define __APP_MSG_H__

#include "App_common.h"
#include "cJSON.h"

typedef struct {
    char* connType;
    uint8_t motorID;
    char* action;
    int16_t motorSpeed;
    char* status;
}App_msg_st;

gate_status_t App_msg_json_2_msg( char* json , App_msg_st* msg );

gate_status_t App_msg_msg_2_json( char* json , App_msg_st* msg );

#endif
