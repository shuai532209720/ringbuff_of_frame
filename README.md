# ringbuff_of_frame
* 拼包和分包
    主要用来socket、串口等接收出现分多次接收到一帧数据的情况。以ringbuff作为缓冲区。
    * 帧格式暂时是以0x0D 0x0A结尾的帧。
    * 暂时只以帧尾作为完整帧判断标志。

问题:
* 若未接收完完整一包数据，断开了连接，不在发送，会出现一帧错误，或者已接受的数据一直占用内存，直到下次接收完帧尾。
