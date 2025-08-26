#include "App_pool.h"


#define APP_POOL_MQ_NAME "/my_queue"


typedef struct {
    void (*task)(void*);
    void* arg;
}app_pool_task_st;


typedef struct {
    int thread_num; //  by Gary: 线程池中线程的数量
    pthread_t* thread_IDs; //  by Gary: 存储所有线程ID 大小未知需要后续动态创建

    mqd_t mqueue_ID;  //  by Gary: 队列ID
    char* mq_name;  //  by Gary: 队列名 char *mq_name;
}app_pool_st;

static app_pool_st pool_st;


/**
 * @brief 	 添加任务
 * @param 	 task 任务函数
 * @param 	 arg 任务参数
 * @note     将两个参数结合起来
 */
gate_status_t App_pool_add_task( void (*task)(void*) , void* arg ) {
    if (task == NULL) {
        log_error( "任务函数不能为空" );
        return ERROR;
    }

    app_pool_task_st pool_task_st = { .arg = arg , .task = task };

    if (mq_send( pool_st.mqueue_ID , (char*)&pool_task_st , sizeof( app_pool_task_st ) , 0 ) == -1) {

        log_error( "添加任务失败" );
        perror( "添加任务失败" );
        return ERROR;

    }
    return OK;
}

void* App_pool_thread_func( void* arg ) {
    while (1) {
        app_pool_task_st task_st;
        ssize_t len = mq_receive( pool_st.mqueue_ID , (char*)&task_st , sizeof( app_pool_task_st ) , NULL );
        if (len == sizeof( app_pool_task_st ) && task_st.task) {
            // if (task_st.task) {
                task_st.task( task_st.arg );
            // }
        }
    }
}



/**
 * @brief    初始化
 * @param 	 thread_num 创建多少线程
 */
gate_status_t App_pool_init( int thread_num ) {

    pool_st.thread_num = thread_num;

    pool_st.thread_IDs = malloc( thread_num * sizeof( pthread_t ) );

    if (pool_st.thread_IDs == NULL || pool_st.thread_IDs == 0) {
        log_error( "线程池分配内存失败malloc failed " );
        perror( "malloc failed " );
    }


    for (size_t i = 0; i < thread_num; i++) {//  by Gary: 创建线程
        int res = pthread_create(
            &pool_st.thread_IDs[i] , NULL ,
            App_pool_thread_func , NULL
        );
        if (res != 0) {
            log_error( "线程池中创建失败" );
            perror( "线程池中创建失败" );
            return ERROR;
        }
    }

    pool_st.mq_name = APP_POOL_MQ_NAME;

    //  by Gary: 删除消息队列
    mq_unlink( pool_st.mq_name );

    struct mq_attr attr = {
        .mq_msgsize = sizeof( app_pool_task_st ),   //  单个消息大小 :TODO
        .mq_maxmsg = thread_num,
        .mq_flags = 0    //  by Gary: 阻塞取消息
    };

    pool_st.mqueue_ID = mq_open( pool_st.mq_name , O_RDWR | O_CREAT , 0666 , &attr );
    // pool_st.mqueue_ID = mq_open( "/my_queue" , O_RDWR | O_CREAT , 0666 , &attr );

    if (pool_st.mqueue_ID == (mqd_t)-1) {
        log_error( "消息队列创建失败" );
        perror( "消息队列创建失败" );
        return ERROR;
    }

    // free( pool_st.thread_IDs );
    log_info( "线程池创建成功" );

    return OK;
}


/**
 * @brief 	 反初始化
 */
void App_pool_deInit( void ) {
    // log_error( "000" );
    for (size_t i = 0; i < pool_st.thread_num; i++) {
        // log_error( "第%d个" , i + 1 );
        pthread_cancel( pool_st.thread_IDs[i] );
    }
    // log_error( "111" );
    mq_close( pool_st.mqueue_ID );
    // log_error( "222" );
    mq_unlink( pool_st.mq_name );
    // log_error( "333" );

    free( pool_st.thread_IDs );
}
