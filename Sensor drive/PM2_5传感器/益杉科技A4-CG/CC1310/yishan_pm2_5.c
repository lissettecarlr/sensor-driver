
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contiki.h"
#include "dev/cc26xx-uart.h"
#include "yishan_pm2_5.h"
#include "lib/ringbuf.h"


static struct ringbuf PM2_5_Buffer;
static unsigned char PM2_5_buffer_data[128]={0}; //定义了一个U8,128大小的数组
process_event_t PM2_5_data_event;


//static struct PM2_5Data PM2_5;

//int PM=0;
//char PM_String[5];

PROCESS(PM_process,"pm_process");

//AUTOSTART_PROCESSES(&PM_process);

//串口数据读取时的回调函数
int PM2_5_input_byte(unsigned char c)
{
    //包头判断
    static unsigned char flag=0,last=0;
    if(c == 0x32 ) {
        last=0x32;
    } else if(flag == 0 && c == 0x3d) {
        if(last ==0x32) {
            flag =1;
            return 1;
        } else {
            last = 0;
        }
    } else {
        last = 0;
    }

    if(flag) {//包头正确
        ringbuf_put(&PM2_5_Buffer, c); //保存数据
    }
    if(ringbuf_elements(&PM2_5_Buffer)>=30) {//一针数据32个字节，去除包头，要读取30个字节
        process_post(&PM_process, PM2_5_data_event, NULL);
        flag=0;
    }
    return 1;
}

//PM2.5结构体初始化
void PM2_5DataInit()
{
    PM2_5.state = 0 ;
    PM2_5.value = 0 ;
    strcpy(PM2_5.value_String,"0");
}

PROCESS_THREAD(PM_process,ev,data)
{
    PROCESS_BEGIN();

    //初始化
    cc26xx_uart_init();
    cc26xx_uart_set_input(PM2_5_input_byte);
    ringbuf_init(&PM2_5_Buffer,PM2_5_buffer_data,sizeof(PM2_5_buffer_data));
    PM2_5_data_event = process_alloc_event();
    PM2_5DataInit();
    //等待数据接收完成

    while(1)
      {
        PROCESS_YIELD();//等待数据接收完成事件
        if(ev == PM2_5_data_event)
        {
          static unsigned char data[30];
          static unsigned char i;
          for(i=0;i<30;i++)
          {
           data[i]= ringbuf_get(&PM2_5_Buffer);
           //PM = data[4]*256+data[5];//获取PM2.5的值 int型
           PM2_5.value = data[4]*256+data[5];//获取PM2.5的值 int型
           itoa(PM2_5.value,PM2_5.value_String, 10); //转化位字符串
           PM2_5.state = 1;
          }

        }
    }
    PROCESS_END();
}


