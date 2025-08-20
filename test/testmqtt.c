#include "App_mqtt.h"

void mqtt_arrive_cb( char* data , int dataLen ) {
    log_info( "mqtt_arrive_cb  data=%s,len=%d" , data , dataLen );
}
int main( void ) {

    App_mqtt_init( mqtt_arrive_cb );
    while (1) {
        App_mqtt_Send("978465123");
        sleep(2);
    }

    return 0;
}