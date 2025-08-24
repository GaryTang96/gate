#ifndef __APP_DEVICE_H__
#define __APP_DEVICE_H__

#include "App_common.h"
#include "App_mqtt.h"
#include "App_buffer.h"
#include "App_pool.h"
#include "App_msg.h"
#include "App_modbus.h"


typedef void*  device_handle_st;

device_handle_st App_device_init(void);

bool App_device_is_running( void );


#endif
