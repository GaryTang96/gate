#include "App_mqtt.h"


typedef struct {
    MQTTClient mqttClient;
    void(*recve_cb)(char* data , int dataLen);
}App_mqtt_t;
App_mqtt_t app_mqtt_t;


#define SERVER_URI "tcp://192.168.56.56:1883"
#define CLIENT_ID "garygate"
#define TOPIC_PULL "pull"
#define TOPIC_PUSH "push"



/***********************   call back start   **********************/
/**
 * @brief 	 失去连接回调
 * @param 	 context
 * @param 	 cause
 */
void connectionLost_cb( void* context , char* cause ) {
    while (1) {//  by Gary: 连接server
        MQTTClient_connectOptions connectOptions = MQTTClient_connectOptions_initializer;
        if (MQTTClient_connect( app_mqtt_t.mqttClient , &connectOptions ) != MQTTCLIENT_SUCCESS) {

            log_error( "mqtt  Reconnect fail" );

        }
        log_info( "mqtt  Reconnect ok" );

        //  by Gary: 订阅
        if (MQTTClient_subscribe( app_mqtt_t.mqttClient , TOPIC_PULL , MQTTREASONCODE_GRANTED_QOS_0 ) != MQTTCLIENT_SUCCESS) {

            log_error( "mqtt Resubscribe TOPIC: %s fail" , TOPIC_PULL );

        } else {
            log_info( "mqtt Resubscribe TOPIC: %s ok" , TOPIC_PULL );
            break;
        }
    }
}

/**
 * @brief 	 信息到达
 * @param 	 context
 * @param 	 topicName
 * @param 	 topicLen
 * @param 	 message
 */
int messageArrived_cb( void* context , char* topicName , int topicLen , MQTTClient_message* message ) {
    // log_error( "topicName=%s , msg=%s" , topicName , message->payload );

    if (app_mqtt_t.recve_cb) {
        app_mqtt_t.recve_cb( message->payload , message->payloadlen );
    }


    //  by Gary: 必须释放
    MQTTClient_freeMessage( &message );
    MQTTClient_free( topicName );
    //  by Gary: 消息正确处理
    return 1;
}

/**
 * @brief
 * @param 	 context
 * @param 	 dt
 */
void deliveryComplete_cb( void* context , MQTTClient_deliveryToken dt ) {
    log_info( "delivery_complete_cb: %d" , dt );
}
/***********************   call back  end  **********************/



/**
 * @brief 	 初始化
 */
gate_status_t App_mqtt_init( void(*recve_cb)(char* data , int dataLen) ) {

    app_mqtt_t.recve_cb = recve_cb;//  by Gary: 保存回调函数

    //  by Gary: 创建 mqtt client
    if (MQTTClient_create(
        &app_mqtt_t.mqttClient ,
        SERVER_URI ,
        CLIENT_ID ,
        MQTTCLIENT_PERSISTENCE_NONE ,
        NULL
    ) != MQTTCLIENT_SUCCESS) {

        log_error( "mqtt client create fail" );
        return ERROR;

    }
    log_info( "mqtt client create ok" );

    //  by Gary: 设置回调
    if (MQTTClient_setCallbacks(
        app_mqtt_t.mqttClient ,
        NULL ,
        connectionLost_cb ,
        messageArrived_cb ,
        deliveryComplete_cb
    ) != MQTTCLIENT_SUCCESS) {

        log_error( "mqtt client setCallback fail" );
        return ERROR;

    }
    log_info( "mqtt client setCallback ok" );

    //  by Gary: 连接server
    MQTTClient_connectOptions connectOptions = MQTTClient_connectOptions_initializer;
    if (MQTTClient_connect( app_mqtt_t.mqttClient , &connectOptions ) != MQTTCLIENT_SUCCESS) {

        log_error( "mqtt client connect fail" );
        perror( "mqtt client connect fail" );
        return ERROR;

    }
    log_info( "mqtt client connect ok" );

    //  by Gary: 订阅
    if (MQTTClient_subscribe( app_mqtt_t.mqttClient , TOPIC_PULL , MQTTREASONCODE_GRANTED_QOS_0 ) != MQTTCLIENT_SUCCESS) {

        log_error( "mqtt subscribe TOPIC: %s fail" , TOPIC_PULL );
        return ERROR;

    }
    log_info( "mqtt subscribe TOPIC: %s ok" , TOPIC_PULL );

    return OK;
}

/**
 * @brief 	 发送消息
 * @param 	 str  data
 * @return 	 gate_status_t
 */
gate_status_t App_mqtt_Send( char* str ) {
    if (MQTTClient_publish(
        app_mqtt_t.mqttClient ,
        TOPIC_PUSH ,
        strlen( str ) ,
        (void*)str ,
        1 ,
        0 ,
        NULL
    ) != MQTTCLIENT_SUCCESS) {
        log_error( "mqtt Send fail" );
        return ERROR;
    }
    log_info( "mqtt Send ok  dataLen=%d" , strlen( str ) );

    return OK;
}

gate_status_t App_mqtt_deInit( void ) {
    //  by Gary: 取消订阅
    MQTTClient_unsubscribe( app_mqtt_t.mqttClient , TOPIC_PULL );
    //  by Gary: 断开连接
    MQTTClient_disconnect( app_mqtt_t.mqttClient , 2000 );
    //  by Gary: 销毁客户端
    MQTTClient_destroy( &app_mqtt_t.mqttClient );

    return OK;
}
