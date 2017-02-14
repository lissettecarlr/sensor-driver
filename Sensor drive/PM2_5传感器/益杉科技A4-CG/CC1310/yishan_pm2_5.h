/* 文件：yishan_pm2_5.c
 * 日期：2016/11/25
 * 作者：lissettecarlr
 * 版本：V1.1
 * 说明：该文件是用来驱动益衫A4型PM2.5传感器。该驱动需要外部启动，在启动后会对串口0数据进行分析
 * 当获得有效数据后保存到全局变量中。不会影响printf的使用
 * Add（2016/11/30）增加了数据结构体，使用state来标志传感器是否工作
 * */


#ifndef YISHAN_PM_2_5_H_
#define YISHAN_PM_2_5_H_

#include "contiki.h"

//数据接收事件
extern process_event_t PM2_5_data_event;

//串口接收的回调函数
int PM2_5_input_byte(unsigned char c);

PROCESS_NAME(PM_process);

//PM2.5浓度值，单位ug/M3
//int PM;
//char PM_String[5];

 struct PM2_5Data{
    int  value;
    char value_String[5];
    int  state;//1表示正常读取
}PM2_5;

#endif
