#ifndef __APP_POOL_H__
#define __APP_POOL_H__


#include "App_common.h"
#include "pthread.h"
#include "mqueue.h"


gate_status_t App_pool_init( int thread_num );

void App_pool_deInit( void );

gate_status_t App_pool_add_task( void (*task)(void*) , void* arg );



#endif
