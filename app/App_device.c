/**
 * Copyright (C), 2025 by GARY, All Rights Reserved.
 * @file 	 App_device.c
 * @author 	 Gary (601782602@qq.com)
 * @date 	 2025-08-22 15:00:11
 * @brief
 */
#include "App_device.h"


#define BUF_ZIZE   1024
#define POOL_SIZE  10

typedef struct {
    bool is_running; // 表示设备是否在运行
    App_buffer_st* down_buf; // 下行缓冲区
    App_buffer_st* up_buf;   // 上行缓冲区
}device_handle;

static device_handle* device = NULL;


//  by Gary: mqtt 需要使用的回调函数
void mqtt_recve_task_cb( char* data , int dataLen );
//  by Gary: 线程池 add task 需要使用的回调函数
void data_upload_task_cb( void* args );

void modbus_rw_task_cb( void* args );



device_handle_st App_device_init( void ) {

    if (device != NULL) {//  by Gary: 已被分配
        return (device_handle_st)device;
    }
    device = malloc( sizeof( device_handle ) );

    device->is_running = false;
    device->up_buf = App_buffer_init( BUF_ZIZE );
    device->down_buf = App_buffer_init( BUF_ZIZE );

    //  mqtt init
    gate_status_t sta = App_mqtt_init( mqtt_recve_task_cb );
    if (sta != OK) {
        //  by Gary: 紧接着清理创建好的东西 [row:26,27]
        App_buff_deInit( device->up_buf );
        App_buff_deInit( device->down_buf );
        free( device );
        log_error( "mqtt 初始化 失败" );
        return NULL;
    }

    //  线程池初始化
    sta = App_pool_init( POOL_SIZE );
    if (sta != OK) {
        //  by Gary: 紧接着清理创建好的东西 [ row: App_buffer_init() ]
        App_buff_deInit( device->up_buf );
        App_buff_deInit( device->down_buf );
        App_mqtt_deInit(); //  by Gary: mqtt也要反初始化
        free( device );
        log_error( "线程池 初始化 失败" );
        return NULL;
    }

    sta = App_pool_add_task( data_upload_task_cb , NULL );
    if (sta != OK) {
        App_pool_deInit();
        App_mqtt_deInit();
        App_buff_deInit( device->up_buf );
        App_buff_deInit( device->down_buf );
        free( device );
        log_error( "添加上传任务 初始化 失败" );
        return NULL;
    }

    sta = App_pool_add_task( modbus_rw_task_cb , NULL );
    if (sta != OK) {
        App_pool_deInit();
        App_mqtt_deInit();
        App_buff_deInit( device->up_buf );
        App_buff_deInit( device->down_buf );
        free( device );
        log_error( "线程池 任务 初始化 失败" );
        return NULL;
    }

    //  by Gary: mobus  init  
    sta = App_modbus_init();
    if (sta != OK) {
        App_pool_deInit();
        App_mqtt_deInit();
        App_buff_deInit( device->up_buf );
        App_buff_deInit( device->down_buf );
        App_modbus_deInit();
        free( device );
        log_error( "modbus 任务 初始化 失败" );
        return NULL;
    }

    device->is_running = true;
    return (device_handle_st)device;
}

/**
 * @brief 	 mqtt 收到的回调函数
 * @param 	 data
 * @param 	 dataLen
 */
void mqtt_recve_task_cb( char* data , int dataLen ) {
    if (data == NULL || dataLen <= 0) {
        log_error( "mqtt收到空数据" );
        return;
    }
    log_debug( "mqtt收到数据%s" , data );
    App_buff_write( device->down_buf , (uint8_t*)data , dataLen );
}

/**
 * @brief 	 数据上传任务
 * @param 	 args
 * @note     和上行数据区分开
 */
void data_upload_task_cb( void* args ) {
    uint8_t data_buff[256] = { 0 };
    while (1) {
        uint8_t real_len = app_buffer_read( device->up_buf , data_buff , sizeof( data_buff ) );
        if (real_len > 0) {
            log_info( "从上行缓冲区读到数据=%.*s" , real_len , data_buff );
            data_buff[real_len] = '\0';
            App_mqtt_Send( (char*)data_buff );
        }
    }
}

/**
 * @brief 	 modbus 读写任务
 * @param 	 args
 */
void modbus_rw_task_cb( void* args ) {
    uint8_t data_buff[256] = { 0 };
    while (1) {
        //  by Gary: 清理垃圾
        memset( data_buff , 0 , sizeof( data_buff ) );
        uint8_t len = app_buffer_read( device->down_buf , data_buff , sizeof( data_buff ) );

        if (len > 0) {
            log_debug( "从下行缓冲区读到数据%.*s" , len , data_buff );
            App_msg_st msg_st;
            App_msg_json_2_msg( data_buff , &msg_st );
            log_info( "msg: %s, %s, %d, %d, %s" , msg_st.action , msg_st.connType , msg_st.motorID , msg_st.motorSpeed , msg_st.status );

            // if (strcmp( msg_st.action , "set" ) == 0) {
            if (strcmp( msg_st.status , "set" ) == 0) {
                //  by Gary: 设置寄存器
                App_modbus_write_single_hold_register( msg_st.motorID , 0 , (uint16_t)msg_st.motorSpeed );
            } else if (strcmp( msg_st.action , "get" ) == 0) {

                //  by Gary: 读输入寄存器的值
                gate_status_t err = App_modbus_read_single_input_register( msg_st.motorID , &msg_st.motorSpeed );
                if (err == OK) {
                    msg_st.status = "ok";
                } else {
                    msg_st.status = "error";
                    msg_st.motorSpeed = 0;
                }

                //  by Gary: 清除
                memset( data_buff , 0 , sizeof( data_buff ) );
                App_msg_msg_2_json( data_buff , &msg_st );
                App_buff_write( device->up_buf , data_buff , strlen( data_buff ) );
            }
        }
    }
}


bool App_device_is_running( void ) {
    return device->is_running;
}