#include "App_common.h"
#include "pthread.h"
#include "mqueue.h"




#define MY_QUEUE  "/my_queue"
#define MSG_SIZE  512



int main( void ) {

    //  by Gary: 创建消息队列
    struct mq_attr attr;
    attr.mq_maxmsg=10; //  by Gary: 最大消息数
    attr.mq_msgsize=MSG_SIZE;
    attr.mq_flags=0; //  by Gary: 阻塞读取数据
    mq_open( MY_QUEUE , O_CREAT | O_RDWR ,0666, &attr);


    return 0;
}