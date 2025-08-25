/**
 * Copyright (C), 2025 by GARY, All Rights Reserved.
 * @file 	 App_msg.c
 * @author 	 Gary (601782602@qq.com)
 * @date 	 2025-08-22 14:59:56
 * @brief
 */
#include "App_msg.h"


gate_status_t App_msg_json_2_msg( char* json , App_msg_st* msg ) {
    cJSON* root = cJSON_Parse( json );
    if (root == NULL) {
        log_error( "json 解析失败" );
        return ERROR;
    }

    cJSON* connType_json = cJSON_GetObjectItem( root , "connType" );
    cJSON* motorId_json = cJSON_GetObjectItem( root , "motorId" );
    cJSON* action_json = cJSON_GetObjectItem( root , "action" );
    cJSON* motorSpeed_json = cJSON_GetObjectItem( root , "motorSpeed" );
    cJSON* status_json = cJSON_GetObjectItem( root , "status" );

    if (connType_json == NULL || motorId_json == NULL || action_json == NULL) {
        cJSON_Delete( root );
        log_error( "json 解析失败" );
        return ERROR;
    }

    msg->connType = my_strdup( connType_json->valuestring );
    msg->motorId = motorId_json->valueint;
    msg->action = my_strdup( action_json->valuestring );

    if (status_json != NULL) {
        msg->status = my_strdup( status_json->valuestring );
    }

    if (motorSpeed_json != NULL) {
        msg->motorSpeed = motorSpeed_json->valueint;
    }

    cJSON_Delete( root );
    return OK;
}

gate_status_t App_msg_msg_2_json( char* json , App_msg_st* msg ) {
    if (json == NULL || msg == NULL) {
        return ERROR;
    }

    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject( root , "connType" , msg->connType );
    cJSON_AddStringToObject( root , "action" , msg->action );
    cJSON_AddNumberToObject( root , "motorId" , msg->motorId );
    cJSON_AddNumberToObject( root , "motorSpeed" , msg->motorSpeed );

    if (msg->status != NULL) {
        cJSON_AddStringToObject( root , "status" , msg->status );
    }

    //  by Gary: 无格式化
    char* root_str = cJSON_PrintUnformatted( root );

    //  by Gary: 将封装好的字符串 放入参数中的json
    strcpy( json , root_str );

    cJSON_Delete( root ); //  by Gary: delete会递归
    cJSON_free( root_str ); //  by Gary: 只会释放一次
    return OK;
}
