#include "App_runner.h"

/**
 * @brief 	 按ctrl + c执行的函数
 * @param 	 signal
 */
void sig_exit( int signal ) {
    log_info( "退出信号" );
    App_device_set_running( false );
}

void App_runner_run( void ) {

    device_handle_st dev = App_device_init();

    if (dev == NULL) {
        log_fatal( "device初始化失败" );
    }

    log_info( "device初始化成功" );

    signal( SIGINT , sig_exit );//  by Gary: => ctrl+c
    signal( SIGTERM , sig_exit );//  by Gary: => kill pid

    // bool b = App_device_is_running;
    while (App_device_is_running()) {
        log_info( "device运行中...." );
        sleep( 2 );
    }

    log_info( "释放" );
    App_device_deInit();
}
