#include "App_common.h"
#include "pthread.h"
#include "mqueue.h"


#define MY_QUEUE  "/my_queue"
#define MSG_SIZE  (sizeof(int))


mqd_t mq_ID;


void* thread_func( void* arg ) {
    char buff[MSG_SIZE];
    //  by Gary: 1.id  2.缓冲区  3.缓冲区大小  4.获取优先级想要了就创建变量存储,不想要就不管
    ssize_t real_len = mq_receive( mq_ID , buff , MSG_SIZE , NULL );
    if (real_len == (ssize_t)-1) {
        log_error( "receive error" );
        perror( "receive error" );
    }
    log_info( "receive %.*s" , real_len , buff );
}

int main( void ) {

    //  by Gary: 创建消息队列
    struct mq_attr attr;
    attr.mq_maxmsg = 10; //  by Gary: 最大消息数
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_flags = 0; //  by Gary: 阻塞读取数据
    attr.mq_curmsgs = 0;
    mq_ID = mq_open( MY_QUEUE , O_CREAT | O_RDWR , 0666 , &attr );

    pthread_t thread_ID;
    pthread_create( &thread_ID , NULL , thread_func , NULL );

    //  by Gary: 想消息队列写入数据
    char* msg = "helo"; //  5 个字节
    if (mq_send( mq_ID , msg , strlen( msg ) , 0 ) == -1) {
        log_error( "send error" );
        perror( "send error" );
        return 0;
    }
    log_info( "send good" );

    //  by Gary: 等待现成的原因是: 处理回调函数的逻辑防止main函数提前关闭
    pthread_join( thread_ID , NULL );
    //  by Gary: 关闭消息队列
    mq_close( mq_ID );
    //  by Gary: 删除消息队列
    mq_unlink( MY_QUEUE );


    return 0;
}