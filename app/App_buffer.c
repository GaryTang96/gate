#include "App_buffer.h"


/**
 * @brief 	 初始化子缓冲区
 * @param 	 capacity
 * @return 	 sub_buffer_st*
 */
static sub_buffer_st* App_init_sub_buffer( uint16_t capacity ) {
    sub_buffer_st* subBuff = (sub_buffer_st*)malloc( sizeof( sub_buffer_st ) );

    if (subBuff == NULL) {
        log_error( "分配 初始化子 失败" );
        perror( "分配 初始化子 失败" );
        return NULL;
    }

    //  by Gary: 给subBuff赋值 初始化 内容
    subBuff->ptr = (uint8_t*)malloc( capacity );
    subBuff->capacity = capacity;
    subBuff->len = 0;

    memset( subBuff->ptr , 0 , capacity );//  by Gary:  (tar , value , len)

    return subBuff;
}

App_buffer_st* App_buffer_init( uint16_t capacity ) {

    //  by Gary: 先申请内存
    App_buffer_st* buffer = (App_buffer_st*)malloc( sizeof( App_buffer_st ) );
    if (buffer == NULL) {
        log_error( "申请内存失败" );
        perror( "申请内存失败" );
        return NULL;
    }

    buffer->sub_buffer_t[0] = App_init_sub_buffer( capacity );
    buffer->sub_buffer_t[1] = App_init_sub_buffer( capacity );

    if (buffer->sub_buffer_t[0] == NULL || buffer->sub_buffer_t[1] == NULL) {
        log_error( "app_init_sub_buffer failed" );
        perror( "app_init_sub_buffer failed" );
        return NULL;
    }

    buffer->r_index = 0; //  by Gary: 规定 0 -> read
    buffer->w_index = 1;

    pthread_mutex_init( &buffer->r_lock , NULL ); //  by Gary: 初始化读写锁
    pthread_mutex_init( &buffer->w_lock , NULL );

}

void App_buff_deInit( App_buffer_st* buffer ) {
    free( buffer->sub_buffer_t[0]->ptr );
    free( buffer->sub_buffer_t[0] );

    free( buffer->sub_buffer_t[1]->ptr );
    free( buffer->sub_buffer_t[1] );

    free( &buffer->r_lock );
    free( &buffer->w_lock );

    free( buffer );
}

/**
 * @brief 写数据
 * @param buffer 缓冲区
 * @param data 数据
 * @param len 数据长度
 * @return gate_status_t 成功返回OK 失败返回ERRROR
 */
gate_status_t App_buff_write( App_buffer_st* buffer , uint8_t* data , uint8_t data_len ) {
    pthread_mutex_lock( &buffer->w_lock );
    //  by Gary: 写缓冲的地方,因为规定w_index是写的地址
    sub_buffer_st* subBuf = buffer->sub_buffer_t[buffer->w_index];
    //  by Gary: 总容量 减去 已经储存的长度 >= (现data长度 + 长度本身[一个字节]), 代表能存下
    //  by Gary:                          <                                  代表存不下
    if (subBuf->capacity - subBuf->len < (data_len + 1)) {
        log_error( "缓冲区剩余空间不足: 剩余空间: %d, 需要空间: %d" ,
            subBuf->capacity - subBuf->len , data_len + 1 );
        pthread_mutex_unlock( &buffer->w_lock );
        return ERROR;
    }

    //  by Gary: 首先获得本次需要写的长度 先将长度存起来一个字节
    subBuf->ptr[subBuf->len] = data_len;
    //  by Gary: +1 代表的是data_len这个字节
    memcpy( subBuf->ptr + subBuf->len + 1 , data , data_len );
    //  by Gary: +1 代表的是data_len这个字节
    subBuf->len = data_len + 1;

    pthread_mutex_unlock( &buffer->w_lock );
    log_info( "写缓冲成功,已释放锁" );
    return OK;
}


/**
 * @brief 读取缓冲区数据
 * @param main_buffer 缓冲区指针
 * @param data 读取到的数据
 * @param data_capacity 能够读取的数据最大长度
 * @return uint8_t 读取到的数据实际长度
 */
uint8_t app_buffer_read( App_buffer_st* master_buffer , uint8_t* data , int data_read_max_capacity ) {
    //  by Gary: 上锁
    pthread_mutex_lock( &master_buffer->r_lock );
    //  by Gary: 找到写缓冲
    sub_buffer_st* sub_buf = master_buffer->sub_buffer_t[master_buffer->r_index];
    if (sub_buf->len == 0) {//  by Gary: 读完了需要交换缓冲区  写满了直接进行报错
        pthread_mutex_lock( &master_buffer->w_lock );
        uint16_t temp = master_buffer->r_index;
        master_buffer->r_index = master_buffer->w_index;
        master_buffer->w_index = temp;
        pthread_mutex_unlock( &master_buffer->w_lock );
        //  by Gary: 交换完成重新赋值
        sub_buf = master_buffer->sub_buffer_t[master_buffer->r_index];
        if (sub_buf->len == 0) {
            log_error( "缓冲区为空" );
            pthread_mutex_unlock( &master_buffer->r_lock );
            return 0;
        }
    }

    //  by Gary: 一组数据读完之后 首字节一定是下一个数据的总长度(因为写的时候就是这样写的)
    uint8_t real_data_len = sub_buf->ptr[0];
    if (real_data_len > data_read_max_capacity) {
        log_error( "数据长度超出,可读取存储的量" );
        pthread_mutex_unlock( &master_buffer->r_lock );
        return 0;
    }

    //  by Gary: 真正的数据读开始
    memcpy( data , sub_buf->ptr + 1 , real_data_len );

    //  by Gary: 将后面的数据进行前移 source: 指针长度向后位移real_data_len + 1 
    memmove( sub_buf->ptr , sub_buf->ptr + real_data_len + 1 , real_data_len + 1 );//different with teacher

    //  by Gary: 位移了多少 更新一下新已存储长度的长度
    sub_buf->len -= real_data_len + 1; 

    pthread_mutex_unlock( &master_buffer->r_lock );
    return real_data_len;
}
