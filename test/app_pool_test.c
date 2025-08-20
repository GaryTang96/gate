#include "App_pool.h"


void task( void* arg ) {

    log_debug( "task%d" , *(size_t*)arg );
}



int main( void ) {

    App_pool_init( 10 );

    for (size_t i = 0; i < 200000; i++) {
        App_pool_add_task( task , &i );

    }
    sleep( 50 );


    return 0;
}
