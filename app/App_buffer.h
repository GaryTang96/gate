#ifndef __APP_BUFFER_H__
#define __APP_BUFFER_H__


#include "App_common.h"
#include "pthread.h"


typedef struct {
    //  by Gary: 缓冲区指针
    uint8_t* ptr;

    uint16_t capacity;   //  by Gary: 缓冲区 容纳多少字节

    uint16_t len;    //  by Gary: 已储存的数据长度
}sub_buffer_st;


typedef struct {
    sub_buffer_st *sub_buffer_t[2];

    uint16_t r_index;
    uint16_t w_index;

    //  by Gary: 锁
    pthread_mutex_t w_lock;
    pthread_mutex_t r_lock;
}App_buffer_st;


App_buffer_st* App_buffer_init( uint16_t capacity );

void App_buff_deInit( App_buffer_st* buffer );


#endif
