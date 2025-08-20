#ifndef __APP_MQTT_H__
#define __APP_MQTT_H__

#include "App_common.h"
#include "cJSON.h"
#include "MQTTClient.h"


gate_status_t App_mqtt_init( void(*recve_cb)(char* data , int dataLen) );

gate_status_t App_mqtt_Send( char* str );




#endif
