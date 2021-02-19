/*
 * @Descripttion : 
 * @Author       : 
 * @Date         : 2021-01-04 03:30:24
 * @LastEditTime : 2021-01-04 09:46:06
 */
#include <stdio.h>
#include <string.h>
#include "ringbuff_frame.h"


static struct ringbuff cache;

int init_ringbuff(void)
{
    memset(&cache, 0, sizeof(cache));
    cache.read_ptr = cache.buffer;
    cache.write_ptr = cache.buffer;
    cache.left_size = sizeof(cache.buffer);
    cache.total_size = sizeof(cache.buffer);
    cache.buffer_end_ptr = cache.buffer + cache.total_size;
}

int write_data_to_ringbuff(const char *data, int length)
{
    int left_size;
    int left_frame;

    if (NULL == data)
        goto err;
    if (length > cache.left_size) //此处可以等待锁，以保证线程安全。
        goto err;
    if (cache.write_ptr + length <= cache.buffer_end_ptr)
    {
        memcpy(cache.write_ptr, data, length);
        cache.write_ptr += length;
    }
    else
    {
        /* code */
        left_size = (int)(cache.buffer_end_ptr - cache.write_ptr);
        memcpy(cache.write_ptr, data, left_size);
        left_frame = length - left_size;
        memcpy(cache.buffer, data + left_size, left_frame);
        cache.write_ptr = cache.buffer + left_frame;
    }
    cache.left_size -= length;

    return 0;

err:
    printf("write error\r\n");
    return -1;
}

int read_frame_from_ringbuff(char *data, int size)
{
    char *ptr = NULL;
    int frame_length = 0;
    int temp = 0;

    if (!data)
    {
        printf("data is null\r\n");
        return -1;
    }

    if (cache.left_size >= cache.total_size)
    {
        printf("cache.left_size >= cache.total_size\r\n");
        return -1;
    }

    if (cache.read_ptr < cache.write_ptr)
    {
        printf("cache.left_size < cache.total_size\r\n");
        for (ptr = cache.read_ptr; ptr < cache.write_ptr - 1; ptr++)
        {
            if ((*ptr == 0x0D) && (*(ptr + 1) == 0x0A))
            {
                frame_length = (int)(ptr + 2 - cache.read_ptr);
                printf("%ld, %ld, %d\r\n", ptr + 2, cache.read_ptr, frame_length);
                if (frame_length > size)
                {
                    return -1;
                }
                memcpy(data, cache.read_ptr, frame_length);
                memset(cache.read_ptr, 0, frame_length);
                cache.read_ptr += frame_length;
                cache.left_size += frame_length;
                return frame_length;
            }
        }
    }
    else
    {
        for (ptr = cache.read_ptr; ptr < cache.buffer_end_ptr - 1; ptr++)
        {

            if ((*ptr == 0x0D) && (*(ptr + 1) == 0x0A))
            {
                frame_length = (int)(ptr - cache.read_ptr);
                if (frame_length > size)
                {
                    return -1;
                }
                memcpy(data, cache.read_ptr, frame_length);
                memset(cache.read_ptr, 0, frame_length);

                if (ptr + 2 >= cache.buffer_end_ptr)
                    cache.read_ptr = cache.buffer;
                else
                    cache.read_ptr = ptr + 2;

                cache.left_size += frame_length;
                return frame_length;
            }
        }
        //不是开头,说明未接收完全一帧。
        if (cache.write_ptr == cache.buffer)
            return 0;

        if (cache.buffer[cache.total_size - 1] == 0x0D && cache.buffer[0] == 0x0A)
        {
            frame_length = cache.buffer_end_ptr - cache.read_ptr;
            frame_length += 1;
            if (frame_length > size)
            {
                return -1;
            }
            //拷贝数据
            memcpy(data, cache.read_ptr, frame_length - 1);
            data[frame_length - 1] = cache.buffer[0];
            //清除
            memset(cache.read_ptr, 0, frame_length - 1);
            cache.buffer[0] = 0;
            cache.read_ptr = cache.buffer + 1;
            cache.left_size += frame_length;
            return frame_length;
        }

        for (ptr = cache.buffer; ptr < cache.write_ptr - 1; ptr++)
        {
            if ((*ptr == 0x0D) && (*(ptr + 1) == 0x0A))
            {
                temp = (int)(cache.buffer_end_ptr - cache.read_ptr);
                frame_length = (int)(ptr + 2 - cache.buffer) + temp;
                if (frame_length > size)
                {
                    return -1;
                }
                memcpy(data, cache.read_ptr, temp);
                memcpy(data + temp, cache.buffer, (int)(ptr + 2 - cache.buffer));

                memset(cache.read_ptr, 0, temp);
                memset(cache.buffer, 0, (int)(ptr + 2 - cache.buffer));

                cache.read_ptr = ptr + 2;

                cache.left_size += frame_length;
                return frame_length;
            }
        }
        return 0;
    }

    return 0;

err:
    return -1;
}

