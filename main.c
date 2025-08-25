#include "App_device.h"



int main( int argc , char const* argv[] ) {
    device_handle_st dev = App_device_init();

    if (dev == NULL) {
        log_fatal( "device初始化失败" );
        return -1;
    }

    log_info( "device初始化成功" );

    while (App_device_is_running) {
        log_info( "device运行中...." );
        sleep( 2 );
    }

    return 0;
}
