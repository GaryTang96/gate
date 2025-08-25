/**
 * Copyright (C), 2025 by GARY, All Rights Reserved.
 * @file 	 App_modbus.c
 * @author 	 Gary (601782602@qq.com)
 * @date 	 2025-08-23 16:53:10
 * @brief 	 
 */
#include "App_modbus.h"


modbus_t* modbus;

/**
 * @brief 	 初始化
 * @return 	 gate_status_t
 */
gate_status_t App_modbus_init( void ) {

    if (modbus) {
        return OK;
    }

    modbus = modbus_new_rtu( "/dev/ttyS1" , 115200 , 'N' , 8 , 1 );

    if (modbus == NULL) {
        log_error( "modbus上下文创建失败" );
        return ERROR;
    }

    //  by Gary: 启用调试模式 
    modbus_set_debug( modbus , true );

    //  by Gary: 建立连接  打开串口 --- 相当于uartenable
    if (modbus_connect( modbus ) == -1) {
        log_error( "创建连接失败" );
        return ERROR;
    }

    return OK;
}

/**
 * @brief 	 反初始化
 * @return 	 gate_status_t
 */
gate_status_t App_modbus_deInit (void){
    modbus_close(modbus);
    modbus_free(modbus);
}


/**
 * @brief 	 写单个保持寄存器
 * @param 	 slaveAddr  从机地址
 * @param 	 reg_num  哪一个寄存器
 * @param 	 w_value  要写的值
 * @return 	 gate_status_t
 */
gate_status_t App_modbus_write_single_hold_register( uint8_t slaveAddr , uint8_t reg_num , uint16_t w_value ) {
    //  by Gary: 设置从机地址
    modbus_set_slave( modbus , slaveAddr );

    //  by Gary: 发送数据
    int err = modbus_write_register( modbus , reg_num , w_value );
    if (err == -1) {
        log_error( "发送数据失败" );
        return ERROR;
    }

    return OK;
}

/**
 * @brief 	 读输入寄存器地址
 * @param 	 uint8_t slaveAddr
 * @param 	 uint16_t* r_value
 * @return 	 gate_status_t
 */
gate_status_t App_modbus_read_single_input_register( uint8_t slaveAddr , uint16_t* r_value ) {
    //  by Gary: 设置从机地址
    modbus_set_slave( modbus , slaveAddr );

    // 2. 读取数据  参数2: 寄存器地址 参数3: 读取的寄存器数量 参数4: 存放读取数据的指针
    int err = modbus_read_input_registers( modbus , slaveAddr , 1 , r_value );
    if (err == -1) {
        log_error( "读取数据失败" );
        return ERROR;
    }

    return OK;
}