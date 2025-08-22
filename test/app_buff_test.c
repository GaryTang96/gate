#include "App_buffer.h"





int main( void ) {

    App_buffer_st* buff = App_buffer_init( 20 );

    App_buff_write( buff , "hello0" , 6 );
    App_buff_write( buff , "hello1" , 6 );
    App_buff_write( buff , "hello2" , 6 );
    App_buff_write( buff , "hello3" , 6 );

    uint8_t data[128];
    uint8_t dalen = app_buffer_read( buff , data , sizeof( data ) );
    log_debug( "%.*s" , dalen , data );

    dalen = app_buffer_read( buff , data , sizeof( data ) );
    log_debug( "%.*s" , dalen , data );

    dalen=app_buffer_read( buff , data , sizeof( data ) );
    log_debug( "%.*s" , dalen , data );

    return 0;
}
