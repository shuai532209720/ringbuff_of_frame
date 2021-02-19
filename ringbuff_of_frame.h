#ifndef __RINGBUFF_FRAME_HH
#define __RINGBUFF_FRAME_HH

#ifdef __cplusplus
extern "C"{
#endif

//Ringbuff长度，至少大于1帧数据长度
#define BUFFER_SIZE 1024

struct ringbuff
{
    /* data */
    char buffer[BUFFER_SIZE];
    char *write_ptr;
    char *read_ptr;
    char *buffer_end_ptr;
    int total_size;
    int left_size;
};

/**
 * 初始化ringbuff
 */ 
int init_ringbuff(void);

/**
 * 向ringbuff中写入数据
 * data -> 数据首地址
 * length -> 数据长度
 * return -> -1: 写入数据大于剩余缓冲区; 0: 写入成功 
 */
int write_data_to_ringbuff(const char *data, int length);


/**
 * 从ringbuff中读取一帧数据
 * data -> 接收缓冲区首地址
 * size -> 接收缓冲区长度
 * return -> -1: 出现错误; 0: 未找到，等待接收; > 0: 帧长 
 */
int read_frame_from_ringbuff(char *data, int size);

#ifdef __cplusplus
}
#endif

#endif
