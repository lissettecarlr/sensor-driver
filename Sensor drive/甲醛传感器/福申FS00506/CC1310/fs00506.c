#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contiki.h"
#include "dev/cc26xx-uart.h"
#include "fs00506.h"
#include "lib/ringbuf.h"



static struct ringbuf FS00506_Buffer;
static unsigned char FS00506_Buffer_data[128]={0}; //定义了一个U8,128大小的数组
process_event_t FS00506_data_event;


PROCESS(FS00506_process,"fs00506_process");

//AUTOSTART_PROCESSES(&FS00506_process);

//串口数据读取时的回调函数
int FS00506_input_byte(unsigned char c)
{
    //包头判断
    static unsigned char flag=0,last=0;
    if(c == 0xff ) {
        last=0xff;
    } else if(flag == 0 && c == 0x01) {
        if(last ==0xff) {
            flag =1;
            return 1;
        } else {
            last = 0;
        }
    } else {
        last = 0;
    }

    if(flag) {//包头正确
        ringbuf_put(&FS00506_Buffer, c); //保存数据
    }
    if(ringbuf_elements(&FS00506_Buffer)>=7) {
        process_post(&FS00506_process, FS00506_data_event, NULL);
        flag=0;
    }
    return 1;
}

void FS00506DataInit()
{
	FS00506.state=0;
	FS00506.HCHO_value = 0;
	FS00506.temp_value = 0;
	FS00506.humi_value = 0;
	int i=0;
	for(i=0;i<6;i++)
	{
	  FS00506.byte_data[i]=0;
	}
}

//结构体初始化
void FS00506CheckError()
{
    FS00506.state = 0;
    FS00506.HCHO_value = 0xff ;
	FS00506.temp_value = 0xff ;  
	FS00506.humi_value = 0xff ;  
    int i=0;
	for(i=0;i<6;i++)
	{
	 FS00506.byte_data[i]=0x01;
	}

}


PROCESS_THREAD(FS00506_process,ev,data)
{
    PROCESS_BEGIN();

    //初始化
    cc26xx_uart_init();
    cc26xx_uart_set_input(FS00506_input_byte);
    ringbuf_init(&FS00506_Buffer,FS00506_Buffer_data,sizeof(FS00506_Buffer_data));
    FS00506_data_event = process_alloc_event();
    FS00506DataInit();
    //等待数据接收完成
	
	static unsigned char fs00506_check=0;
	static unsigned char check=0;
    while(1)
      {
        PROCESS_YIELD();//等待数据接收完成事件
        if(ev == FS00506_data_event)
        {
          static unsigned char i;
		  FS00506.state = 1;
		  fs00506_check = 0;
          for(i=0;i<6;i++)
          {
            FS00506.byte_data[i]= ringbuf_get(&FS00506_Buffer);
			fs00506_check+=FS00506.byte_data[i];
          }

		  check = ringbuf_get(&FS00506_Buffer);
          fs00506_check = (~(fs00506_check+0x01))+1;

		 if( fs00506_check == check)
		 {
           FS00506.HCHO_value =(int)FS00506.byte_data[0]*256+FS00506.byte_data[1];  
		   FS00506.temp_value = (int)FS00506.byte_data[2]*256+FS00506.byte_data[3];
		   FS00506.humi_value = (int)FS00506.byte_data[4]*256+FS00506.byte_data[5];
		 }
		 else
		 {
          FS00506CheckError();
		 }
        }
    }
    PROCESS_END();
}


