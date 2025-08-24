#ifndef __APP_MODBUS_H__
#define __APP_MODBUS_H__

#include "App_common.h"
#include "modbus/modbus.h"


gate_status_t App_modbus_init( void );

gate_status_t App_modbus_deInit( void );

gate_status_t App_modbus_write_single_hold_register( uint8_t slaveAddr , uint8_t reg_num , uint16_t w_value );

gate_status_t App_modbus_read_single_input_register( uint8_t slaveAddr , uint16_t* r_value );


#endif
